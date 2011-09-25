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

#import "TextureManager.h"
#import <CoreFoundation/CoreFoundation.h>
#import "Immediate.h"
#import "SmartMM.h"


//Texture info...
static GLuint g_boundTexture	= 0;

Texture::Texture(const char *in_textureName)
: m_texID(0)
{
	m_data = CreateImageDataSourceFromFile(in_textureName);
}


Texture::Texture(IImageDataSource *in_ds)
: m_texID(0)
, m_data(in_ds)
{
	assert(in_ds != NULL);
}


GLuint Texture::use()
{
	if (m_texID == 0)	lazyLoad();
	
	GLuint r = g_boundTexture;
	
	if (g_boundTexture != m_texID)
	{
		glBindTexture(GL_TEXTURE_2D, m_texID);
		g_boundTexture = m_texID;
	}
	
	return r;
}


GLuint FrameBuffer::use()
{
	if (m_texID == 0)	lazyInit();
	
	GLuint r = g_boundTexture;
	
	if (g_boundTexture != m_texID)
	{
		glBindTexture(GL_TEXTURE_2D, m_texID);
		g_boundTexture = m_texID;
	}
	
	return r;
}


void Texture::lazyLoad()
{
	Coord2DI s = m_data()->size();
	
	Coord2DI npt2 = s;
	
	//Quick hack to get next power of 2 on a 32-bit machine
	//	(see http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2 )
	//	(and http://acius2.blogspot.com/2007/11/calculating-next-power-of-2.html )
	//
	//		Essentially sets all bits less than the current bit to 1 and adds 1.
	npt2.x--;
	npt2.x = (npt2.x >> 1) | npt2.x;
	npt2.x = (npt2.x >> 2) | npt2.x;
	npt2.x = (npt2.x >> 4) | npt2.x;
	npt2.x = (npt2.x >> 8) | npt2.x;
	npt2.x = (npt2.x >> 16) | npt2.x;
	npt2.x++;
	
	npt2.y--;
	npt2.y = (npt2.y >> 1) | npt2.y;
	npt2.y = (npt2.y >> 2) | npt2.y;
	npt2.y = (npt2.y >> 4) | npt2.y;
	npt2.y = (npt2.y >> 8) | npt2.y;
	npt2.y = (npt2.y >> 16) | npt2.y;
	npt2.y++;
	//End quick hack
	
	m_npt2 = npt2;
	
	int width = s.x;
	int height = s.y;
	
	glGenTextures(1, &m_texID);
	if (m_texID == 0)	throw "Texture::lazyLoad::Failed creating texture!";
	
	//Slightly recursive, but is elegant
	{
		BindTexture bt(this);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, npt2.x, npt2.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, m_data()->data());
		m_data()->releaseData();
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_data()->minFilter());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_data()->magFilter());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_data()->wrapU());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_data()->wrapV());
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, m_data()->generateMipmap());
	}
}


void Texture::checkForMoreData()
{
	if (m_texID == 0)	return;
	
	Coord2DI s = size();
	
	if (m_data()->hasUpdatedData())
	{
		BindTexture bt(this);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s.x, s.y, GL_RGBA, GL_UNSIGNED_BYTE, m_data()->data());
		m_data()->releaseData();
	}
}



BindTexture::~BindTexture()
{
	if (g_boundTexture != m_prev)
	{
		glBindTexture(GL_TEXTURE_2D, m_prev);
		g_boundTexture = m_prev;
	}
}
