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

/*!	\file	GPUOperation.h
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
	
	
	//! Specialize a VBO for a specific type.
	/*!	VBOs typically are not random.  Rather, they represent an array of
		something.  In this case T.
	 
		T should be an object.  T must have the following methods:
		-	size	- The number of types within the VBO
		-	[]		- Overloaded operator returning a descriptor of the types:
					-	Is it float?  short?  4-vector?  3-vector?
					-	The shader will specify links from this to underlying
						arrays.  ES 1x shaders actually map to the fixed-point
						hardware with appropriate changes of client state.
					-	Matching from the shader to the object is done through
						hints.
	 
		The reasoning is that data should be divorced from the shader (share
		the same shader across the same data-set).  Shaders will only be able
		to bind to VBOs with compatible signatures...
	 
		\tparam	T	The type within the VBO		*/
	template<class T>
	class TVBO : public VBO
	{
	};
};

#endif
