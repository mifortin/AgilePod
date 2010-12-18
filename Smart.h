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

#include <stdio.h>
#include <stdlib.h>

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


//A simple smart-pointer for arrays of data to be deleted...
template<class T>
class Many
{
private:
	T *m_many;
	
	//Prevent copies (bad things can happen!)
	Many(Many &toCopy)	{}

public:
	Many(T *in_init = NULL)
	: m_many(in_init)
	{}
	
	inline T* operator=(T *in_init)
	{
		if (m_many)	delete m_many;
		m_many = in_init;
		
		return m_many;
	}
	
	inline T* operator()()	const	{	return m_many;	}
	
	inline T* operator->()	const	{	return m_many;	}
	
	virtual ~Many()	{	if(m_many)		delete []m_many;	}
};


//Implementation of the One object for standard C objects.
//	The reason to not use polymorphism is to enable the compiler to see
//	more means to optimize the code.  Also removes any virtual functions.
//	Essentially, I'd like to use these smart pointers in performance-
//	critical code.
//
//	T	- the type that we want to auto-release
//	REL	- the free function / test function class, see examples below...
template<class T, class REL>
class OneC
{
private:
	T m_one;
	OneC(OneC &toCopy)	{}

public:
	OneC(T in_init = (T)REL::defaultValue())
	: m_one(in_init)
	{}
	
	inline T operator=(T in_init)
	{
		if (!REL::isNull(m_one))
			REL::freeResource(m_one);
		
		m_one = in_init;
		
		return m_one;
	}
	
	inline T operator()() const	{	return m_one;	}
	inline T operator->() const	{	return m_one;	}
	
	virtual ~OneC()	{	if (!REL::isNull(m_one))	REL::freeResource(m_one);	}
};


//OneC specialized for things that go NULL...
class OneCNullSpecification
{
public:
	static bool isNull(void *f)
	{
		return f == NULL;
	}
	
	static void* defaultValue()
	{
		return NULL;
	}
};


//OneC specification for malloc'd data
template<class T>
class OneCMallocSpecification : public OneCNullSpecification
{
public:
	static void freeResource(T d)
	{
		free(d);
	}
};


//OneC specialized for C FILE objects
//	Note that specializations simply need to implement a single-parameter
//	freeResources and isNull function.  Also needs a function to return the
//	default value.
class OneCFileReleaseSpecification : public OneCNullSpecification
{
public:
	static void freeResource(FILE *f)
	{
		fclose(f);
	}
};


//Use OneCFile in lieu of FILE* C objects.  This will automatically close the
//file upon assigning NULL and at the end of scope.
typedef		OneC<FILE*, OneCFileReleaseSpecification>			OneCFile;

//OneCMalloc handles malloced data...
template<class T>
class OneCMalloc : public OneC<T*, OneCMallocSpecification<T*> >
{
public:
	T *malloc(int number)
	{
		return OneC<T*, OneCMallocSpecification<T*> >::operator=((T*)calloc(sizeof(T),number));
	}
	
	T *operator[](int in_index)
	{
		return OneC<T*, OneCMallocSpecification<T*> >::operator()[in_index];
	}
};

#endif
