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

//x-y 			Origin of ellipse
//rStart/End	start/end radius of ellipse
//u-v			Origin of texture coordinates
//tStart/End	start/end of texture coord
//tS?			scales render coordinates into texture coordinates
template<int increments>
static void drawEllipse(const Coord2D in_position,
						const float rStart, const float rEnd,
						const int u, const int v,
						const float tStart, const float tEnd,
						const float tSx, const float tSy,
						const gliColourUnsignedByte innerColor = gliColourWhite,
						const gliColourUnsignedByte outerColor = gliColourWhite)
{
	const float angleIncrement = M_PI*2/(float)increments;

	float a;
	for (a=0; a<M_PI*2; a+= angleIncrement)
	{
		//2 triangles...
		const float ax1 = cos(a);
		const float ax2 = cos(a + angleIncrement);
		const float ay1 = sin(a);
		const float ay2 = sin(a + angleIncrement);
		
		//Compute vertices
		const float vax1 = ax1*rStart + in_position.x;
		const float vax2 = ax2*rStart + in_position.x;
		const float vay1 = ay1*rStart + in_position.y;
		const float vay2 = ay2*rStart + in_position.y;
		
		const float vax1X = ax1*rEnd + in_position.x;
		const float vax2X = ax2*rEnd + in_position.x;
		const float vay1X = ay1*rEnd + in_position.y;
		const float vay2X = ay2*rEnd + in_position.y;
		
		//Warp texture coordinates
		const float tvax1 = ax1*tStart + in_position.x;
		const float tvax2 = ax2*tStart + in_position.x;
		const float tvay1 = ay1*tStart + in_position.y;
		const float tvay2 = ay2*tStart + in_position.y;
		
		const float tvax1X = ax1*tEnd + in_position.x;
		const float tvax2X = ax2*tEnd + in_position.x;
		const float tvay1X = ay1*tEnd + in_position.y;
		const float tvay2X = ay2*tEnd + in_position.y;
		
		const int tax1 = tvax1 * tSx + u;
		const int tax2 = tvax2 * tSx + u;
		const int tay1 = tvay1 * tSy + v;
		const int tay2 = tvay2 * tSy + v;
		
		const int tax1X = tvax1X * tSx + u;
		const int tax2X = tvax2X * tSx + u;
		const int tay1X = tvay1X * tSy + v;
		const int tay2X = tvay2X * tSy + v;
		
		gl.colour(innerColor);
		gl.texCoordi(tax1, tay1);
		gl.vertex(vax1, vay1);
		gl.colour(outerColor);
		gl.texCoordi(tax2X, tay2X);
		gl.vertex(vax2X, vay2X);
		gl.texCoordi(tax1X, tay1X);
		gl.vertex(vax1X, vay1X);
		
		
		gl.colour(innerColor);
		gl.texCoordi(tax1, tay1);
		gl.vertex(vax1, vay1);
		gl.colour(outerColor);
		gl.texCoordi(tax2X, tay2X);
		gl.vertex(vax2X, vay2X);
		gl.colour(innerColor);
		gl.texCoordi(tax2, tay2);
		gl.vertex(vax2, vay2);
	}
}


//Much simpler blit function!
template<int W, int H>
static void blit(Coord2D in_tex, Coord2D in_dest)
{
	const float FW = (float)W;
	const float FH = (float)H;
	gl.begin(GL_TRIANGLE_STRIP);
	{
		gl.texCoordi(in_tex.x*FW, in_tex.y*FH);
		gl.vertex(in_dest.x, in_dest.y);
		
		gl.texCoordi(in_tex.x*FW+FW, in_tex.y*FH);
		gl.vertex(in_dest.x+FW, in_dest.y);
		
		gl.texCoordi(in_tex.x*FW, in_tex.y*FH+FH);
		gl.vertex(in_dest.x, in_dest.y+FH);
		
		gl.texCoordi(in_tex.x*FW+FW, in_tex.y*FH+FH);
		gl.vertex(in_dest.x+FW, in_dest.y+FH);
	}
	gl.end();
}


static void blitS(float in_x, float in_y, float in_w, float in_h, float d_x, float d_y,
		   float in_dw, float in_dh)
{
	gl.begin(GL_TRIANGLE_STRIP);
	{
		gl.texCoordi(in_x, in_y);
		gl.vertex(d_x, d_y);
		
		gl.texCoordi(in_x+in_w, in_y);
		gl.vertex(d_x+in_dw, d_y);
		
		gl.texCoordi(in_x, in_y+in_h);
		gl.vertex(d_x, d_y+in_dh);
		
		gl.texCoordi(in_x+in_w, in_y+in_h);
		gl.vertex(d_x+in_dw, d_y+in_dh);
	}
	gl.end();
}


static void fillRect(float in_x, float in_y, float in_w, float in_h)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_COORD_ARRAY);
	
	float data[12] = {
		in_x,		in_y,		0,
		in_x+in_w,	in_y,		0,
		in_x,		in_y+in_h,	0,
		in_x+in_w,	in_y+in_h,	0
	};
	
	glVertexPointer(3,GL_FLOAT,sizeof(float)*3,data);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_COORD_ARRAY);
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


static double x_time()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	
	return (double)t.tv_sec + ((double)t.tv_usec) / 1000000.0;
}

#endif
