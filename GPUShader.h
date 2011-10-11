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

#ifndef BubblePod_GPUShader_h
#define BubblePod_GPUShader_h

/*!	\file	GPUShader.h
	\brief	The ability to specify a shader for a given VBO
 
	A shader is disjoint from the VBO.  It as well provides a description.
	
	It is the draw operation (that finally displays the VBO on-screen) that
	ensures that the proper arrays are bound.	*/

namespace GPU
{
	
	class Shader
	{
	};
	
	
	class BindShader
	{
	};
};

#endif
