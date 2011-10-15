/*
 *	Copyright 2011 Michael Fortin
 *
 *	 Licensed under the Apache License, Version 2.0 (the "License");
 *	 you may not use this file except in compliance with the License.
 *	 You may obtain a copy of the License at
 *	 
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *	 
 *	 Unless required by applicable law or agreed to in writing, software
 *	 distributed under the License is distributed on an "AS IS" BASIS,
 *	 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	 See the License for the specific language governing permissions and
 *	 limitations under the License.
 */

#include "GPUBuffer.h"

namespace GPU
{
	//! Global VBO reference (for binding)
	static RCOne<VBO> g_vbo = NULL;
	
	
	VBO::VBO(int in_maxSize, bool in_isVertices, bool in_isStatic)
	: m_isVertices(in_isVertices)
	, m_isStatic(in_isStatic)
	, m_maxSize(in_maxSize)
	{
		glGenBuffers(1, &m_buff);
		
		BindVBO b(this);
		glBufferData(	m_isVertices?GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER,
						m_maxSize, NULL,
						m_isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}
	
	
	VBO::~VBO()
	{
		glDeleteBuffers(1, &m_buff);
	}
	
	
	void VBO::bind()
	{
		if (g_vbo() != this)
			glBindBuffer(m_isVertices?GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER, m_buff);
	}
	
	
	void VBO::uploadData(const void *in_data, int length, int start)
	{
		BindVBO b(this);
		
		if (length <= 0)	length = m_maxSize - start;
		
		glBufferSubData(m_isVertices?GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER,
						start, length, NULL);
	}
	
	
	
	BindVBO::BindVBO(VBO *in_vbo)
	: m_vbo(g_vbo())
	{
		rebind(in_vbo);
	}
	
	BindVBO::~BindVBO()
	{
		m_vbo()->bind();
		g_vbo = m_vbo();
	}
	
	void BindVBO::rebind(VBO *in_other)
	{
		in_other->bind();
		g_vbo = in_other;
	}
	
	namespace Type
	{
		namespace Description
		{
			const TypeDescription V3::desc[]
			=	{	TypeDescription(Float32, Vertices, 3)	};
			
			const int V3::length()
			{	return sizeof(desc) / sizeof(TypeDescription);	}
			
			
			
			const TypeDescription V3_C4::desc[]
			=	{	TypeDescription(Float32,	Vertices,	3),
					TypeDescription(Int8,		Colours,	4)		};
			
			const int V3_C4::length()
			{	return sizeof(desc) / sizeof(TypeDescription);	}
		}
	};
};
