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
   
/*
 *	A restricted implementation of GL immediate mode for iTouch.
 */

#ifndef IMMEDIATE_H
#define IMMEDIATE_H

#ifndef POSITION_MULT
#define POSITION_MULT	2
#endif

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import "Camera.h"

#define ALIGN(x)	__attribute__((aligned(x/8)))

////////////////////////////////////////////////////////////////////////////////
//
//	OpenGL Colour Object
//
class gliColour
{
	GLubyte m_r, m_g, m_b, m_a;
public:
	
	gliColour(float ir, float ig=0, float ib=0, float ia=0)
	{
		m_r = 255*ir;
		m_g = 255*ig;
		m_b = 255*ib;
		m_a = 255*ia;
	}
	
	gliColour(GLubyte ir=0, GLubyte ig=0, GLubyte ib=0, GLubyte ia=0)
	{
		m_r = ir;
		m_g = ig;
		m_b = ib;
		m_a = ia;
	}
	
	gliColour(int ir, int ig = 0, int ib = 0, int ia = 0)
	{
		m_r = ir;
		m_g = ig;
		m_b = ib;
		m_a = ia;
	}
	
	GLubyte &r()	{	return m_r;	}
	GLubyte &g()	{	return m_g;	}
	GLubyte &b()	{	return m_b;	}
	GLubyte &a()	{	return m_a;	}
} ALIGN(32);


////////////////////////////////////////////////////////////////////////////////
//
//	OpenGL 3D Position Object
//
class gliPosition3D
{
	GLshort m_x, m_y, m_z;	
public:
	gliPosition3D(float ix, float iy=0, float iz=0)
	: m_x(ix*POSITION_MULT)
	, m_y(iy*POSITION_MULT)
	, m_z(iz*POSITION_MULT)
	{}
	
	gliPosition3D(GLshort ix=0, GLshort iy=0, GLshort iz=0)
	: m_x(ix)
	, m_y(iy)
	, m_z(iz)
	{}
} ALIGN(32);


////////////////////////////////////////////////////////////////////////////////
//
//	OpenGL Texture Coordinate
//
class gliTexCoord
{	
	GLshort m_u, m_v;
public:
	gliTexCoord(float iu, float iv=0)
	: m_u(iu*1023)
	, m_v(iv*1023)
	{}
	
	gliTexCoord(GLshort iu=0, GLshort iv=0)
	: m_u(iu)
	, m_v(iv)
	{}
} ALIGN(32);



//Aggregate object that holds all the needed data for submission to the GL
class gliSubmit
{
public:
	gliPosition3D		position;
	gliColour			colour;
	gliTexCoord			texCoord;
}ALIGN(32);


//Prepare...
class gliBlendFunc;
template<int G, int I> class gliEnable;
template<int G, int I> class gliDisable;

typedef gliEnable<GL_TEXTURE_2D,			0>	gliEnableTexture;
typedef gliEnable<GL_BLEND,					1>	gliEnableBlendFunc;
typedef gliEnable<GL_COLOR_ARRAY,			2>	gliEnableColorArray;
typedef gliEnable<GL_TEXTURE_COORD_ARRAY,	3>	gliEnableTexCoordArray;


typedef gliDisable<GL_TEXTURE_2D,			0>	gliDisableTexture;
typedef gliDisable<GL_BLEND,				1>	gliDisableBlendFunc;
typedef gliDisable<GL_COLOR_ARRAY,			2>	gliDisableColorArray;
typedef gliDisable<GL_TEXTURE_COORD_ARRAY,	3>	gliDisableTexCoordArray;


//Each of these objects are specific to the GLI object.  That is they
//define the internal structure!
class gli
{
private:
	//The OpenGL submission.
	gliSubmit submission[4096];
	
	//Immediate texture coordinate and color
	gliTexCoord 		m_texCoord;
	
	gliColour 			m_colour;
	
	
	//Texture info...
	GLuint m_textureID;
	GLuint m_boundTexture;
	
	//Current blend mode
	GLenum m_srcBlend;
	GLenum m_dstBlend;
	
	GLenum m_boundSrcBlend;
	GLenum m_boundDstBlend;
	
	//Device info...
	int		m_width;
	int		m_height;
	float	m_scale;
	
	
	//Drawing mode...
	short m_mode;
	short m_curVertex;


////////////////////////////////////////////////////////////////////////////////
	//Push a new blending mode
	inline void blendFunc(GLenum in_srcBlend, GLenum in_dstBlend)
	{
		m_srcBlend = in_srcBlend;
		m_dstBlend = in_dstBlend;
	}
	
	inline GLenum srcBlend() const		{	return m_srcBlend;				}
	inline GLenum dstBlend() const		{	return m_dstBlend;				}


////////////////////////////////////////////////////////////////////////////////
	//Enable/disable...
	friend class gliBlendFunc;
	friend class gliEnable<GL_TEXTURE_2D, 	 0	>;
	friend class gliEnable<GL_BLEND,		 1	>;
	friend class gliEnable<GL_COLOR_ARRAY,	 2	>;
	friend class gliEnable<GL_TEXTURE_COORD_ARRAY,	 3	>;
	
	char m_enable[4];					//Our logical state
	char m_pvtEnable[4];				//Actual state..
	
public:
	gli()
	: m_textureID(0)
	, m_boundTexture(0)
	, m_width(0)
	, m_height(0)
	, m_scale(1)
	, m_srcBlend(GL_ONE)
	, m_dstBlend(GL_ONE)
	, m_boundSrcBlend(GL_ONE)
	, m_boundDstBlend(GL_ONE)
	{
		memset(m_enable, 0, sizeof(m_enable));
		memset(m_pvtEnable, 0, sizeof(m_pvtEnable));
	}
	
	//Update device info
	inline void specifyDeviceSize(int in_width, int in_height)
	{
		m_width = in_width;
		m_height = in_height;
	}
	
	inline void specifyDeviceScale(float in_scale)
	{
		m_scale = in_scale;
	}
	
	inline int deviceWidth()					{	return m_width;					}
	inline int deviceHeight()					{	return m_height;				}
	inline float deviceScale()					{	return m_scale;					}

	//add texture coordinates...
	inline void texCoord(float u, float v=0)	{	m_texCoord = gliTexCoord(u,v);		}
	inline void texCoordi(GLshort u, GLshort v=0)		{	m_texCoord = gliTexCoord(u,v);		}
	
	inline void colour(float r, float g=0, float b=0, float a=0)
	{	m_colour = gliColour(r,g,b,a);		}
	inline void colouri(GLubyte r=0, GLubyte g=0, GLubyte b=0, GLubyte a=0)
	{	m_colour = gliColour(r,g,b,a);		}
	inline void colour(const gliColour &in_color)
	{	m_colour = in_color;				}
	
	//Call this if binding only needs to occur on the next draw call.
	//	(hint - usually call this function)
	inline GLuint useTexture(GLuint in_textureID)
	{
		int r = m_textureID;
		m_textureID = in_textureID;
		return r;
	}
	
	//Upload RGBA image data for a texture.
	inline void uploadImageData(void *imageData, int width, int height, int mipmap=0)
	{
		if (m_boundTexture !=  m_textureID)
		{
			m_boundTexture = m_textureID;
			glBindTexture(GL_TEXTURE_2D, m_textureID);
		}
		
		glTexImage2D(GL_TEXTURE_2D, mipmap, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	}
	
	inline void vertex(float x, float y=0, float z=0)
	{
		submission[m_curVertex].position = gliPosition3D(x,y,z);
		submission[m_curVertex].colour = m_colour;
		submission[m_curVertex].texCoord = m_texCoord;
		m_curVertex++;
	}
	
	inline void pushMatrix()
	{
		glPushMatrix();
	}
	
	inline void popMatrix()
	{
		glPopMatrix();
	}
	
	inline void vertexi(GLshort x=0, GLshort y=0, GLshort z=0)
	{
		submission[m_curVertex].position = gliPosition3D(x,y,z);
		submission[m_curVertex].colour = m_colour;
		submission[m_curVertex].texCoord = m_texCoord;
		m_curVertex++;
	}
	
	inline void translate(float dx, float dy=0, float dz = 0)
	{
		glTranslatef(dx*POSITION_MULT, dy*POSITION_MULT, dz*POSITION_MULT);
	}
	
	inline void translate(const Coord2D in_t)
	{
		translate(in_t.x, in_t.y);
	}
	
	inline void scale(const Coord2D in_t)
	{
		glScalef(in_t.x, in_t.y, 1);
	}

	//Start and end a shape...
	inline void begin(short mode)
	{
		m_mode = mode;
		m_curVertex = 0;
	}

	inline void end()
	{
		//Check for texturing...
		if (m_enable[0] != m_pvtEnable[0])
		{
			m_pvtEnable[0] = m_enable[0];
			if (m_enable[0])
				glEnable(GL_TEXTURE_2D);
			else
				glDisable(GL_TEXTURE_2D);
		}
		
		if (m_enable[0])
		{
			if (m_boundTexture !=  m_textureID)
			{
				m_boundTexture = m_textureID;
				glBindTexture(GL_TEXTURE_2D, m_textureID);
			}
		}
		
		//Check for blending...
		if (m_enable[1] != m_pvtEnable[1])
		{
			m_pvtEnable[1] = m_enable[1];
			if (m_enable[1])
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);
		}
		
		if (m_enable[1])
		{
			if (m_boundSrcBlend != m_srcBlend || m_boundDstBlend != m_dstBlend)
			{
				m_boundSrcBlend = m_srcBlend;
				m_boundDstBlend = m_dstBlend;
				glBlendFunc(m_srcBlend, m_dstBlend);
			}
		}
	
		//Set up colour arrays...
		if (m_enable[2] != m_pvtEnable[2])
		{
			m_pvtEnable[2] = m_enable[2];
			if (m_enable[2])
				glEnableClientState(GL_COLOR_ARRAY);
			else
				glDisableClientState(GL_COLOR_ARRAY);
		}
		if (m_enable[2])
			glColorPointer(		4, GL_UNSIGNED_BYTE,
								sizeof(submission[0]),
								&(submission[0].colour));
		
		//Set up texture coordinate arrays...
		if (m_enable[3] != m_pvtEnable[3])
		{
			m_pvtEnable[3] = m_enable[3];
			if (m_enable[3])
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			else
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (m_enable[3])
			glTexCoordPointer(	2, GL_SHORT,
								sizeof(submission[0]),
								&(submission[0].texCoord));
		
		//Vertices and drawing the arrays at the end...
		glVertexPointer(	3, GL_SHORT,
							sizeof(submission[0]),
							&(submission[0].position));

		glDrawArrays(m_mode, 0, m_curVertex);
	}
	
}ALIGN(32);

extern gli gl;
extern gliColour gliColourWhite;


static void gliTextureMatrixSetup()
{
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glScalef(1.0f/1024.0f,1.0f/1024.0f,1.0f);
}

static void gliProjectionSetup()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

static void gliModelSetup()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


////////////////////////////////////////////////////////////////////////////////
//
//	OpenGL Blending Mode
//		Usage: 	instantiate the object.  It will enable the given
//				blending modes and disable them once it gets popped off
//				the stack.
//
class gliBlendFunc
{
	GLenum m_oldSrc;
	GLenum m_oldDst;

public:
	gliBlendFunc(GLenum in_src=GL_ONE, GLenum in_dst=GL_ZERO)
	: m_oldSrc(gl.srcBlend())
	, m_oldDst(gl.dstBlend())
	{
		gl.blendFunc(in_src, in_dst);
	}
	
	inline void blendFunc(GLenum in_src=GL_ONE, GLenum in_dst=GL_ZERO)
	{
		gl.blendFunc(in_src, in_dst);
	}
	
	inline ~gliBlendFunc()
	{
		gl.blendFunc(m_oldSrc, m_oldDst);
	}
};


////////////////////////////////////////////////////////////////////////////////
//
//	OpenGL Enable Disable
//		Usage:	enables and disables GL states.
//				use the provided typedefs...
//		Impl:	we use a template system - an index into an array in
//				the global GL object, and the GL enable/disable key.
//
template<int GL_MODE, int INDEX>
class gliEnable
{
	char m_prevState;
public:
	gliEnable(bool in_enable = true)
	{
		m_prevState = gl.m_enable[INDEX];
		gl.m_enable[INDEX] = in_enable ? 1 : 0;
	}
	
	inline void enable(bool in_enable)
	{
		gl.m_enable[INDEX] = in_enable ? 1 : 0;
	}
	
	inline ~gliEnable()
	{
		gl.m_enable[INDEX] = m_prevState;
	}
};

template<int GL_MODE, int INDEX>
class gliDisable : public gliEnable<GL_MODE, INDEX>
{
public:
	gliDisable(bool in_enable = false)
	: gliEnable<GL_MODE, INDEX>::gliEnable(in_enable)
	{}
};

#endif
