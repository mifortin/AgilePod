/*
   Copyright 2010,2011 Michael Fortin

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

#ifndef COORD3D_H
#define COORD3D_H

#include "Coord2D.h"
#include <cmath>

//! Redefinition of CMAcceleration
/*!	This allows for everything to remain in C++, removes need to link
	against CoreMotion if unneeded */
#ifndef __COREMOTION__
typedef struct {
	double x;
	double y;
	double z;
} CMAcceleration;
#endif

//! 3D coordinate object
/*!	Abstract coordinate object.  Allows for changing underlying
	number representation type easily.
	\tparam	T	Number type (Coord3D is float)
 */
template<class T>
class TCoord3D
{
public:
	//! Coordinates (Euclidean, typically)
	T x,y,z;
	
	//! Standard constructor (everything default to 0)
	/*!	\param	[in] 	in_x		X coordinate (default 0)
		\param 	[in]	in_y		Y coordinate (default 0)
		\param 	[in]	in_z		Z coordinate (default 0) */
	TCoord3D(T in_x = 0, T in_y = 0, T in_z = 0)
	: x(in_x), y(in_y), z(in_z) {}
	
	//! Construct from a CoreMotion acceleration object
	/*!	\param	[in]	in_accel	The core motion acceleration object */
	TCoord3D(CMAcceleration in_accel)
	: x(in_accel.x), y(in_accel.y), z(in_accel.z)
	{}
	
	//! Standard addition operator
	TCoord3D &operator+=(const TCoord3D &other)
	{
		x+=other.x;
		y+=other.y;
		z+=other.z;
		return *this;
	}
	
	//! Standard subtraction operator
	TCoord3D &operator-=(const TCoord3D &other)
	{
		x-=other.x;
		y-=other.y;
		z-=other.z;
		return *this;
	}
	
	//! Self-division operator
	TCoord3D &operator/=(const T other)
	{
		x/=other;
		y/=other;
		z/=other;
		return *this;
	}
	
	
	//! Self-multiplication operator
	TCoord3D &operator*=(const T other)
	{
		x*=other;
		y*=other;
		z*=other;
		return *this;
	}
	
	//! Negation operator
	TCoord3D operator-()
	{
		return TCoord3D(-x, -y, -z);
		return *this;
	}
};

//! Specialization of TCoord3D for float
typedef TCoord3D<float> Coord3D;


//! Compute the dot product
template<class T>
static T dot(const TCoord3D<T> &a, const TCoord3D<T> &b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}


//! Compute the magnitude
template<class T>
static T magnitude(const TCoord3D<T> &a)
{
	return std::sqrt(dot(a,a));
}

template<class T>
static TCoord3D<T> operator-(const TCoord3D<T> &a, const TCoord3D<T> &b)
{
	return TCoord3D<T>(a.x - b.x, a.y - b.y, a.z - b.z);
}

template<class T>
static TCoord3D<T> operator+(const TCoord3D<T> &a, const TCoord3D<T> &b)
{
	return TCoord3D<T>(a.x + b.x, a.y + b.y, a.z + b.z);
}

template<class T>
static TCoord3D<T> operator*(const TCoord3D<T> &a, T b)
{
	return TCoord3D<T>(a.x*b, a.y*b, a.z*b);
}

template<class T>
static TCoord3D<T> operator*(const TCoord3D<T> &a, const TCoord3D<T> &b)
{
	return TCoord3D<T>(a.x*b.x, a.y*b.y, a.z*b.z);
}

//! Division operators
template<class T>
static TCoord3D<T> operator/(const TCoord3D<T> &a, float b)
{
	return TCoord3D<T>(a.x/b, a.y/b, a.z/b);
}

template<class T>
static TCoord3D<T> operator/(float b, const TCoord3D<T> &a)
{
	return TCoord3D<T>(b/a.x, b/a.y, b/a.z);
}

//! Normalize the vector
template<class T>
static TCoord3D<T> normalize(const TCoord3D<T> &a)
{
	return a / magnitude(a);
}

template<class T>
static TCoord3D<T> operator*(T b, const TCoord3D<T> &a)
{
	return TCoord3D<T>(a.x*b, a.y*b, a.z*b);
}

//! Move to 3-space
template<class T>
static TCoord3D<T> to3Space(TCoord3D<T> &a)
{
	return a;
}

//! Upgrade from 3-space to 2-space
template<class T>
static TCoord3D<T> to3Space(TCoord2D<T> &a)
{
	return TCoord3D<T>(a.x, a.y, 0);
}


static float distance(const Coord3D &a, const Coord3D &b)
{
	const Coord3D d = a-b;
	return sqrtf(d.x*d.x + d.y*d.y + d.z*d.z);
}

#endif
