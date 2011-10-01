/*
   Copyright 2010, 2011 Michael Fortin

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
#include "GPUExtensions.h"


#include <OpenGLES/ES2/gl.h>

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
	glGenFramebuffers(1, &m_fbID);
	
	BindTexture bt(this);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	if (GPU::Support::NPOT())
	{
		m_pot = m_size;
	}
	else
	{
		m_pot = m_size;
		
		m_pot.x--;
		m_pot.x = (m_pot.x >> 1) | m_pot.x;
		m_pot.x = (m_pot.x >> 2) | m_pot.x;
		m_pot.x = (m_pot.x >> 4) | m_pot.x;
		m_pot.x = (m_pot.x >> 8) | m_pot.x;
		m_pot.x = (m_pot.x >> 16) | m_pot.x;
		m_pot.x++;
		
		m_pot.y--;
		m_pot.y = (m_pot.y >> 1) | m_pot.y;
		m_pot.y = (m_pot.y >> 2) | m_pot.y;
		m_pot.y = (m_pot.y >> 4) | m_pot.y;
		m_pot.y = (m_pot.y >> 8) | m_pot.y;
		m_pot.y = (m_pot.y >> 16) | m_pot.y;
		m_pot.y++;
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_pot.x, m_pot.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbID);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texID, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("failed to make complete framebuffer object %x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		throw "Failed creating frame buffer object";
	}
	
}


FrameBuffer::FrameBuffer(Coord2DI in_size)
: m_size(in_size)
, m_texID(0)
, m_fbID(0)
{}


FrameBuffer::FrameBuffer(GLuint in_renderBuffer)
{
	assert(g_curFB == NULL);
	g_curFB = this;

	glGenFramebuffers(1, &m_fbID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbID);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER_OES, in_renderBuffer);
	
	int width, height;
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
	
	m_size.x = width;
	m_size.y = height;
	
	m_pot = m_size;
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("failed to make complete framebuffer object %x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		throw "Failed creating frame buffer object";
	}
	
	glViewport(0, 0, m_size.x, m_size.y);
	
	//gl.specifyDeviceSize(m_width, m_height);
}


void FrameBuffer::downloadFrameBuffer(int8_t *out_dest)
{
	assert(g_curFB != NULL);
	
	if (this != g_curFB)
	{
		RenderToTarget t(this);
		glReadPixels(	0,0, m_size.x, m_size.y, GL_RGBA,
						GL_UNSIGNED_BYTE,
						out_dest);
	}
	else
	{
		glReadPixels(	0,0, m_size.x, m_size.y, GL_RGBA,
						GL_UNSIGNED_BYTE,
						out_dest);
	}
}


FrameBuffer::~FrameBuffer()
{
	if (g_curFB == this)
	{
		g_curFB = NULL;
	}
	
	if (m_texID != 0)
		glDeleteTextures(1, &m_texID);
	
	if (m_fbID != 0)
		glDeleteFramebuffers(1, &m_fbID);
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
		glBindFramebuffer(GL_FRAMEBUFFER, in_fb->m_fbID);
	
	glViewport(0,0,in_fb->m_size.x, in_fb->m_size.y);
}


RenderToTarget::~RenderToTarget()
{
	g_curFB = m_prev;
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_prev->m_fbID);
	glViewport(0, 0, m_prev->m_size.x, m_prev->m_size.y);
}
