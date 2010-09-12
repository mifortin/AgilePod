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

#ifndef SIMD_H
#define SIMD_H

//Not really related to AgilePod, but something I'll find infinitely useful
//	- an object that provides easy access to SIMD-style operations
//
//	My current machine has 128bit vector ops; future machines will have
//	256bit vector ops.
//
//	For expansion; I'll allow the number of objects and the type to be
//	specified.  The compiler should be smart enough to figure out the intent.
//	(eg. the autovectorizer).
//
//	I'm supposing that the compiler will be generally smart enough to
//	generate the code that I want...
//
//		However; the door is left open for manual tuning (if ever it becomes
//		desirable).

//Default to 16-byte.  Up this for 32-byte aligned SIMD machines.
#ifndef SIMD_SIZE
#define SIMD_SIZE	16
#endif

typedef float simd_float __attribute__ ((vector_size (SIMD_SIZE)));

union xsimd_float
{
	simd_float x;
	float f[SIMD_SIZE/sizeof(float)];
};

//Default to float with 4.  But allow easy expansion to 8-element-wide SIMD.
class SIMDf32
{	
public:
	//DO NOT USE (for operators and nicer functions)
	xsimd_float x;
	
	//Public functions below...
	SIMDf32()	{}
	
	SIMDf32(simd_float in_x)
	{
		x.x = in_x;
	}
	
	float &operator[](int i)
	{
		return x.f[i];
	}
} __attribute__ ((aligned(SIMD_SIZE)));


static SIMDf32 operator+(const SIMDf32 &a, const SIMDf32 &b)
{
	return SIMDf32(a.x.x + b.x.x);
}


static SIMDf32 operator-(const SIMDf32 &a, const SIMDf32 &b)
{
	return SIMDf32(a.x.x - b.x.x);
}


static SIMDf32 operator*(const SIMDf32 &a, const SIMDf32 &b)
{
	return SIMDf32(a.x.x * b.x.x);
}

static SIMDf32 operator/(const SIMDf32 &a, const SIMDf32 &b)
{
	return SIMDf32(a.x.x / b.x.x);
}


#endif
