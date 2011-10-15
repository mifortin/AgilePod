/*
 *	Copyright 2011 Michael Fortin
 *
 *	 Licensed under the Apache License, Version 2.0 (the "License");
 *	 you may not use this file except in compliance with the License.
 *	 You may obtain a copy of the License at
 *	 
 *		http://www.apache.org/licenses/LICENSE-2.0
 *	 
 *	 Unless required by applicable law or agreed to in writing, software
 *	 distributed under the License is distributed on an "AS IS" BASIS,
 *	 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	 See the License for the specific language governing permissions and
 *	 limitations under the License.
 */

#ifndef FirstTest_Error_h
#define FirstTest_Error_h

#include <stdexcept>
#include <cstdarg>


//! Base class for errors.
/*!	This object will allow the specification of string formatting like C
	for easier descriptions of data */
class APError : public std::exception
{
private:
	std::string m_err;
	
public:
	//! Creates a new APError...
	APError(const char *in_fmt, ...) throw();
	
	//! Explicitly specify throw
	virtual ~APError() throw()	{}
	
	//! Return the string (like std::exception likes)
	virtual const char* what() const throw();
};


#endif
