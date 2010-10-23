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
#include <CoreFoundation/CoreFoundation.h>

/*
	Restorable
	
	Purpose
		Provide a means for objects to expose their hierarchy to Restorer.
*/
class Restorer;
class Restorable
{
public:
	virtual void handle(Restorer *in_restore) = 0;
};


/*
	Restorer
	
	Purpose:
		Provide a simpler means to save / restore the game state (and more).
	
	Method:
		Backed by a property-list, key-value pairs are saved to the file
		then restored as needed.
	
	Notes:
		Using CoreFoundation to appear like a pure C++ file from callers.
*/
class Restorer
{
public:
	
	//In the case of objects, we just need the object - default values
	//are assigned within the object...
	virtual void Object(CFStringRef in_key, Restorable *in_object)				= 0;
	
	//The following basic data types can easily be saved & restored
	//	Each takes in a key, a target, and a default value.
	//
	//	In the case of restore
	//		if a key does not exist in the file, the default value is taken.
	//		else we always load what's in the file.
	//
	//	In the case of save
	//		target is written to file under the key.  default is ignored.
	virtual void Int(CFStringRef in_key, int *io_value, int in_default)			= 0;
	virtual void Float(CFStringRef in_key, float *io_value, float in_default)	= 0;
	
	//Needed...
	virtual ~Restorer()	{}
};


/*
	CreateSaveObject
	
	Purpose
		Creates a new Restorer object that will accumulate data and save
		it to the specified file.
	
	Notes:
		You must call delete to free resources and actually save the data
	
	Error
		Throws a string describing the error
*/
Restorer *CreateSaveObject(CFStringRef in_fileName);

/*
	CreateRestoreObject
	
	Purpose
		Creates a new Restorer object that will restore data from the
		save file.
	
	Notes:
		You must call delete to free associated resources
	
	Error
		Throws a string describing the error
*/
Restorer *CreateRestoreObject(CFStringRef in_fileName);

#endif
