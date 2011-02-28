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

/*! \defgroup Coord Series of objects and methods operating on vectors.
*/

/*! \addtogroup Coord
	Series of objects and methods operating on vectors.
	@{
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
	
	//! Scale a TCoord byte the same type...
	/*!	\param		in_s[in]	Scaling factor to apply
		\return		scaled TParam object.
		*/
	TCoord4D<T> operator*(const T in_s)		const
	{
		return TCoord4D<T>(x*in_s, y*in_s, z*in_s, w*in_s);
	}
	
	
	//! Scale this TCoord byte the same type...
	/*!	\param		in_s[in]	Scaling factor to apply
		\return		Reference to this TCoord.
		*/
	TCoord4D<T> &operator*=(const T in_s)
	{
		x *= in_s;
		y *= in_s;
		z *= in_s;
		w *= in_s;
		return *this;
	}


	//! Component-wise multiplication of a TCoord
	/*! \param		in_s[in]	Second TCoord
		\return		TCoord with component-wise multiplication.	*/
	TCoord4D<T> operator*(const TCoord4D<T> in_s)	const
	{
		return TCoord4D<T>(	x*in_s.x, y*in_s.y,
							z*in_s.z, w*in_s.w);
	}


	//! Component-wise multiplication to this TCoord
	/*! \param		in_s[in]	Second TCoord
		\return		reference to this TCoord.	*/
	TCoord4D<T> &operator*=(const TCoord4D<T> in_s)
	{
		x *= in_s.x;
		y *= in_s.y;
		z *= in_s.z;
		w *= in_s.w;
		return *this;
	}
	
	
	//! Component-wise addition of a TCoord
	/*!	\param		in_s[in]	TCoord to add
		\return		Compoent-wise sums of the TCoords */
	TCoord4D<T> operator+(const TCoord4D<T> in_s)		const
	{
		return TCoord4D<T>(	x+in_s.x, y+in_s.y,
							z+in_s.z, w+in_s.w);
	}
	
	
	//! Component-wise addition to this TCoord
	/*!	\param		in_s[in]	TCoord to add
		\return		Reference to this TCoord4D */
	TCoord4D<T> operator+=(const TCoord4D<T> in_s)
	{
		x += in_s.x;
		y += in_s.y;
		z += in_s.z;
		w += in_s.w;
		return *this;
	}
};


//! Simple matrix object to transform Coord4D
/*!	Operations happen on homogeneous coordinates
	\tparam	T[in]	Numeric type for calculations
*/
template<class T>
class TMatrix4D
{
public:
	//! First row of the matrix (values go down)
	TCoord4D<T> r1;
	
	//!	Second row of the matrix (located to right of r1)
	TCoord4D<T> r2;
	
	//! Third row of the matrix
	TCoord4D<T> r3;
	
	//! Fourth row of the matrix
	TCoord4D<T> r4;
	
	//! Homogeneous component... (located under r1)
	/*! 5th logical element in the rows of the matrix */
	TCoord4D<T> h;
	
	
	//!	 Initialize as identity...
	TMatrix4D(	const TCoord4D<T>	in_1	=	TCoord4D<T>(1,0,0,0),
				const TCoord4D<T>	in_2	=	TCoord4D<T>(0,1,0,0),
				const TCoord4D<T>	in_3	=	TCoord4D<T>(0,0,1,0),
				const TCoord4D<T>	in_4	=	TCoord4D<T>(0,0,0,1),
				const TCoord4D<T>	in_h	=	TCoord4D<T>(0,0,0,0))
	: r1(in_1)
	, r2(in_2)
	, r3(in_3)
	, r4(in_4)
	, h(in_h)
	{}
	
	
	//! Transform a vector by this matrix
	/*!	\param in_m[in]		Vector to transform with this matrix
		\return				The transformed vector */
	TCoord4D<T> operator*(const TCoord4D<T> in_m) const
	{
		return r1*in_m + r2*in_m + r3*in_m + r4*in_m + h*in_m;
	}
		
	
	//! Apply a scale transform to this matrix.
	/*! \param in_x[in]		X scale (default of 1)
		\param in_y[in]		Y scale (default of 1)
		\param in_z[in]		Z scale (default of 1)
		\param in_w[in]		W scale (default of 1)
		\return		Reference to this TMatrix4D */
	TMatrix4D<T> &scale(	const T in_x = 1, const T in_y = 1,
							const T in_z = 1, const T in_w = 1)
	{
		r1 *= in_x;
		r2 *= in_y;
		r3 *= in_z;
		r4 *= in_w;
		
		h *= TCoord4D<T>(in_x, in_y, in_z, in_w);
		
		return *this;
	}
	
	
	//! Apply a translation transformation to this matrix.
	/*! \param in_x[in]		X offset (default of 0)
		\param in_y[in]		Y offset (default of 0)
		\param in_z[in]		Z offset (default of 0)
		\param in_w[in]		W offset (default of 0)
		\return		Reference to this TMatrix4D */
	TMatrix4D<T> &translate(	const T in_x = 0, const T in_y = 0,
								const T in_z = 0, const T in_w = 0)
	{
		h += TCoord4D<T>(in_x, in_y, in_z, in_w);
		
		return *this;
	}
};


//! Coord4D as floating-point
typedef TCoord4D<float>	Coord4D;

//! Matrix4D as floating-point
typedef TMatrix4D<float> Matrix4D;

/*! @} */
#endif
