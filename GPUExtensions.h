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

#ifndef BubblePod_GPUExtensions_h
#define BubblePod_GPUExtensions_h

/*!	\file	GPUExtensions.h
	\brief	A simple way to query which GPU extensions are supported.
	
	This is primarily used internally by the system to determine how to allocate
	data (for example, can we create an NPOT texture?  can we work with
	compressed texture data?  etc.)
 
	For testing, applications should check if they run when all extensions are
	disabled.
 */


namespace GPU
{
	
	namespace Support
	{
		//!	Return true if Non-power-of-2-textures are supported.
		/*!	This is a core feature in OpenGL ES 2x. It is optional in 1x.
			
			When CLAMP_TO_EDGE is specified for the clamping mode and
			NEAREST or LINEAR is specified for the minification filter,
			the device supports NPOT and returns true.
			*/
		bool NPOT();
	
		
		//! True if BGRA textures can be efficiently uploaded
		/*!	An optional feature in both OpenGL ES 1x and 2x.
		 
			The input format of a texture may be GL_BGRA_EXT
			with type GL_UNSIGNED_BYTE	*/
		bool BGRATexture();
		
		
		//! True if a buffer may be mapped into main memory
		/*!	Unlocks glGetPointervOES						*/
		bool MapBuffer();
	};
	
};


#endif
