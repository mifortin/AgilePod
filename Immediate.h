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
   
/*! \file Immediate.h
 *	\brief	A restricted implementation of GL immediate mode for iTouch.
 */

#ifndef IMMEDIATE_H
#define IMMEDIATE_H

#ifndef POSITION_MULT
#define POSITION_MULT	2
#endif

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#import "Camera.h"
#import "Coord4D.h"

#define ALIGN(x)	__attribute__((aligned(x/8)))

////////////////////////////////////////////////////////////////////////////////
//
//	OpenGL Colour Object
//

//!	Internal OpenGL colour object, based on TCoord4D
class gliColour : protected TCoord4D<GLubyte>
{
public:
	
	gliColour(float ir, float ig=0, float ib=0, float ia=0)
	: TCoord4D<GLubyte>::TCoord4D(255*ir, 255*ig, 255*ib, 255*ia)
	{	}
	
	gliColour(GLubyte ir=0, GLubyte ig=0, GLubyte ib=0, GLubyte ia=0)
	: TCoord4D<GLubyte>::TCoord4D(ir, ig, ib, ia)
	{	}
	
	gliColour(int ir, int ig = 0, int ib = 0, int ia = 0)
	: TCoord4D<GLubyte>::TCoord4D(ir, ig, ib, ia)
	{	}
	
	//!	Obtain red component
	GLubyte &r()	{	return x;	}
	
	//! Obtain green component
	GLubyte &g()	{	return y;	}
	
	//! Obtain blue component
	GLubyte &b()	{	return z;	}
	
	//! Obtain alpha component
	GLubyte &a()	{	return w;	}
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
class gliTexCoord : protected Coord2DI
{	
public:
	gliTexCoord(float iu, float iv=0)
	: Coord2DI(iu,iv)
	{}
	
	gliTexCoord(GLshort iu=0, GLshort iv=0)
	: Coord2DI(iu, iv)
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
class Texture;
class FrameBuffer;
class BindTexture;
class Draw;

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


//! Object managing simplified OpenGLES1 state
/*! \ingroup OpenGLES1
	Most methods are private and accessed via friend classes */
class gli
{
private:
	//The OpenGL submission.
	gliSubmit submission[4096];
	
	//Immediate texture coordinate and color
	gliTexCoord 		m_texCoord;
	
	gliColour 			m_colour;
	
	//Device info...
	int		m_width;
	int		m_height;
	float	m_scale;
	
	
	//Drawing mode...
	short m_mode;
	short m_curVertex;


////////////////////////////////////////////////////////////////////////////////
	//Enable/disable...
	friend class gliEnable<GL_TEXTURE_2D, 	 0	>;
	friend class gliEnable<GL_BLEND,		 1	>;
	friend class gliEnable<GL_COLOR_ARRAY,	 2	>;
	friend class gliEnable<GL_TEXTURE_COORD_ARRAY,	 3	>;
	
	char m_enable[4];					//Our logical state
	char m_pvtEnable[4];				//Actual state..
	
	
////////////////////////////////////////////////////////////////////////////////
	//Texturing...
	friend class FrameBuffer;
	friend class BindTexture;
	
	
	
////////////////////////////////////////////////////////////////////////////////
	//Drawing...
	friend class Draw;
	
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
		
		//Check for blending...
		if (m_enable[1] != m_pvtEnable[1])
		{
			m_pvtEnable[1] = m_enable[1];
			if (m_enable[1])
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);
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
	
	inline void texCoord(float u, float v=0)
	{	m_texCoord = gliTexCoord(u,v);		}
	
	//! Set up the current texture coordinate to use
	/*!	\param	in_t[in]	Texture coordinate to set */
	inline void texCoord(const gliTexCoord &in_t)
	{	m_texCoord = in_t;		}
	
	inline void texCoordi(GLshort u, GLshort v=0)
	{	m_texCoord = gliTexCoord(u,v);		}
	
	inline void colour(float r, float g=0, float b=0, float a=0)
	{	m_colour = gliColour(r,g,b,a);		}
	inline void colouri(GLubyte r=0, GLubyte g=0, GLubyte b=0, GLubyte a=0)
	{	m_colour = gliColour(r,g,b,a);		}
	inline void colour(const gliColour &in_color)
	{	m_colour = in_color;				}
	
	inline void vertex(float x, float y=0, float z=0)
	{
		submission[m_curVertex].position = gliPosition3D(x,y,z);
		submission[m_curVertex].colour = m_colour;
		submission[m_curVertex].texCoord = m_texCoord;
		m_curVertex++;
	}
	
	inline void vertexi(GLshort x=0, GLshort y=0, GLshort z=0)
	{
		submission[m_curVertex].position = gliPosition3D(x,y,z);
		submission[m_curVertex].colour = m_colour;
		submission[m_curVertex].texCoord = m_texCoord;
		m_curVertex++;
	}
	
public:
	gli()
	: m_width(0)
	, m_height(0)
	, m_scale(1)
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

	
	inline void pushMatrix()
	{
		glPushMatrix();
	}
	
	inline void popMatrix()
	{
		glPopMatrix();
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


//!	OpenGL Blending Mode
/*!	
		Usage: 	instantiate the object.  It will enable the given
				blending modes and disable them once it gets popped off
				the stack.
 
		This object works with both OpenGL ES 1 and 2.
 
		The intended use is on the stack.  So as it gets pushed/popped on
		the stack the GL state is updated accordingly.
*/
class gliBlendFunc
{
	GLenum m_oldSrc;		//!< Previous src blend func
	GLenum m_oldDst;		//!< Previous dest blend func

public:
	//! Create a new blendFunc object that will restore previous values on free.
	gliBlendFunc(GLenum in_src=GL_ONE, GLenum in_dst=GL_ZERO);
	
	//! Change the blend function
	void blendFunc(GLenum in_src=GL_ONE, GLenum in_dst=GL_ZERO);
	
	//! Restore the previous blend function
	inline ~gliBlendFunc()
	{
		blendFunc(m_oldSrc, m_oldDst);
	}
};


//!	OpenGL Enable Disable
/*!\ingroup OpenGLES1
		Usage:	enables and disables GL states.
				use the provided typedefs...
		Impl:	we use a template system - an index into an array in
				the global GL object, and the GL enable/disable key.
*/

template<int GL_MODE, int INDEX>
class gliEnable
{
	char m_prevState;
public:
	//! Enable the given state (pass false to disable)
	gliEnable(bool in_enable = true)
	{
		m_prevState = gl.m_enable[INDEX];
		gl.m_enable[INDEX] = in_enable ? 1 : 0;
	}
	
	//! Toggle enable/disable
	inline void enable(bool in_enable = true)
	{
		gl.m_enable[INDEX] = in_enable ? 1 : 0;
	}
	
	//! Disable (explicit)
	inline void disable()
	{
		enable(false);
	}
	
	//! Upon destruction, return previous state.
	inline ~gliEnable()
	{
		gl.m_enable[INDEX] = m_prevState;
	}
};

//! Equivalent to gliEnable except disables by default
template<int GL_MODE, int INDEX>
class gliDisable : public gliEnable<GL_MODE, INDEX>
{
public:
	gliDisable(bool in_enable = false)
	: gliEnable<GL_MODE, INDEX>::gliEnable(in_enable)
	{}
};


//!	OpenGL Drawing Routines
/*!
 \ingroup OpenGLES1
 Usage:	Instantiate within the context which drawing is desired...
 
 For example, to draw a triangle (no need for lists):
\code
{
	Draw d(GL_TRIANGLES)	//Rendering starts when d goes out of scope.
	
	d.texCoord(...);		//A la OpenGL immediate mode!
	d.vertex(...);
}
\endcode
*/

class Draw
{
public:
	//!Make sure that each begin has an associated end
	Draw(short in_mode)
	{
		gl.begin(in_mode);
	}
	
	~Draw()
	{
		gl.end();
	}
	
	//!Forward all the vertex and texture stuff...
	inline void texCoord(float u, float v=0)
	{	gl.texCoord(u,v);		}
	
	//! Apply a texture coordinate from texture
	inline void texCoord(const gliTexCoord &in_t)
	{	gl.texCoord(in_t);		}
	
	//! Texture coordinates from integers
	inline void texCoordi(GLshort u, GLshort v=0)
	{	gl.texCoordi(u,v);		}
	
	//! Texture coordinate from Coord2D
	/*!	\param in_t[in]		Texture coordinates within Coord2D	*/
	inline void texCoord(const Coord2D &in_t)
	{	gl.texCoord(in_t.x, in_t.y);	}
	
	//! Set up the colour using floats.
	/*!	 \param	r[in]	Red component
		 \param g[in]	Green component, default 0
		 \param	b[in]	Blue component, default 0
		 \param	a[in]	Alpha component, default 0	*/
	inline void colour(float r, float g=0, float b=0, float a=0)
	{	gl.colour(r,g,b,a);		}
	
	//! Set the colour using bytes explicitly (range 0...255)
	/*!	\param	r[in]	Red component, default 0
		\param 	g[in]	Green component, default 0
		\param	b[in]	Blue component, default 0
		\param	a[in]	Alpha component, default 0	*/
	inline void colouri(GLubyte r=0, GLubyte g=0, GLubyte b=0, GLubyte a=0)
	{	gl.colouri(r,g,b,a);		}
	
	//! Assign a colour using an internal gliColour object
	/*! \param in_colour[in]		Colour to set the GL state to */
	inline void colour(const gliColour &in_colour)
	{	gl.colour(in_colour);				}
	
	//! Assign a colour stored in a Coord4D
	/*! \param in_colour[in]		Colour to set the GL state to */
	inline void colour(const Coord4D in_colour)
	{	gl.colour(in_colour.x, in_colour.y, in_colour.z, in_colour.w);	}
	
	//! Set up a vertex
	/*!	\param x[in]	X coordinate
		\param y[in]	Y coordinate (default 0)
		\param z[in]	Z coordinate (default 0)	*/
	inline void vertex(float x, float y=0, float z=0)
	{
		gl.vertex(x,y,z);
	}
	
	inline void vertex(Coord2D in_c)
	{
		gl.vertex(in_c.x, in_c.y, 0);
	}
	
	inline void vertexi(GLshort x=0, GLshort y=0, GLshort z=0)
	{
		gl.vertexi(x,y,z);
	}
};


//!Wraps the transforms (automatically push / pop matrices)
/*! \ingroup OpenGLES1 */
class gliTransform
{
public:
	gliTransform()		{	gl.pushMatrix();	}
	~gliTransform()		{	gl.popMatrix();		}
	
	void translate(const Coord2D &in_c)
	{
		gl.translate(in_c.x, in_c.y);
	}
	
	void scale(const Coord2D &in_s)
	{
		gl.scale(in_s);
	}
};

#endif
