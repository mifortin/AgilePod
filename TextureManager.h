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

static GLuint g_appTexture		= 0;
static GLuint g_miniTexture		= 0;

void SetAppTexture(GLuint in_appTexture)
{
	g_appTexture = in_appTexture;
}


void SetMiniTexture(GLuint in_miniTexture)
{
	g_miniTexture = in_miniTexture;
}


static void useAppTexture()
{
	glBindTexture(GL_TEXTURE_2D, g_appTexture);
}

static void useMiniTexture()
{
	glBindTexture(GL_TEXTURE_2D, g_miniTexture);
}


//A way to manage the textures on-demand.
//	We only load up the proper textures based upon screen size information.
class Texture
{
	GLuint m_texID;
public:
	

	inline void bind()
	{
		gl.bindTexture(m_texID);
	}
};

#endif
