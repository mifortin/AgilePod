/*
   Copyright 2010 Michael Fortin

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#ifndef RESTORER_H
#define RESTORER_H

#include "Smart.h"

//Object responsible for providing access to a binary buffer to save and restore
//data.
class FileData
{
protected:
	OneCFile	m_filePtr;

public:
	//Builds a new FileData.
	FileData(char *in_srcFile, bool readonly = true);
};


class Save : public FileData
{
private:

public:
	Save(char *in_srcFile)
	: FileData(in_srcFile, false)
	{}
	
	//The key is used internally to allow skipping data that might
	//not have been defined previously.
	//	(I suggest 4cc codes with a clear "end" key for each object)
	//
	//	0 is a reserved key used for internal purposes.
	template<class T>
	void write(int key, const T &value)
	{
		if (fwrite(&key, sizeof(key), 1, m_filePtr()) != sizeof(key))
			throw "Failed writing key!";
		
		short sizeOfValue = sizeof(value);
		if (fwrite(&sizeOfValue, sizeof(sizeOfValue), 1, m_filePtr())
			!= sizeof(sizeOfValue))
			throw "Failed writing size of value!";
		
		if (fwrite(&value, sizeof(value), 1, m_filePtr()) != sizeof(value))
			throw "Failed writing value!";
	}
};


class Restore : public FileData
{
private:
	int m_nextKey;
	short m_nextSize;
	
	void readNextKey()
	{
		if (m_filePtr() != NULL)
		{
			m_nextKey = 0;
			m_nextSize = 0;
			
			if (	fread(&m_nextKey, sizeof(m_nextKey), 1, m_filePtr())
						!= sizeof(m_nextKey)
				||	fread(&m_nextSize, sizeof(m_nextSize), 1, m_filePtr())
						!= sizeof(m_nextSize))
			{
				m_filePtr = NULL;
				return;
			}
		}
	}

public:
	Restore(char *in_srcFile)
	: FileData(in_srcFile, true)
	, m_nextKey(0)
	{
		readNextKey();
	}
	
	//Simple way to read in keys.  We assume keys were stored in sequential
	//order - and that keys are added, not removed - also that data-structures
	//retain their original size.  (if size changes, versions of data-structures
	//should be introduced)
	template<class T>
	void read(int key, T &value, T defaultValue)
	{
		if (key != m_nextKey)
		{
			value = defaultValue;
		}
		else if (sizeof(value) != m_nextSize)
			throw "Size of data structure changed!";
		else
		{
			if (fread(&value, sizeof(value), 1, m_filePtr()) != sizeof(value))
				throw "Incomplete data structure found in file.";
			
			readNextKey();
		}
	}
};

#endif
