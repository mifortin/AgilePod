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

#ifndef SMART_H
#define SMART_H

//A simple smart-pointer.  We have a reference to an object.  When a new
//object is added, we delete the previous and save the new reference.
//
//	Usage:	One<Object>		instanceName
//			instanceName = new Object();	//deletes existing...
//			if (instanceName())				//true if backed by non-null pointer
//
template<class T>
class One
{
private:
	T *m_one;
	
	//Prevent copies (bad things can happen!)
	One(One &toCopy)	{}

public:
	One(T *in_init = NULL)
	: m_one(in_init)
	{}
	
	inline T* operator=(T *in_init)
	{
		if (m_one)	delete m_one;
		m_one = in_init;
		
		return m_one;
	}
	
	inline T* operator()()	const	{	return m_one;	}
	
	inline T* operator->()	const	{	return m_one;	}
	
	virtual ~One()	{	if(m_one)		delete m_one;	}
};

#endif
