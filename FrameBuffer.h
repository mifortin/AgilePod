/*
   Copyright 2010,2011 Michael Fortin

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

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

#include "Immediate.h"
#include "Coord2D.h"

/*!	\file	FrameBuffer.h
	\brief	Methods to simplify render-to-texture operations

	Render-to-texture is often used to do special effects.  It is bound as a
	texture using the BindTexture object.  The RenderToTarget object will
	change the rendering path from what it was to this FrameBuffer.
*/
	

class BindTexture;
class RenderToTarget;

//! Provides the ability to render to texture
/*!
	Provides a means to render to a texture.
 
	\code
//Create a new frame buffer of width (64,64)
FrameBuffer fb(64,64);

//Render to the frame buffer:
{
	RenderToTarget r(fb);	//Stop rendering to fb when r goes out of scope
}

//Use fb as a texture
{
	BindTexture bt(fb);		//fb is now the current texture
}
	\endcode
*/
class FrameBuffer
{
private:
	//The OpenGL texture ID that we use with the frame buffer....
	GLuint			m_texID;
	GLuint			m_fbID;
	
	//!The size of the texture	(to create on first render)
	Coord2DI		m_size;
	
	//!The size as a power of two (or npot if supported)
	Coord2DI		m_pot;
	
	//Lazy initializer
	void 			lazyInit();

	//To minimize the public interface...
	friend class RenderToTarget;
	friend class BindTexture;
	
	//! Use this FrameBuffer (implementation in TextureManager.mm)
	GLuint use(int in_index);

public:
	//!Creates a new FrameBuffer
	/*!		\param	size[in]	Size of the FrameBuffer. */
	FrameBuffer(Coord2DI in_size);
	
	//!Creates a FrameBuffer attached to a RenderBuffer.
	/*!	In this case, initialization is not delayed, and this is bound as the
		default FrameBuffer object.  (eg. when nothing else is bound,
		this is the object that is bound to draw to screen)
	 
		Typically, this is used to set the renderBuffer that draws to the screen.
		When a RenderToTarget completes, it reverts to the default FrameBuffer
		object (draw to screen) when not nested.
	 */
	FrameBuffer(GLuint in_renderBuffer);
	
	//! Copy the FrameBuffer (RGBA as 8*bit) into the given buffer.
	/*!	\param out_dest[out]	Where to copy byte data.
								Should have bufferSizeInBytes() bytes. */
	void downloadFrameBuffer(int8_t *out_dest);
	
	//!The size of the buffer (minimum required for download)
	int bufferSizeInBytes()	{		return m_size.x*m_size.y*4;	}
	
	//! What is the size of the image within the buffer
	/*!	May be smaller than size on systems that don't support NPOT */
	inline Coord2D			sizeInTexture()
	{
		return Coord2D(	(float)m_size.x / (float)m_pot.x,
						(float)m_size.y / (float)m_pot.y);
	}
	
	//!Frees the FrameBuffer object
	~FrameBuffer();
	
	//! Size of the FrameBuffer
	const Coord2DI size()	const	{	return  m_size;		}
	
	//! Use the OpenGL ES 2.x functions
	static void useOpenGLES2();
};

//! Sets a FrameBuffer object as the target for OpenGL rendering calls.
/*! RenderToTarget sets up a frame buffer to be drawn to.  Once it goes out
	of scope, it will revert the frane buffer to draw to the previous target.
*/
class RenderToTarget
{
private:
	//! Stack of frame-buffer objects.
	FrameBuffer *m_prev;			//What we wish to restore...

public:
	//!FrameBuffer that we will render to
	RenderToTarget(FrameBuffer *in_fb);
	
	//!When we're done with the FrameBuffer... (restore parameters)
	~RenderToTarget();
};

#endif
