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

#ifndef COORD4D_H
#define COORD4D_H

/*! \file Coord4D.h
	\brief	Coordinates in 4-space
	Provides utilities to deal with coordinates in 4-space.
*/

//! Abstract Coord4D
/*! \tparam T[in]	An object that acts like a number */
template<class T>
class TCoord4D
{
public:
	//! X component
	T	x;
	
	//! Y component
	T	y;
	
	//! Z component
	T	z;
	
	//! W component
	T	w;
	
	//! Creates a new Coord4D
	/*!	\param in_x[in]		X-coordinate (default of 0)
		\param in_y[in]		Y-coordinate (default of 0)
		\param in_z[in]		Z-coordinate (default of 0)
		\param in_w[in]		W-coordinate (default of 0) */
	TCoord4D(T in_x = 0, T in_y = 0, T in_z = 0, T in_w = 0)
	: x (in_x)
	, y (in_y)
	, z (in_z)
	, w (in_w)
	{	}
};


//! Scale a TCoord byte the same type...
/*!	\tparam		T			Specialization of TCoord
	\param		in_t[in]	TCoord to scale
	\param		in_s[in]	Scaling factor to apply
	\return		scaled TParam object.	*/
template<class T>
TCoord4D<T> operator*(TCoord4D<T> in_t, T in_s)
{
	return TCoord4D<T>(in_t.x*in_s, in_t.y*in_s, in_t.z*in_s, in_t.w*in_s);
}


//! Component-wise multiplication of a TCoord
/*!	\tparam		T			Specialization of TCoord
	\param		in_t[in]	First TCoord
	\param		in_s[in]	Second TCoord
	\return		TCoord with component-wise multiplication.	*/
template<class T>
TCoord4D<T> operator*(TCoord4D<T> in_t, TCoord4D<T> in_s)
{
	return TCoord4D<T>(	in_t.x*in_s.x, in_t.y*in_s.y,
						in_t.z*in_s.z, in_t.w*in_s.w);
}



//! Coord4D as floating-point
typedef TCoord4D<float>	Coord4D;

#endif
