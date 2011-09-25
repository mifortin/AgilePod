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

#include "FrameBuffer.h"

#include "Immediate.h"
#include "TextureManager.h"


#include <OpenGLES/ES2/gl.h>

////////////////////////////////////////////////////////////////////////////////
//
//	Store information about the current frame buffer here.  This will
//	allow us to swap in/out the frame buffer at will.
//
static FrameBuffer *g_curFB = NULL;


// The following are prototypes of functions and GL constants that change
//	between OpenGL ES 1x and OpenGL ES 2x.
static void (*apGenFramebuffers)(GLsizei n, GLuint * framebuffers)	= glGenFramebuffersOES;
static void (*apBindFramebuffer)(GLenum target, GLuint framebuffer)	= glBindFramebufferOES;
static void (*apFramebufferTexture2D)(	GLenum target,
										GLenum attachment,
										GLenum textarget,
										GLuint texture,
										GLint level)				= glFramebufferTexture2DOES;
static GLenum (*apCheckFramebufferStatus)(GLenum target)			= glCheckFramebufferStatusOES;
static void (*apFramebufferRenderbuffer)(	GLenum target,
											GLenum attachment,
											GLenum renderbuffertarget,
											GLuint renderbuffer)	= glFramebufferRenderbufferOES;
static void (*apGetRenderbufferParameteriv)(GLenum target,
											GLenum pname,
											GLint * params)			= glGetRenderbufferParameterivOES;
static void (*apDeleteFramebuffers)(GLsizei n,
									const GLuint * framebuffers)	= glDeleteFramebuffersOES;



// Just in case the symbols are different...
//	The constants are the same
void FrameBuffer::useOpenGLES2()
{
	apGenFramebuffers = glGenFramebuffers;
	apBindFramebuffer = glBindFramebuffer;
	apFramebufferTexture2D = glFramebufferTexture2D;
	apCheckFramebufferStatus = glCheckFramebufferStatus;
	apFramebufferRenderbuffer = glFramebufferRenderbuffer;
	apGetRenderbufferParameteriv = glGetRenderbufferParameteriv;
	apDeleteFramebuffers = glDeleteFramebuffers;
}


////////////////////////////////////////////////////////////////////////////////
//

void FrameBuffer::lazyInit()
{
	glGenTextures(1, &m_texID);
	BindTexture bt(this);
	apGenFramebuffers(1, &m_fbID);
		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	apBindFramebuffer(GL_FRAMEBUFFER_OES, m_fbID);
	
	apFramebufferTexture2D(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, m_texID, 0);
	
	if (apCheckFramebufferStatus(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
	{
		printf("failed to make complete framebuffer object %x\n", apCheckFramebufferStatus(GL_FRAMEBUFFER_OES));
		throw "Failed creating frame buffer object";
	}
	
}


FrameBuffer::FrameBuffer(int in_width, int in_height)
: m_width(in_width)
, m_height(in_height)
, m_texID(0)
, m_fbID(0)
{}


FrameBuffer::FrameBuffer(GLuint in_renderBuffer)
{
	assert(g_curFB == NULL);
	g_curFB = this;

	apGenFramebuffers(1, &m_fbID);
	apBindFramebuffer(GL_FRAMEBUFFER_OES, m_fbID);
	apFramebufferRenderbuffer(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, in_renderBuffer);
	
	apGetRenderbufferParameteriv(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &m_width);
	apGetRenderbufferParameteriv(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &m_height);
	
	if (apCheckFramebufferStatus(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
	{
		printf("failed to make complete framebuffer object %x\n", apCheckFramebufferStatus(GL_FRAMEBUFFER_OES));
		throw "Failed creating frame buffer object";
	}
	
	glViewport(0, 0, m_width, m_height);
	
	gl.specifyDeviceSize(m_width, m_height);
}


void FrameBuffer::downloadFrameBuffer(int8_t *out_dest)
{
	assert(g_curFB != NULL);
	
	if (this != g_curFB)
	{
		RenderToTarget t(this);
		glReadPixels(	0,0, m_width, m_height, GL_RGBA,
						GL_UNSIGNED_BYTE,
						out_dest);
	}
	else
	{
		glReadPixels(	0,0, m_width, m_height, GL_RGBA,
						GL_UNSIGNED_BYTE,
						out_dest);
	}
}


FrameBuffer::~FrameBuffer()
{
	if (m_texID != 0)
		glDeleteTextures(1, &m_texID);
	
	if (m_fbID != 0)
		apDeleteFramebuffers(1, &m_fbID);
}


////////////////////////////////////////////////////////////////////////////////
//

RenderToTarget::RenderToTarget(FrameBuffer *in_fb)
{
	m_prev = g_curFB;
	g_curFB = in_fb;
	
	if (in_fb->m_fbID == 0)
		in_fb->lazyInit();
	else
		apBindFramebuffer(GL_FRAMEBUFFER_OES, in_fb->m_fbID);
	
	glViewport(0,0,in_fb->m_width, in_fb->m_height);
}


RenderToTarget::~RenderToTarget()
{
	g_curFB = m_prev;
	
	apBindFramebuffer(GL_FRAMEBUFFER_OES, m_prev->m_fbID);
	glViewport(0, 0, m_prev->m_width, m_prev->m_height);
}
