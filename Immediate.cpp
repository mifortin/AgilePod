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

#import "Immediate.h"

gli gl;
gliColour gliColourWhite(255,255,255,255);



//! Previously applied blending modes
static GLenum g_blendSrc = GL_ONE, g_blendDst = GL_ZERO;

gliBlendFunc::gliBlendFunc(GLenum in_src, GLenum in_dst)
: m_oldSrc(g_blendSrc)
, m_oldDst(g_blendDst)
{
	blendFunc(in_src, in_dst);
}

void gliBlendFunc::blendFunc(GLenum in_src, GLenum in_dst)
{
	g_blendDst = in_dst;
	g_blendSrc = in_src;
	glBlendFunc(in_src, in_dst);
}
