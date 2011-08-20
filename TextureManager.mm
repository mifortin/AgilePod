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


void Texture::lazyLoad()
{
	Coord2DI s = m_data()->size();
	int width = s.x;
	int height = s.y;
	
	glGenTextures(1, &m_texID);
	if (m_texID == 0)	throw "Texture::lazyLoad::Failed creating texture!";
	
	//Slightly recursive, but is elegant
	{
		BindTexture bt(this);
		
		gl.uploadImageData(width, height, m_data()->data());
		m_data()->releaseData();
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_data()->minFilter());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_data()->magFilter());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_data()->wrapU());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_data()->wrapV());
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, m_data()->generateMipmap());
	}
}
