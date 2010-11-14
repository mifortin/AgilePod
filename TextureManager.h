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

#include "Immediate.h"
#include "FrameBuffer.h"
#include "Smart.h"
#include "Camera.h"

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H


//A way to manage the textures on-demand.
//	We only load up the proper textures based upon screen size information.
class BindTexture;
class Texture
{
private:
	//The OpenGL texture ID that we use	( 0 = not a texture)
	GLuint 				m_texID;
	
	//We only create the texture when needed...
	OneCMalloc<char>	m_fileName;
	
	//Allow a way to get the size...
	Coord2D				m_size;
	
	//Call this to actually load the image(s)
	void lazyLoad();
	
	//Reduce public surface for texturing...
	friend class BindTexture;
	//Lazily bind the texture.  That is; it is only bound
	//upon rendering - and only if the texture ID changed.
	inline GLuint use()
	{
		if (m_texID == 0)	lazyLoad();
		
		return gl.useTexture(m_texID);
	}
	
public:
	Texture(const char *in_textureName);
	
	inline Coord2D			size()	const	{	return m_size;	}
	
	virtual ~Texture()
	{
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

	~BindTexture()
	{
		gl.useTexture(m_prev);
	}
};
#endif
