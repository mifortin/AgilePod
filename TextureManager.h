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
	
	//!Second buffer for streaming data to the GPU
	GLuint				m_texSecond;
	
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
	
	//! Load the image (prevent lazy loading)
	/*!	The GPU likes it when things are loaded during a warm-up phase.  This
		is the warm-up phase!	*/
	inline void load()	{	if (m_texID == 0)	lazyLoad();		}
	
	//! Call to check for new data
	/*!	This is not done automatically since it is preferred to start loading
		near the end of a frame.
	 
		Upon calling this method, the object will create a second buffer to
		not force a flush of the current rendering pipeline and will upload
		at the image to the second buffer.
	 
		It is most optimal to call this method at the end of a frame.
	 
		Internally, it is assumed that the return texture modes are not changed
		(remained constant) from the data source.
	 */
	void checkForMoreData();
	
	//! Clear the texture.
	virtual ~Texture()
	{
		if (m_texID != 0)
			glDeleteTextures(1, &m_texID);
		
		if (m_texSecond != 0)
			glDeleteTextures(1, &m_texSecond);
	}
};


//! Binds a texture to the current active texture unit (usually assumed 0)
/*!	This object is assumed to exist on the stack (and never the heap).  In
	such situations, it binds the texture upon instantation within a block
	and upon leaving the block it restores the previously bound texture.	*/
class BindTexture
{
private:
	//! Previous texture (accumulated on stack)
	GLuint m_prev;

public:
	//! Binds a texture object
	BindTexture(Texture *t)
	{
		m_prev = t->use();
	}
	
	//! Binds a frame buffer object
	BindTexture(FrameBuffer *f)
	{
		m_prev = f->use();
	}
	
	//! Binds another texture (more efficient than nesting)
	void rebind(Texture *t)
	{
		t->use();
	}
	
	//! Binds another framebuffer (more efficient than nesting)
	void rebind(FrameBuffer *f)
	{
		f->use();
	}
	
	//! Restore the texture upon destruction.
	~BindTexture();
};


#endif
