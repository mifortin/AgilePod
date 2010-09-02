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

//Different types of GL data
template <GLuint GLTYPE, class GLDATA, int MULT>
class gliType
{
private:
	GLDATA m_data;

public:
	gliType()
	: m_data(0)
	{}

	inline GLuint type() const			{	return GLTYPE;	}
	inline float multiplier() const		{	return MULT;	}
	
	inline GLDATA operator()() const	{	return m_data;	}
	
	inline gliType& operator=(float i)
	{	m_data = (GLDATA)(i*(float)MULT);	return *this; }
	
	inline gliType& operator=(int i)
	{	m_data = (GLDATA)i;			return *this; }
	
	inline gliType& operator=(short s)
	{	m_data = (GLDATA)s;			return *this; }
	
	inline gliType& operator=(unsigned char uc)
	{	m_data = (GLDATA)uc;			return *this; }
	
	inline float floatValue() const	{	return (float)m_data / (float)MULT; }
	inline float intValue()	  const	{	return (int)m_data;	}
};

typedef gliType<GL_UNSIGNED_BYTE, 	GLubyte, 255>			gliTypeUnsignedByte;
typedef gliType<GL_FLOAT,			GLfloat, 1>				gliTypeFloat;
typedef gliType<GL_SHORT,			GLshort, 2>				gliTypeShort;
typedef gliType<GL_SHORT,			GLshort, 1024>			gliTypeShort1024;


//Actual data types
template<class GL_TYPE, int COUNT>
class gliDataList
{
protected:
	GL_TYPE m[COUNT];

public:
	inline GLuint type() const		{ return m[0].type();		}
	inline float multiplier() const	{ return m[0].multiplier();	}
	inline int size() const			{ return COUNT;				}
};


template<class GL_TYPE>
class gliColour : public gliDataList<GL_TYPE,4>
{
public:
	gliColour(float ir, float ig=0, float ib=0, float ia=0)
	{
		gliDataList<GL_TYPE,4>::m[0] = ir;
		gliDataList<GL_TYPE,4>::m[1] = ib;
		gliDataList<GL_TYPE,4>::m[2] = ib;
		gliDataList<GL_TYPE,4>::m[3] = ia;
	}
	
	gliColour(int ir=0, int ig=0, int ib=0, int ia=0)
	{
		gliDataList<GL_TYPE,4>::m[0] = ir;
		gliDataList<GL_TYPE,4>::m[1] = ib;
		gliDataList<GL_TYPE,4>::m[2] = ib;
		gliDataList<GL_TYPE,4>::m[3] = ia;
	}
	
	GL_TYPE &r()	{	return gliDataList<GL_TYPE,4>::m[0];	}
	GL_TYPE &g()	{	return gliDataList<GL_TYPE,4>::m[1];	}
	GL_TYPE &b()	{	return gliDataList<GL_TYPE,4>::m[2];	}
	GL_TYPE &a()	{	return gliDataList<GL_TYPE,4>::m[3];	}
} ALIGN(32);

typedef gliColour<gliTypeUnsignedByte>	gliColourUnsignedByte;
typedef gliColour<gliTypeFloat>			gliColourFloat;
typedef gliColour<gliTypeShort>			gliColourShort;


template<class GL_TYPE>
class gliPosition2D : public gliDataList<GL_TYPE, 2>
{	
public:
	gliPosition2D(float ix, float iy=0, float iz=0)
	{
		gliDataList<GL_TYPE,2>::m[0] = ix;
		gliDataList<GL_TYPE,2>::m[1] = iy;
	}
	
	gliPosition2D(int ix=0, int iy=0, int iz=0)
	{
		gliDataList<GL_TYPE,2>::m[0] = ix;
		gliDataList<GL_TYPE,2>::m[1] = iy;
	}
} ALIGN(32);
typedef gliPosition2D<gliTypeFloat>		gliPosition2DFloat;
typedef gliPosition2D<gliTypeShort>		gliPosition2DShort;



template<class GL_TYPE>
class gliPosition3D : public gliDataList<GL_TYPE, 3>
{	
public:
	gliPosition3D(float ix, float iy=0, float iz=0)
	{
		gliDataList<GL_TYPE,3>::m[0] = ix;
		gliDataList<GL_TYPE,3>::m[1] = iy;
		gliDataList<GL_TYPE,3>::m[2] = iz;
	}
	
	gliPosition3D(int ix=0, int iy=0, int iz=0)
	{
		gliDataList<GL_TYPE,3>::m[0] = ix;
		gliDataList<GL_TYPE,3>::m[1] = iy;
		gliDataList<GL_TYPE,3>::m[2] = iy;
	}
} ALIGN(32);
typedef gliPosition3D<gliTypeFloat>		gliPosition3DFloat;
typedef gliPosition3D<gliTypeShort>		gliPosition3DShort;



template<class GL_TYPE>
class gliTexCoord : public gliDataList<GL_TYPE, 2>
{	
public:
	gliTexCoord(float iu, float iv=0)
	{
		gliDataList<GL_TYPE,2>::m[0] = iu;
		gliDataList<GL_TYPE,2>::m[1] = iv;
	}
	
	gliTexCoord(int iu=0, int iv=0)
	{
		gliDataList<GL_TYPE,2>::m[0] = iu;
		gliDataList<GL_TYPE,2>::m[1] = iv;
	}
} ALIGN(32);
typedef gliTexCoord<gliTypeFloat>		gliTexCoordFloat;
typedef gliTexCoord<gliTypeShort1024>		gliTexCoordShort;


//Aggregate object that holds all the needed data for submission to the GL
template<class GL_COORD, class GL_TEX, class GL_COL>
class gliSubmit
{
public:
	GL_COORD	position;
	GL_COL		colour;
	GL_TEX		texCoord;
}ALIGN(32);


//Each of these objects are specific to the GLI object.  That is they
//define the internal structure!
template<int maxEle, class GL_COORD, class GL_TEX, class GL_COL>
class gli
{
private:
	//The OpenGL submission.
	gliSubmit <GL_COORD, GL_TEX, GL_COL> submission[maxEle];
	
	//Immediate texture coordinate and color
	GL_TEX m_texCoord;
	GL_COL m_colour;
	
	//Drawing mode...
	short m_mode;
	short m_curVertex;
	
	//Texture info...
	GLuint m_textureID;
	GLuint m_boundTexture;
	
public:
	gli()
	: m_textureID(0)
	, m_boundTexture(0)
	{}

	//add texture coordinates...
	inline void texCoord(float u, float v=0)	{	m_texCoord = GL_TEX(u,v);		}
	inline void texCoordi(int u, int v=0)		{	m_texCoord = GL_TEX(u,v);		}
	
	inline void colour(float r, float g=0, float b=0, float a=0)
	{	m_colour = GL_COL(r,g,b,a);		}
	inline void colouri(int r=0, int g=0, int b=0, int a=0)
	{	m_colour = GL_COL(r,g,b,a);		}
	inline void colour(const GL_COL &in_color)
	{	m_colour = in_color;			}
	
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
		submission[m_curVertex].position = GL_COORD(x,y,z);
		submission[m_curVertex].colour = m_colour;
		submission[m_curVertex].texCoord = m_texCoord;
		m_curVertex++;
	}
	
	inline void vertexi(int x=0, int y=0, int z=0)
	{
		submission[m_curVertex].position = GL_COORD(x,y,z);
		submission[m_curVertex].colour = m_colour;
		submission[m_curVertex].texCoord = m_texCoord;
		m_curVertex++;
	}
	
	inline void translate(float dx, float dy=0, float dz = 0)
	{
		glTranslatef(dx*POSITION_MULT, dy*POSITION_MULT, dz*POSITION_MULT);
	}
	
	inline void translate(Coord2D &in_t)
	{
		translate(in_t.x, in_t.y);
	}

	//Start and end a shape...
	inline void begin(short mode)
	{
		m_mode = mode;
		m_curVertex = 0;
	}

	inline void end()
	{
		if (m_boundTexture !=  m_textureID)
		{
			m_boundTexture = m_textureID;
			glBindTexture(GL_TEXTURE_2D, m_textureID);
		}
	
		glColorPointer(		submission[0].colour.size(),
							submission[0].colour.type(),
							sizeof(submission[0]),
							&(submission[0].colour));
		glTexCoordPointer(	submission[0].texCoord.size(),
							submission[0].texCoord.type(),
							sizeof(submission[0]),
							&(submission[0].texCoord));
		glVertexPointer(	submission[0].position.size(),
							submission[0].position.type(),
							sizeof(submission[0]),
							&(submission[0].position));

		glDrawArrays(m_mode, 0, m_curVertex);
	}
	
}ALIGN(32);
typedef gli<1024, gliPosition2DShort, gliTexCoordShort, gliColourUnsignedByte> gl2D;

extern gl2D gl;
extern gliColourUnsignedByte gliColourWhite;


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

#endif
