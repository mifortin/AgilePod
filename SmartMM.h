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

#ifndef SMARTMM_H
#define SMARTMM_H

/*
	OneNS
		Smart-pointer for ObjectiveC objects
*/
template<class T>
class OneNS
{
private:
	T	m_one;

public:
	OneNS(const OneNS &cpy)
	{
		m_one = cpy.m_one;
		if (m_one)		[m_one retain];
	}
	
	OneNS(T in_one = nil)
	{
		m_one = in_one;
		if (m_one)		[m_one retain];
	}
	
	inline T operator=(T in_init)
	{
		if (in_init)	[in_init retain];
		if (m_one)		[m_one release];
		
		m_one = in_init;
		
		return m_one;
	}
	
	inline bool operator==(T in_cmp)
	{
		return in_cmp == m_one;
	}
	
	inline bool operator!=(T in_cmp)
	{
		return in_cmp != m_one;
	}
	
	inline T operator()() const		{	return m_one;	}
	
	virtual ~OneNS()
	{
		if (m_one)		[m_one release];
	}
};

#endif
