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
 
#ifndef DATASOURCE_H
#define DATASOURCE_H
#include "Coord2D.h"

/*! \file DataSource.h
	\brief Interfaces for abstract data sources.
	
	Data sources are being used as a means to hide details about where
	data is coming from into objects.  This allows the objects to "forget"
	data when it is not being used and query a data source once the data
	is required.
	
	Ideally, there is no need to know when data gets swapped in for the
	application, just that it happens... like magic!
*/

//! Abstract representation of a source of data
/*!
	Consider a texture object.  Data may come from an image (all too often)
	and need special consideration depending upon the underlying device.
	Alternatively, data may come a generated texture (like a font).
	
	This is not only restricted to textures.  Vertex data may come from files,
	programmatically, etc.  Audio as well.  To a further extent, levels in a
	game may be generated in some cases but loaded in others.
	
	This object attempts to encapsulate the source of the data.  Upon low
	memory, the data sources can release data as they see fit.  They may
	also be called upon to regenerate data after it has been released.  This
	frees the developer from having to manage the resources.
*/
class IDataSource
{
public:

	//! Get a pointer to the data
	/*!
		Data is stored in memory.  Somewhere.  It can be mapped from a file,
		or it can be an internal store.
	*/
	virtual void* data()									= 0;
	
	//! Called whenever memory is low...
	/*!
		On mobile devices, memory is at a premium.  Ideally, we can flush out
		what isn't being used whenever the OS is juggling too many applications
		at once.
	*/
	virtual void onLowMemory()								= 0;
};


//! Specific data source for images
/*!
	We opted to not have metadata interfaces for the DataSource as it would
	look bizarre and messy within the code.  Specialized classes will look
	better.
*/
class IImageDataSource : public IDataSource
{
	//! Size of the image (assumed to be a bitmap)
	/*!
		Size of the image.  It can throw an exception if something
		bad happened during load or generation.
	*/
	virtual Coord2DI size()									= 0;
};


#endif
