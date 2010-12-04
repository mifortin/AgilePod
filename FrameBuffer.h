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

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include "Immediate.h"

////////////////////////////////////////////////////////////////////////////////
//
//	FrameBuffer manages a FrameBuffer.
//
class BindTexture;
class RenderToTarget;
class FrameBuffer
{
private:
	//The OpenGL texture ID that we use with the frame buffer....
	GLuint			m_texID;
	GLuint			m_fbID;
	
	//The size of the texture	(to create on first render)
	int				m_width, m_height;
	
	//Lazy initializer
	void 			lazyInit();

	//To minimize the public interface...
	friend class RenderToTarget;
	friend class BindTexture;
	
	inline GLuint use()
	{
		if (m_texID == 0)	lazyInit();
		return gl.useTexture(m_texID);
	}

public:
	//Creates a new FrameBuffer
	//	width:	width of the buffer
	//	height:	height of the buffer
	FrameBuffer(int in_width, int in_height);
	
	//Creates a FrameBuffer attached to a RenderBuffer.
	//	In this case, initialization is not delayed, and this is bound as the
	//	default FrameBuffer object.  (eg. when nothing else is bound,
	//	this is the object that is bound to draw to screen)
	FrameBuffer(GLuint in_renderBuffer);
	
	//Captures the contents of a FrameBuffer to a buffer.
	void downloadFrameBuffer(int8_t *out_dest);
	
	//The size of the buffer (minimum required for download)
	int bufferSize()	{		return m_width*m_height*4;	}
	
	//Frees the FrameBuffer object
	~FrameBuffer();
	
	//Getters...
	int width()			{		return m_width;		}
	int height()		{		return m_height;	}
};


////////////////////////////////////////////////////////////////////////////////
//
//	RenderToTarget sets up a frame buffer to be drawn to.  Once it goes out
//	of scope, it will revert the frane buffer to draw to the previous target.
//
class RenderToTarget
{
private:
	FrameBuffer *m_prev;			//What we wish to restore...

public:
	//FrameBuffer that we will render to
	RenderToTarget(FrameBuffer *in_fb);
	
	//When we're done with the FrameBuffer... (restore parameters)
	~RenderToTarget();
};

#endif
