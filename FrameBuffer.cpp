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

////////////////////////////////////////////////////////////////////////////////
//
//	Store information about the current frame buffer here.  This will
//	allow us to swap in/out the frame buffer at will.
//
static FrameBuffer *g_curFB = NULL;

////////////////////////////////////////////////////////////////////////////////
//

void FrameBuffer::lazyInit()
{
	glGenTextures(1, &m_texID);
	glGenFramebuffersOES(1, &m_fbID);
	
	GLuint prev = gl.useTexture(m_texID);
	gl.uploadImageData(m_width, m_height);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, m_fbID);
	
	glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, m_texID, 0);
	
	if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
	{
		printf("failed to make complete framebuffer object %x\n", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		throw "Failed creating frame buffer object";
	}
	
	gl.useTexture(prev);
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

	glGenFramebuffersOES(1, &m_fbID);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, m_fbID);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, in_renderBuffer);
	
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &m_width);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &m_height);
	
	if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
	{
		printf("failed to make complete framebuffer object %x\n", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
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
		glDeleteFramebuffersOES(1, &m_fbID);
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
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, in_fb->m_fbID);
	
	glViewport(0,0,in_fb->m_width, in_fb->m_height);
}


RenderToTarget::~RenderToTarget()
{
	g_curFB = m_prev;
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, m_prev->m_fbID);
	glViewport(0, 0, m_prev->m_width, m_prev->m_height);
}
