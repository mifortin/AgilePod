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

#include "Immediate.h"
#include "FrameBuffer.h"
#include "DataSource.h"
#include "Camera.h"

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

/*!	\file TextureManager.h
	\brief	Basic texture management
	
	Objects connecting visuals to the graphics system.
*/

//A way to manage the textures on-demand.
//	We only load up the proper textures based upon screen size information.
class BindTexture;

//! Represents a texture.  That is an object in the GPUs video memory.
/*! A texture is backed by an object of type IImageDataSource.  These
	can be custom data sources that do procedural rendering or a static image.
	Static images can be loaded using CreateImageDataSourceFromFile.
*/
class Texture
{
private:
	//!The OpenGL texture ID that we use	( 0 = not a texture)
	GLuint 				m_texID;
	
	//!The size (as a power of 2)
	Coord2DI			m_npt2;
	
	//!The backing object
	RCOne<IImageDataSource>	m_data;
	
	//!Reduce public surface for texturing...
	friend class BindTexture;
	
	//!Method that actually does the creating
	void lazyLoad();
	
	//!Lazily bind the texture.
	/*!That is; it is only bound upon rendering - and only if the texture ID changed. */
	GLuint use();
	
public:
	//! Create a new texture from a file
	/*! \param in_textureName[in]	The file to load */
	Texture(const char *in_textureName);
	
	//! Create a new texture from an arbitrary data source
	/*! \param in_ds[in] The data source.  It must not be NULL */
	Texture(IImageDataSource *in_ds);
	
	//! Determine the size of the image
	/*! \return Coord2DI representing size of image */
	inline Coord2DI			size()
	{
		return m_data()->size();
	}
	
	//! Call to check for new data
	/*!	This is not done automatically since it is preferred to start loading
		near the end of a frame. */
	void checkForMoreData();
	
	//! Clear the texture.
	virtual ~Texture()
	{
		if (m_texID != 0)
			glDeleteTextures(1, &m_texID);
	}
};


//A way to safely bind textures...
class BindTexture
{
private:
	GLuint m_prev;

public:
	BindTexture(Texture *t)
	{
		m_prev = t->use();
	}
	
	BindTexture(FrameBuffer *f)
	{
		m_prev = f->use();
	}
	
	void rebind(Texture *t)
	{
		t->use();
	}
	
	void rebind(FrameBuffer *f)
	{
		f->use();
	}
	
	//! Restore the texture
	~BindTexture();
};
#endif
