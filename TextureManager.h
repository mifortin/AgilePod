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

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H


//A way to manage the textures on-demand.
//	We only load up the proper textures based upon screen size information.
class Texture
{
	GLuint m_texID;
public:
	Texture(const char *in_textureName);
	
	//Lazily bind the texture.  That is; it is only bound
	//upon rendering - and only if the texture ID changed.
	inline GLuint use()
	{
		return gl.useTexture(m_texID);
	}
	
	virtual ~Texture()
	{
		glDeleteTextures(1, &m_texID);
	}
};

#endif
