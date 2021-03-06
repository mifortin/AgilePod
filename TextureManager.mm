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
#include "GPUExtensions.h"


//Texture info...
static GLuint g_boundTexture[4]	= {0,0,0,0};

Texture::Texture(const char *in_textureName)
: m_texID(0)
, m_texSecond(0)
{
	m_data = CreateImageDataSourceFromFile(in_textureName);
}


Texture::Texture(IImageDataSource *in_ds)
: m_texID(0)
, m_data(in_ds)
, m_texSecond(0)
{
	assert(in_ds != NULL);
}


GLuint Texture::use(int in_index)
{
	if (m_texID == 0)	lazyLoad();
	
	GLuint r = g_boundTexture[in_index];
	
	if (g_boundTexture[in_index] != m_texID)
	{
		glActiveTexture(GL_TEXTURE0 + in_index);
		glBindTexture(GL_TEXTURE_2D, m_texID);
		g_boundTexture[in_index] = m_texID;
	}
	
	return r;
}


GLuint FrameBuffer::use(int in_index)
{
	if (m_texID == 0)	lazyInit();
	
	GLuint r = g_boundTexture[in_index];
	
	if (g_boundTexture[in_index] != m_texID)
	{
		glActiveTexture(GL_TEXTURE0 + in_index);
		glBindTexture(GL_TEXTURE_2D, m_texID);
		g_boundTexture[in_index] = m_texID;
	}
	
	return r;
}


void Texture::lazyLoad()
{
	Coord2DI s = m_data()->size();
	
	m_size = s;
	Coord2DI npt2 = s;
	
	if (GPU::Support::NPOT() &&
		(m_data->minFilter() == GL_LINEAR || m_data->minFilter() == GL_NEAREST) &&
		(m_data->magFilter() == GL_LINEAR || m_data->minFilter() == GL_NEAREST) &&
		(m_data->wrapU() == GL_CLAMP_TO_EDGE) &&
		(m_data->wrapV() == GL_CLAMP_TO_EDGE) &&
		!(m_data->generateMipmap() == GL_TRUE))
	{
		m_npt2 = npt2;		//Power of 2 texture OK!
	}
	else
	{
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
	}
	
	int width = s.x;
	int height = s.y;
	
	glGenTextures(1, &m_texID);
	if (m_texID == 0)	throw "Texture::lazyLoad::Failed creating texture!";
	
	//Slightly recursive, but is elegant
	{
		BindTexture bt(this);
		
		// More efficient to set parameters first!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_data()->minFilter());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_data()->magFilter());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_data()->wrapU());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_data()->wrapV());
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, m_data()->generateMipmap());
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, npt2.x, npt2.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, m_data()->data());
		m_data()->releaseData();
		
	}
}


void Texture::checkForMoreData()
{
	if (m_texID == 0)	return;
	
	if (m_data()->hasUpdatedData())
	{
		Coord2DI s = size();
		
		if (m_texSecond == 0)
		{
			m_texSecond = m_texID;
			glGenTextures(1, &m_texID);
			
			BindTexture bt(this);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_data()->minFilter());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_data()->magFilter());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_data()->wrapU());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_data()->wrapV());
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, m_data()->generateMipmap());
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_npt2.x, m_npt2.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}
		else
		{
			GLuint tmp = m_texID;
			m_texID = m_texSecond;
			m_texSecond = tmp;
		}
		
		BindTexture bt(this);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s.x, s.y, GL_RGBA, GL_UNSIGNED_BYTE, m_data()->data());
		m_data()->releaseData();
	}
}



BindTexture::~BindTexture()
{
	if (g_boundTexture[m_index] != m_prev)
	{
		glActiveTexture(GL_TEXTURE0 + m_index);
		glBindTexture(GL_TEXTURE_2D, m_prev);
		g_boundTexture[m_index] = m_prev;
	}
}
