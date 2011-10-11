/*
 Copyright 2011 Michael Fortin
 
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

#ifndef BubblePod_GPUOperation_h
#define BubblePod_GPUOperation_h

#include "Coord3D.h"
#include "Coord4D.h"

/*!	\file	GPUBuffer.h
	\brief	Provides an additional layer of abstraction for OpenGL ES 2x.
 
	OpenGL ES 2x replaced the fixed-function pipeline of OpenGL ES 1x with
	fragment and vertex shaders.  Under the hood (see http://developer.apple.com/library/ios/#DOCUMENTATION/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/OpenGLESPlatforms/OpenGLESPlatforms.html#//apple_ref/doc/uid/TP40008793-CH106-SW1 ), 
	OpenGL ES 2x can be seen as providing specialized shaders for OpenGL ES 1x
	depending upon the currently enabled client states.
 
	In this way, an operation on the GPU can be seen as either a valid combination
	of client states on the fixed-function pipeline or the running of shaders
	that provide more control over what happens.
 
	To support this generically, various specialized GPU operations describe
	vertex data with varying levels of detail (colour? tex coordinates? etc).
	This support is in a base class providing a means to specify data
	simply and efficiently.
 
	Furthermore, within OpenGL ES 2, a user of the library can use shaders and
	provide custom GPUOperation objects that specify data as is needed by the
	shaders.
 
	For compatibility - shaders that mimic the OpenGL ES 1 are provided.  A
	mechanism to query whether an operation is supported is provided (certain
	operations are always supported).
 
	=================
 
	Currently, the system assumes that an element array buffer is always
	provided.
 
	This is since there is no fall-back when a 0 should be set as a given VBO.
	
	It's also a good thing, to a given extent.... (if in_maxSize = 0 then it
	should clear the VBO binding?  Could work nicely if cleared bindings are
	setup on the stack before code execution begins...)
 */

#include "Smart.h"
#include <OpenGLES/ES2/gl.h>

namespace GPU
{	
	//! Basic operations that a VBO (minus template) should do - binding
	/*!	This just provides the machinery to create, upload, and bind buffers.
		The TVBO object provides machinery to specialize a VBO for specific
		data sets.  Specific VBOs are then bound to shader objects to 
		determine rendering.  Shaders should expose enough parameters to
		reduce rendering and changing of VBOs.
	 
		As far as I know, this is the most elegant way forward.
	 */
	class VBO : public RC
	{
	private:
		//! The buffer in the back
		GLuint m_buff;
		
		//! Is the data static or are we continually re-uploading?
		/*!	If continually re-uploading, the GL manual recommends swapping
			buffers */
		bool m_isStatic;
		
		//! Is this vertices or shorts / elements?
		bool m_isVertices;
		
		//! The size that the buffer was allocated for.
		/*!	GLBufferSubData is faster - but it needs a second buffer or else
			the CPU must wait on the GL to complete before upload can continue.	*/
		int m_maxSize;
		
	protected:
		friend class BindVBO;
		
		//! Do the binding (used by BindVBO)
		void bind();
		
		//! Actually upload the data
		/*!	\param	in_data		The data to be uploaded to the GPU
			\param	in_length	The amount of data (<=0 for full length)
			\param	in_start	Where to start the upload (usually 0 for static)
		 */
		void uploadData(const void *in_data, int in_length=-1, int in_start=0);
		
	public:
		//! Create a new VBO (default is for vertices with static data)
		VBO(int in_maxSize, bool in_isVertices = true, bool in_isStatic = true);
		
		//! Clear out the buffer
		virtual ~VBO();
	};
	
	
	//! Provides a means to bind and unbind vertex buffer objects
	/*!	Note, like the other Bind objects, this one provides increased code
		legibility only if it is used on the stack.  On the heap, it may
		overly complicate the code.	*/
	class BindVBO
	{
	private:
		//! Previously bound VBO object
		RCOne<VBO>	m_vbo;
		
	public:
		//! Initialize with a VBO object (can't be NULL)
		/*!	Internally, the currently bound vbo is stored into m_vbo	*/
		BindVBO(VBO *in_vbo);
		
		//! Done with the VBO object
		/*!	Internally, m_vbo is restored into g_vbo.					*/
		virtual ~BindVBO();
		
		//! Bind another VBO (can't be NULL)
		void rebind(VBO *in_other);
	};
	
	
	namespace Type
	{
		//! Enumerate the different types that exist on the GPU
		/*!	The values equate to OpenGL values for simplicity	*/
		enum Type
		{
			Float32		= GL_FLOAT,			//!< Standard 32-bit float
			Int8		= GL_UNSIGNED_BYTE,	//!< Unsigned char
			Int16		= GL_SHORT,			//!< Short
			Int32		= GL_INT			//!< 32-bit int.
		};
		
		//! Purpose for the different types (vertices, colours, etc.)
		enum Purpose
		{
			Vertices,						//!< A list of vertices
			Colours,						//!< A list of colours
			Indices,						//!< A list of indices
			TextureCoordinates				//!< A list of tex coordinates
		};
	
	
		//!	Describes a type within an FBO (such as a set of floats, etc.)
		/*!	Later, all the types are assumed to be packed.  So a float
			will take 4 bytes, and if a short comes after it will be
			offset by 4 bytes after the float.	*/
		class TypeDescription
		{
		private:
			//! Data type for the data (upload)
			const Type m_type;
			
			//! The purpose of the type.
			/*! If more than one of the same type, then the first occurence is 0 */
			const Purpose m_purpose;
			
			//! The size of the vector.
			/*!	Differentiate between vertices on the line, plane, space, and 4d */
			const int m_size;
			
		public:
			//! Create a new TypeDescription
			TypeDescription(Type in_type, Purpose in_purpose, int in_s)
			: m_type(in_type)
			, m_purpose(in_purpose)
			, m_size(in_s)
			{
				if (in_s <= 0 || in_s > 4)	throw "TypeDescription::Invalid size";
			}
			
			//! Get the type
			const Type type() const			{	return m_type;				}
			
			//! Get the purpose
			const Purpose purpose() const	{	return m_purpose;			}
			
			//! Get the size
			const int size() const			{	return m_size;				}
		};
		
		
		namespace Description
		{
			//! Describes a single vertex
			class V3 : public Coord3D
			{
				static const TypeDescription desc[];
			
			public:
				static const int length();
				static const TypeDescription* description()	{ return desc;	}
			};
			
			//! Describes a vertex + colour
			class V3_C4
			{
				static const TypeDescription desc[];
				
			public:
				Coord3D		position;
				ByteCoord4D	colour;		//!< x=r, y=g, z=b, w=a
				
				static const int length();
				static const TypeDescription* description() { return desc;	}
			};
		};
		
		
		//! Specialize a VBO for a specific type.
		/*!	VBOs typically are not random.  Rather, they represent an array of
		 something.  In this case T.
		 
		 T should be an object.  T must have the following methods:
		 -	description:	returns a vector of TypeDescription objects
		 
		 The reasoning is that data should be divorced from the shader (share
		 the same shader across the same data-set).  Shaders will only be able
		 to bind to VBOs with compatible signatures...
		 
		 \tparam	T	The type within the VBO		*/
		template<class T>
		class TVBO : public VBO
		{
		private:
			//! The data uploaded to the VBO.
			Many<T> m_data;
			
			//! The amount of data
			int m_count;
			
		public:
			//! Create a new VBO with in_count elements
			TVBO(int in_count, bool in_isStatic = true)
			: VBO(in_count *sizeof(T), false, in_isStatic)
			, m_count(in_count)
			{
				m_data = new T[in_count];
			}
			
			//! Count (number of elements) in VBO
			int count() const				{	return m_count;				}
			
			//! Access the data
			T &operator[](int in_index)		{	return m_data()[in_index];	}
			
			//! Synchronize the data with the source
			void sync()
			{
				uploadData((void*)m_data());
			}
			
			//! The number of elements in the VBO data structure
			static const int elementCount()	{	return T::length();		}
			
			//! A list of the elements in the VBO data structure
			static const TypeDescription* elementDescription()
			{	return T::description();	}
			
		};
	}
	
	
	//! Specialized VBO with just a single set of vertices
	typedef Type::TVBO<Type::Description::V3>			VBO_V3;
	typedef Type::TVBO<Type::Description::V3_C4>		VBO_V3_C4;
};

#endif
