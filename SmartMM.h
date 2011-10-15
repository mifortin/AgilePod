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

#ifndef SMARTMM_H
#define SMARTMM_H

/*! \file SmartMM.h
	\brief Smart pointers to help deal with Objective-C memory management
	
	Retaining and releasing Objective-C objects is menial labour
	that can be automated using C++.  We use a C++ constructor (or assignment)
	to retain and the destructor to release.  Optimally, this object is inlined
	providing no degradation in performance compared to manually doing the
	retain and releasing in code.
	
	Realistically, the overhead is calling autorelease on various objects and
	that the Objective-C bridge, as I understand it, creates a method for
	initialization and destruction of C++ objects.  As such, in Objective-C,
	there is a slight overhead in run-time.
	
	The slight overhead is nothing compared to the time saved by reducing
	the chances of making mistakes with retaining and releasing.
 
	There is a wrapper called OneNSArray specifically for NSArray objects.
	It provides C++ style array offsets and type information.
*/

//! Smart pointer for Objective-C objects
/*!
	\ingroup SmartPointers
	
	This object takes an Objective-C object and automatically calls retain
	an release on it.  Passed in objects are assumed to be autoreleased.
	
	\code
//Create a string set to be released by the current auto-release pool
NSString *myString = [NSString stringWithFormat:@"%s %s", "Hello", "World"];

{
	//Implicit retain of myString (through constructor)
	OneNS<NSString> myRef(myString);
	
	//Call methods on string...
	[myRef() length];
	
	//Implicit release of myString (through destructor)
}
	\endcode
 
 
	This object should be seen as deprecated.  Older projects that do not use
	ARC should define NO_ARC 
*/
template<class T>
class OneNS
{
private:
	//! Private reference to Objective-C object
	T	m_one;
	
public:
	//! Copy constructor
	/*! \param cpy[in]		OneNS object to copy */
	OneNS(const OneNS &cpy)
	{
		m_one = cpy.m_one;
#ifdef NO_ARC
		if (m_one)		[m_one retain];
#endif
	}
	
	//! Create a new smart pointer
	/*! \param in_one[in]	Default value of nil, a reference of the object to retain */
	OneNS(T in_one = nil)
	{
		m_one = in_one;
#ifdef NO_ARC
		if (m_one)		[m_one retain];
#endif
	}
	
	//! Assign a different pointer
	/*! \param in_init[in]	New reference to retain or nil
		\return	The previous reference and releasing it once. */
	inline T operator=(T in_init)
	{
#ifdef NO_ARC
		[in_init retain];
		if (m_one)		[m_one release];
#endif
		
		m_one = in_init;
		
		return m_one;
	}
	
	//! Compare two references.
	/*! \param in_cmp[in]	Reference to compare
		\return true if the addresses of the internal reference and in_cmp
				are the same. */
	inline bool operator==(T in_cmp)
	{
		return in_cmp == m_one;
	}
	
	//! Not equal
	/*! \param in_cmp[in]	Reference to compare with
		\return	true if the address is different */
	inline bool operator!=(T in_cmp)
	{
		return in_cmp != m_one;
	}
	
	//! Overloaded function operator
	/*! Use this to access the value of the underlying objective-C reference. */
	inline T operator()() const		{	return m_one;	}
	
	//! Nicver way to get the underlying value
	inline T v() const				{	return m_one;	}
	
	//! Destructor releases held instance.
#ifdef NO_ARC
	virtual ~OneNS()
	{
		if (m_one)		[m_one release];
	}
#endif
};


//! Smart pointer for core foundation objects
template<class T>
class OneCF
{
private:
	//! Private refrence to the data.
	T	m_one;
	
public:
	//! Copy constructor
	/*! \param cpy[in]		OneNS object to copy */
	OneCF(const OneCF &cpy)
	{
		m_one = cpy.m_one;
		if (m_one)		CFRetain(m_one);
	}
	
	//! Create a new smart pointer
	/*! \param in_one[in]	Default value of nil, a reference of the object to retain */
	OneCF(T in_one = nil)
	{
		m_one = in_one;
		if (m_one)		CFRetain(m_one);
	}
	
	//! Assign a different pointer
	/*! \param in_init[in]	New reference to retain or nil
	 \return	The previous reference and releasing it once. */
	inline T operator=(T in_init)
	{
		if (in_init)	CFRetain(in_init);
		if (m_one)		CFRelease(m_one);
		
		m_one = in_init;
		
		return m_one;
	}
	
	//! Compare two references.
	/*! \param in_cmp[in]	Reference to compare
	 \return true if the addresses of the internal reference and in_cmp
	 are the same. */
	inline bool operator==(T in_cmp)
	{
		return in_cmp == m_one;
	}
	
	//! Not equal
	/*! \param in_cmp[in]	Reference to compare with
	 \return	true if the address is different */
	inline bool operator!=(T in_cmp)
	{
		return in_cmp != m_one;
	}
	
	//! Overloaded function operator
	/*! Use this to access the value of the underlying objective-C reference. */
	inline T operator()() const		{	return m_one;	}
	
	//! Nicver way to get the underlying value
	inline T v() const				{	return m_one;	}
	
	//! Destructor releases held instance.
	virtual ~OneCF()
	{
		if (m_one)		CFRelease(m_one);
	}
};


//! Abstract list object
/*!	\tparam The type of list.  Single-type lists makes code easier later */
template<class T>
class IList : public RC
{
public:
	//! Get access to an element
	virtual T operator[](int in_index)				= 0;
	
	//! Check the count
	virtual int count()								= 0;
	
	//! Virtual destructor
	virtual ~IList()	{}
};

/*!
 A wrapper around an NSArray.  This provides a way to see the type as we're
 coding.
 
 \tparam	T	The type contained within the NSArray
 */
template<class T>
class OneNSArray : public OneNS<NSArray*>, public IList<T>
{
public:
	//! Provide a convenient C++ operator to get to the elements, properly typed
	inline T operator[](int in_index)
	{ return (T)[v() objectAtIndex:in_index];	}
	
	//! An easy way to get the count
	inline int count()
	{ return [v() count];	}
	
	//! Tell GCC about operator overloading...
	inline NSArray* operator=(NSArray *in_init)
	{	return OneNS<NSArray*>::operator=(in_init);	}
	
	//! Forward constructor
	OneNSArray(NSArray* in_data) : OneNS<NSArray*>(in_data)		{}
	
	//! Forward copy-constructor
	OneNSArray(const OneNSArray &cpy) : OneNS<NSArray*>(cpy)	{}
};

#endif
