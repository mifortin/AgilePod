/*
 *	Copyright 2011 Michael Fortin
 *
 *	 Licensed under the Apache License, Version 2.0 (the "License");
 *	 you may not use this file except in compliance with the License.
 *	 You may obtain a copy of the License at
 *	 
 *		http://www.apache.org/licenses/LICENSE-2.0
 *	 
 *	 Unless required by applicable law or agreed to in writing, software
 *	 distributed under the License is distributed on an "AS IS" BASIS,
 *	 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	 See the License for the specific language governing permissions and
 *	 limitations under the License.
 */

#ifndef BubblePod_GPUShader_h
#define BubblePod_GPUShader_h

#include <OpenGLES/ES2/gl.h>

#include "Coord2D.h"
#include "Coord3D.h"
#include "Coord4D.h"
#include "Matrix2D.h"
#include "Matrix3D.h"
#include "GPUBuffer.h"

/*!	\file	GPUShader.h
	\brief	The ability to specify a shader for a given VBO
 
	A shader is disjoint from the VBO.  It as well provides a description.
	
	It is the draw operation (that finally displays the VBO on-screen) that
	ensures that the proper arrays are bound.	*/

namespace GPU
{
	//! Represents a parameter
	/*!	In GL, these are uniforms.  They are like constant parameters to a
		shader	*/
	class Parameter
	{
	private:
		//! Offset of the parameter (according to GL)
		GLint offset;
		
		//! Parameter is private (force going through shader)
		Parameter(GLint in_offset) : offset(in_offset) {}
		
		//! Needed for access to the constructor
		friend class Shader;
		
	public:
		//! Send in a scalar
		void set(const float in_val) const
		{	glUniform1f(offset, in_val);	}
		
		//! Send in a vector-2
		void set(const Coord2D &in_val) const
		{	glUniform2f(offset, in_val.x, in_val.y);	}
		
		//! Send in a vector-3
		void set(const Coord3D &in_val) const
		{	glUniform3f(offset, in_val.x, in_val.y, in_val.z);	}
		
		//! Send in a vector-4
		void set(const Coord4D &in_val) const
		{	glUniform4f(offset, in_val.x, in_val.y, in_val.z, in_val.w);	}
		
		//! Send in a matrix2x2
		void set(const Matrix2D &in_val) const
		{	glUniformMatrix2fv(offset, 1, GL_FALSE, (float*)in_val.rows);	}
		
		//! Send in a matrix3x3
		void set(const Matrix3D &in_val) const
		{	glUniformMatrix3fv(offset, 1, GL_FALSE, (float*)in_val.rows);	}
	};
	
	
	//! Represents an attribute
	/*!	This object matches data from the VBOs to the data within the shaders. */
	class Attribute
	{
	private:
		GLint offset;		//!< Offset of the parameter (according to GL)
		
		//! Private constructor to force going through shader
		Attribute(GLint in_offset) : offset(in_offset) {}
		
		//! Required for access to the constructor
		friend class Shader;
		
	public:
		//! Associate an attribute to an element within a VBO
		template<class T>
		void set(const Type::TVBO<T> &in_vbo, int in_offset)
		{
			const Type::TypeDescription *d = in_vbo.elementDescription(in_offset);
			glVertexAttribPointer(offset, d->size(), d->type(), GL_FALSE, in_vbo.count(), (char*)in_vbo.m_data() + d->offset());
		}
	};
	
	
	//! Represents a shader
	class Shader
	{
	private:
		//! The program to be bound
		GLuint m_program;
		
		//! Bind shader needs to access the shader object...
		friend class BindShader;
		
	public:
		//! Create a shader from external files
		/*!	It was possible to make this more complex, but it's not needed
			\param in_szFile	The name of the file where the shader code is
								stored.  Two files are assumed, one with a
								.vsh extension and the other with a .fsh.
								They are linked together to form the program */
		Shader(const char *in_szFile);
		
		//! Get the location of a given uniform
		Parameter getParameter(const char *in_name) const;
		
		//! Get the location of a given attribute
		Attribute getAttribute(const char *in_name) const;
		
		//! Done with the shader
		~Shader();
	};
	
	
	//! Makes a given shader active
	class BindShader
	{
		GLuint m_prevProgram;	//!< Used to restore the program
		
	public:
		//! Bind the shader as the current shader
		BindShader(Shader *in_program);
		
		//! Changes the bound shader (fewer GL ops)
		void rebind(Shader *in_program);
		
		//! Rebind the previously bound shader
		~BindShader();
	};
};

#endif
