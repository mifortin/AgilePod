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
	/*!	\param in_x[in]		X coordinate (default 0)
		\param in_y[in]		Y coordinate (default 0)
		\param in_z[in]		Z coordinate (default 0) */
	TCoord3D(T in_x = 0, T in_y = 0, T in_z = 0)
	: x(in_x), y(in_y), z(in_z) {}
	
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


static float dot(const Coord3D &a, const Coord3D &b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}


static float distance(const Coord3D &a)
{
	return sqrtf(dot(a,a));
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

template<class T>
static TCoord3D<T> operator/(const TCoord3D<T> &a, float b)
{
	return TCoord3D<T>(a.x/b, a.y/b, a.z/b);
}

template<class T>
static TCoord3D<T> operator*(T b, const TCoord3D<T> &a)
{
	return TCoord3D<T>(a.x*b, a.y*b, a.z*b);
}


static Coord3D to3Space(Coord3D &a)
{
	return a;
}


static Coord3D to3Space(Coord2D &a)
{
	return Coord3D(a.x, a.y, 0);
}


static float distance(const Coord3D &a, const Coord3D &b)
{
	const Coord3D d = a-b;
	return sqrtf(d.x*d.x + d.y*d.y + d.z*d.z);
}

#endif
