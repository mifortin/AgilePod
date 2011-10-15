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

#include "APError.h"

APError::APError(const char *in_fmt, ...) throw()
{
	va_list v1;
	va_start(v1, in_fmt);
	
	char *a = NULL;
	vasprintf(&a, in_fmt, v1);
	
	if (a == NULL)	throw std::bad_alloc();
	
	m_err = a;
	
	free(a);
	
	va_end(v1);
}


const char* APError::what() const throw()
{
	return m_err.c_str();
}

