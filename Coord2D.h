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

#ifndef COORD2D
#define COORD2D

#include <CoreGraphics/CoreGraphics.h>
#include "Restorer.h"
#include "Angle.h"


/*!	\file	Coord2D.h
	\brief	Objects to help simplify the use of 2D coordinates
	
	Often-times, it is clearer when writing code in compact vector notation;
	which is the point of these objects.
 
	Coord2D operates on floating-point values.
	Coord2DI operates in integers.
*/



////////////////////////////////////////////////////////////////////////////////
//! 2D Coordinate
/*!	\tparam	T	The basic type that the coordinates are made of.  Use Coord2D
				for floats.  Coord2DI for 16-bit integers.	*/
template<class T>
class TCoord2D
{
public:
	//! The x and y coordinates
	T x,y;
	
	//! Initialize with two explicit values
	TCoord2D(T ix=0, T iy=0)
	: x(ix), y(iy)
	{}
	
	//! Conversion from a CGPoint
	TCoord2D(CGPoint in_pt)
	: x(in_pt.x), y(in_pt.y)
	{}
	
	//! Increment the components
	TCoord2D &operator+=(const TCoord2D &other)
	{
		x+=other.x;
		y+=other.y;
		return *this;
	}
	
	//! Decrement the components
	TCoord2D &operator-=(const TCoord2D &a)
	{
		x -= a.x;
		y -= a.y;
		return *this;
	}
	
	//! Subtract two vectors
	TCoord2D operator-(const TCoord2D &b) const
	{
		return TCoord2D(x - b.x, y - b.y);
	}
	
	//! Sum two vectors
	TCoord2D operator+(const TCoord2D &b) const
	{
		return TCoord2D(x + b.x, y + b.y);
	}
	
	//! Product with a scalar
	TCoord2D operator*(const float b) const
	{
		return TCoord2D(x*b, y*b);
	}
	
	//! Component-wise product
	TCoord2D operator*(const TCoord2D &b) const
	{
		return TCoord2D(x*b.x, y*b.y);
	}
	
	
	//! Component-wise division
	TCoord2D operator/(const TCoord2D &b) const
	{
		return TCoord2D(x/b.x, y/b.y);
	}
	
	//! Divide by scalar
	TCoord2D operator/(const float b) const
	{
		return TCoord2D(x/b, y/b);
	}

	//! Compute the magnitude (distance to origin)
	inline float magnitude() const
	{
		return sqrtf(x*x + y*y);
	}
	
	//! Normalize
	inline TCoord2D normal() const
	{
		return *this/magnitude();
	}
	
	//! Negate
	inline TCoord2D operator-() const
	{
		return TCoord2D(-x, -y);
	}
	
	inline bool operator==(const TCoord2D &other)
	{
		return x == other.x && y == other.y;
	}
	
	
	inline bool operator!=(const TCoord2D &other)
	{
		return x != other.x || y != other.y;
	}
	
	inline TCoord2D clamp(float min, float max) const
	{
		TCoord2D r = *this;
		
		if (r.x < min)	r.x = min;
		if (r.y < min)	r.y = min;
		
		if (r.x > max)	r.x = max;
		if (r.y > max)	r.y = max;
		
		return r;
	}
	
	TCoord2D &operator*=(float b)
	{
		x *= b;
		y *= b;
		return *this;
	}
	
	TCoord2D &operator/=(float in_b)
	{
		x /= in_b;
		y /= in_b;
		return *this;
	}
	
	operator CGPoint()
	{
		return CGPointMake(x, y);
	}
};

//! Specialize 2D coordinates for floats
typedef TCoord2D<float> Coord2D;

//! Specialize 2D coordinates for ints
typedef TCoord2D<short> Coord2DI;

//! Divides scalar by Coord2D
template<class T>
static TCoord2D<T> operator/(const T b, const TCoord2D<T> &a)
{
	return TCoord2D<T>(b/a.x, b/a.y);
}

//! Multiplies Coord2D by scalar.
template<class T>
static TCoord2D<T> operator*(const float b, const TCoord2D<T> &a)
{
	return TCoord2D<T>(a.x*b, a.y*b);
}

//! Compute the distance between two points
static float distance(const Coord2D &a, const Coord2D &b)
{
	const Coord2D d = a-b;
	return sqrtf(d.x*d.x + d.y*d.y);
}

//! Computes atan2 on a Coord2D
/*!	This function forwards the values in a to atan2 stdlib call.
	\param a[in]	Coord2D
	\return			angle, in radians	*/
static Angle atan2(const Coord2D a)
{
	return Angle(atan2f(a.y, a.x));
}


//! Compute the square of the distance between two coordinates
/*!	\param	a[in]	First coordinate
	\param	b[in]	Second coordinate
	\return			Distance squared between a and b.	*/
static float distanceSquared(const Coord2D &a, const Coord2D &b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	
	return dx*dx + dy*dy;
}


//! Check to see if all components of a are greater than all of b.
/*!	\param	a[in]	Coordinate
	\param	b[in]	Second coordinate
	\return			True if a.x >= b.x and a.y >= b.y
 
	This is useful when checking bounds, for example:
\code
if (a >= Coord2D(10,10) && a <= Coord2D(100,100))
{
	//a is within the box formed by 10,10 and 100,100
}
\endcode
*/
static bool operator>=(const Coord2D &a, const Coord2D &b)
{
	return a.x >= b.x && a.y >= b.y;
}


//! Check to see if all components of a are less than than all of b.
/*!	\param	a[in]	Coordinate
	\param	b[in]	Second coordinate
	\return			True if a.x <= b.x and a.y <= b.y
 
	This is useful when checking bounds, for example:
\code
if (a >= Coord2D(10,10) && a <= Coord2D(100,100))
{
	//a is within the box formed by 10,10 and 100,100
}
\endcode
 */
static bool operator<=(const Coord2D &a, const Coord2D &b)
{
	return a.x <= b.x && a.y <= b.y;
}


static float magnitude(const Coord2D &a)
{
	return sqrtf(a.x*a.x + a.y*a.y);
}

//! Dot product of two Coord2D objects
/*!	\param a[in]	First Coord2D object
	\param b[in]	Second Coord2D object
	\return			dot product of both	*/
static float dot(const Coord2D &a, const Coord2D &b)
{
	return a.x*b.x + a.y*b.y;
}


//! Takes the floor of the respective components of a Coord2D
/*!	\param	a[in]	Arbitrary Coord2D
	\return			The floor of a.	*/
static Coord2D floor(const Coord2D &a)
{
	return Coord2D(floorf(a.x), floorf(a.y));
}


////////////////////////////////////////////////////////////////////////////////
//		Interop object that allows us to save/restore a Coord2D...
//
//			Meant to be used in the scope of the saving function...
class Coord2DRestorer : public Restorable
{
private:
	Coord2D m_default;
	Coord2D *m_toSave;

public:
	Coord2DRestorer(Coord2D *io_toSave, const Coord2D in_default)
	{
		m_default = in_default;
		m_toSave = io_toSave;
	}
	
	void handle(Restorer *in_restore)
	{
		in_restore->Float(CFSTR("x"), &m_toSave->x, m_default.x);
		in_restore->Float(CFSTR("y"), &m_toSave->y, m_default.y);
	}
};

#endif
