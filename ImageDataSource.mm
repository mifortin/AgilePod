/*
   Copyright 2011 Michael Fortin

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

#include "DataSource.h"
#import "SmartMM.h"
#import "Immediate.h"

/*!	\file ImageDataSource.cpp
	\brief Specification of the image data source for images stored in files.
	
	Simply, we route as much as possible to the OS.  It does a good job!
*/

//! Object that represents an image file source
/*! This object is an opaque data type representing an image.
*/
class ImageFileDataSource : public IImageDataSource
{
private:
	OneNS<NSString*>		m_fileName;
	Many<GLubyte>			m_fileData;

public:
	ImageFileDataSource(const char *in_szFile)
	{
		m_fileName = [NSString stringWithUTF8String:in_szFile];
	}
	
	
////////////////////////////////////////////////////////////////////////////////
//	IDataSource
	virtual void *data()
	{
		return NULL;
	}
	
	
	virtual void onLowMemory()
	{
	}


////////////////////////////////////////////////////////////////////////////////
//	IImageDataSource
	virtual Coord2DI size()
	{
		return Coord2DI(0,0);
	}
};


IImageDataSource *CreateImageDataSourceFromFile(const char* in_szFile)
{
	return new ImageFileDataSource(in_szFile);
}
