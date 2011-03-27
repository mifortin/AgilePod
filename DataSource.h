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
#include "Smart.h"
#include "Immediate.h"

/*! \file DataSource.h
	\brief Interfaces for abstract data sources.
	
	Data sources are being used as a means to hide details about where
	data is coming from into objects.  This allows the objects to "forget"
	data when it is not being used and query a data source once the data
	is required.
	
	Ideally, there is no need to know when data gets swapped in for the
	application, just that it happens... like magic!
*/

/*! \defgroup DataSource DataSource: Loading generated or external data into the system.
	
	IDataSource is the most basic interface that everything inherits from.
	
	IImageDataSource specializes IDataSource for image data.
*/


//! Abstract representation of a source of data
/*!
	\ingroup DataSource
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
class IDataSource : public RC
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
	\ingroup DataSource
	We opted to not have metadata interfaces for the DataSource as it would
	look bizarre and messy within the code.  Specialized classes will look
	better.
	
	Use \ref CreateImageDataSourceFromFile to create an IImageDataSource from
	a file within the application bundle.
	
	If IDataSource::data() returns NULL, then we assume subregions are uploaded
*/
class IImageDataSource : public IDataSource
{
public:
	//! Size of the image (assumed to be a bitmap)
	/*!
		Size of the image.  It can throw an exception if something
		bad happened during load or generation.
	*/
	virtual Coord2DI size()									= 0;
	
	//! Min filter
	/*!	The filter to apply when making the texture smaller. 
		By default, this is GL_LINEAR.  This value is set once. */
	virtual int minFilter()			{	return GL_LINEAR;			}
	
	//! Mag filter
	/*! The filter to apply when enlarging the texture.
		By default, we set this to GL_LINEAR.  This value is set once. */
	virtual int magFilter()			{	return GL_LINEAR;			}
	
	//! Texture Wrap U
	/*!	Sets the texture wrap mode.  We like GL_CLAMP_TO_EDGE.
		This value is set once -- changing the return value has no effect. */
	virtual int wrapU()				{	return GL_CLAMP_TO_EDGE;	}
	
	//! Texture Wrap V
	/*! Default is GL_CLAMP_TO_EDGE.
		This value is set once -- changing the return value has no effect. */
	virtual int wrapV()				{	return GL_CLAMP_TO_EDGE;	}
	
	//! Automagically generate mipmaps?
	/*! Default is GL_FALSE.
		This value is set once -- changing the return value has no effect. */
	virtual int generateMipmap()	{	return GL_FALSE;			}
};


/*! \page ExampleLoadTextureFile Example How to Load Textures
	Here's how to load a texture from a file:
	
	\code
		Texture *x = new Texture(CreateImageDataSourceFromFile("FileName"));
	\endcode
*/

//! Creates an image data source from a file
/*!
	\ingroup DataSource
	We assume that the file is of type PNG.  Upon calling this function, we
	simply return a new object (caller has to free) that saves the file
	name.  Upon a call to "data", the requested data is loaded and presented.
	
	All cached data is destroyed once onLowMemory is called.
	
	Depending upon the device, we load up a different file.
	Files ending with ~iPad load the iPad version, ~iPhone load the
	iPhone/iPod version.  Adding in @2 specifies that displays, like
	on the iPhone 4 is present.
	
	For example, a file called "Stars" will attempt to load the following
	on an iPhone 4 (in the given order).
	-# Stars~iPhone@2.png
	-# Stars~iPhone.png
	-# Stars.png
 
	Sometimes, an image may be shared on two platforms.  Two get around this,
	within the plist, the ~iPad@1 key can be used to specify a special image
	for an iPad with scale 1.  Default is to pick image name that is more
	specific to a given device then to use the more general default.
 
	For example, specifying ~iPhone@1 and ~iPhone will specify a default image
	for low-resolutions iPhones (and iPods) with high resolutions for any iPhone
	or iPod with a retina or better display.
 
	Specifying ~iPhone@2 and ~iPhone would default higher-than-retina displays
	to use the low-resolution image specified by ~iPhone.
	
	If none of these sources leads to an image, then image loading is considered
	to be a failure.
	
	Blending modes can be specified in a file called TextureMap.plist, which
	could like the example code below:
	\code
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<!-- Image named Stars -->
	<key>Stars</key>
	<dict>
		<!-- Set the min filter to nearest -->
		<key>GL_TEXTURE_MIN_FILTER</key>
		<string>GL_NEAREST</string>
		
		<!-- Set the texture wrapping to repeat along the width -->
		<key>GL_TEXTURE_WRAP_S</key>
		<string>GL_REPEAT</string>
	</dict>
</dict>
</plist>
	\endcode
	
	For clarity, we opted to use the same names as specified in the OpenGL
	ES 1.1 Reference Pages.  See the documentation for glTexParameter for
	further information on what the keys & available values are and their
	respective effects.
	
	\return Reference to the image data source.  Be sure to retain the
			data source using the RCOne object.
*/
IImageDataSource *CreateImageDataSourceFromFile(const char *in_szFile);

#endif
