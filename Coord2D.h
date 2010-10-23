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

#ifndef COORD2D
#define COORD2D

#include <CoreGraphics/CoreGraphics.h>
#include "Restorer.h"

////////////////////////////////////////////////////////////////////////////////
class Coord2D;

static Coord2D operator/(const Coord2D &a, const float b);

class Coord2D : public CGPoint
{
public:
	
	Coord2D(float ix=0, float iy=0)
	: CGPoint(CGPointMake(ix, iy))
	{}
	
	Coord2D(CGPoint in_pt)
	: CGPoint(in_pt)
	{}
	
	Coord2D &operator+=(const Coord2D &other)
	{
		x+=other.x;
		y+=other.y;
		return *this;
	}
	
	Coord2D &operator-=(const Coord2D &a)
	{
		x -= a.x;
		y -= a.y;
		return *this;
	}
	
	inline float magnitude() const
	{
		return sqrtf(x*x + y*y);
	}
	
	inline Coord2D normal() const
	{
		return *this/magnitude();
	}
	
	inline Coord2D operator-() const
	{
		return Coord2D(-x, -y);
	}
	
	inline Coord2D clamp(float min, float max) const
	{
		Coord2D r = *this;
		
		if (r.x < min)	r.x = min;
		if (r.y < min)	r.y = min;
		
		if (r.x > max)	r.x = max;
		if (r.y > max)	r.y = max;
		
		return r;
	}
	
	Coord2D &operator*=(float b)
	{
		x *= b;
		y *= b;
		return *this;
	}
	
	Coord2D &operator/=(float in_b)
	{
		x /= in_b;
		y /= in_b;
		return *this;
	}
};


static float distanceSquared(const Coord2D &a, const Coord2D &b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	
	return dx*dx + dy*dy;
}


static Coord2D operator-(const Coord2D &a, const Coord2D &b)
{
	return Coord2D(a.x - b.x, a.y - b.y);
}

static Coord2D operator+(const Coord2D &a, const Coord2D &b)
{
	return Coord2D(a.x + b.x, a.y + b.y);
}

static Coord2D operator*(const Coord2D &a, const float b)
{
	return Coord2D(a.x*b, a.y*b);
}


static Coord2D operator*(const float b, const Coord2D &a)
{
	return Coord2D(a.x*b, a.y*b);
}


static Coord2D operator/(const Coord2D &a, const float b)
{
	return Coord2D(a.x/b, a.y/b);
}

static Coord2D operator/(const float b, const Coord2D &a)
{
	return Coord2D(b/a.x, b/a.y);
}

static float distance(const Coord2D &a, const Coord2D &b)
{
	const Coord2D d = a-b;
	return sqrtf(d.x*d.x + d.y*d.y);
}


static float magnitude(const Coord2D &a)
{
	return sqrtf(a.x*a.x + a.y*a.y);
}


static float dot(const Coord2D &a, const Coord2D &b)
{
	return a.x*b.x + a.y*b.y;
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
