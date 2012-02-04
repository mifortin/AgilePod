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
#ifndef BLIT_H
#define BLIT_H

#include <stdlib.h>
#include "Immediate.h"
#include <sys/time.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#include <stdio.h>
#include <math.h>



//Much simpler blit function!
template<int W, int H>
static void blit(Coord2D in_tex, Coord2D in_dest)
{
	const float FW = (float)W;
	const float FH = (float)H;
	{
		Draw d(GL_TRIANGLE_STRIP);
		d.texCoordi(in_tex.x*FW, in_tex.y*FH);
		d.vertex(in_dest.x, in_dest.y);
		
		d.texCoordi(in_tex.x*FW+FW, in_tex.y*FH);
		d.vertex(in_dest.x+FW, in_dest.y);
		
		d.texCoordi(in_tex.x*FW, in_tex.y*FH+FH);
		d.vertex(in_dest.x, in_dest.y+FH);
		
		d.texCoordi(in_tex.x*FW+FW, in_tex.y*FH+FH);
		d.vertex(in_dest.x+FW, in_dest.y+FH);
	}
}


static void blitS(float in_x, float in_y, float in_w, float in_h, float d_x, float d_y,
		   float in_dw, float in_dh)
{
	{
		Draw d(GL_TRIANGLE_STRIP);
		d.texCoordi(in_x, in_y);
		d.vertex(d_x, d_y);
		
		d.texCoordi(in_x+in_w, in_y);
		d.vertex(d_x+in_dw, d_y);
		
		d.texCoordi(in_x, in_y+in_h);
		d.vertex(d_x, d_y+in_dh);
		
		d.texCoordi(in_x+in_w, in_y+in_h);
		d.vertex(d_x+in_dw, d_y+in_dh);
	}
}


static void fillRect(float d_x, float d_y, float in_dw, float in_dh)
{
	gliDisableTexture glETexture;
	gliDisableTexCoordArray glETextureCoord;
	
	{
		Draw d(GL_TRIANGLE_STRIP);
		d.vertex(d_x, d_y);
		d.vertex(d_x+in_dw, d_y);
		d.vertex(d_x, d_y+in_dh);
		d.vertex(d_x+in_dw, d_y+in_dh);
	}
}


static void drawText32(float in_sx, float in_sy,
				float in_x, float in_y,
				float in_w, float in_h,
				const char *in_sz_uch)
{
	in_sx /= 1024.0f;
	in_sy /= 1024.0f;
	
	const unsigned char *in_sz = (const unsigned char*)in_sz_uch;
	
	while (*in_sz)
	{
		float x1 = -1;
		float y1 = -1;
		if (*in_sz >= 'a' && *in_sz <= 'z')
		{
			x1 = 16.0f / 1024.0f;
			y1 = (*in_sz - 'a') * 32;
			y1 /= 1024.0f;
		}
		else if (*in_sz >= 'A' && *in_sz <= 'Z')
		{
			x1 = 0;
			y1 = (*in_sz - 'A') * 32;
			y1 /= 1024.0f;
		}
		else if (*in_sz == '\'')
		{
			x1 = 0;
			y1 = ('z'-'a'+1)*32 / 1024.0f;
		}
		else if (*in_sz == 0xc3)
		{
			in_sz++;
			switch(*in_sz)
			{
				case 0x80:	//À
					x1 = 0;
					y1 = ('z'-'a'+3)*32 / 1024.0f;
					break;
					
				case 0x89: 	//É
					x1 = 0;
					y1 = ('z'-'a'+2)*32 / 1024.0f;
					break;
					
				case 0x8A:	//Ê
					x1 = 0;
					y1 = ('z'-'a'+4)*32 / 1024.0f;
					break;
					
				case 0xa0:	//à
					x1 = 16.0f / 1024.0f;
					y1 = ('z'-'a'+3)*32 / 1024.0f;
					break;
					
				case 0xa9:	//é
					x1 = 16.0f / 1024.0f;
					y1 = ('z'-'a'+2)*32 / 1024.0f;
					break;
					
				case 0xaa:	//ê
					x1 = 16.0f / 1024.0f;
					y1 = ('z'-'a'+4)*32 / 1024.0f;
					break;
			}
		}
		
		if (! (x1 < 0 || y1 < 0))
		{
			float data[20] = {
				in_x,		in_y,		0,	in_sx+x1+0.0001f,				in_sy+y1+0.0001f,
				in_x+in_w,	in_y,		0,	in_sx+x1+16.0f/1024.0f-0.0001f,	in_sy+y1+0.0001f,
				in_x,		in_y+in_h,	0,	in_sx+x1+0.0001f,				in_sy+y1+32.0f/1024.0f-0.0001f,
				in_x+in_w, in_y+in_h,	0,	in_sx+x1+16.0f/1024.0f-0.0001f,	in_sy+y1+32.0f/1024.0f-0.0001f};
			
			glVertexPointer(3,GL_FLOAT,sizeof(float)*5,data);
			glTexCoordPointer(2,GL_FLOAT,sizeof(float)*5,data+3);
			glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		}
		
		in_x += in_w;
		
		in_sz++;
	}
}

static float frand()
{
	return (float)rand() / (float)RAND_MAX;
}


#endif
