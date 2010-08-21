/*
 *  Camera.h
 *  GravitySpace
 *
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>

////////////////////////////////////////////////////////////////////////////////

class Coord2D
{
public:
	float x, y;
	
	Coord2D(float ix=0, float iy=0)
	: x(ix), y(iy) {}
	
	Coord2D &operator+=(const Coord2D &other)
	{
		x+=other.x;
		y+=other.y;
		return *this;
	}
	
	
	Coord2D &operator*=(float b)
	{
		x *= b;
		y *= b;
		return *this;
	}
};


static Coord2D operator-(const Coord2D &a, const Coord2D &b)
{
	return Coord2D(a.x - b.x, a.y - b.y);
}


static Coord2D operator*(const Coord2D &a, float b)
{
	return Coord2D(a.x*b, a.y*b);
}


static float distance(const Coord2D &a, const Coord2D &b)
{
	const Coord2D d = a-b;
	return sqrtf(d.x*d.x + d.y*d.y);
}


////////////////////////////////////////////////////////////////////////////////


class Coord3D
{
public:
	float x, y, z;
	
	Coord3D(float ix=0, float iy=0, float iz=0)
	: x(ix), y(iy), z(iz) {}
	
	Coord3D &operator+=(const Coord3D &other)
	{
		x+=other.x;
		y+=other.y;
		z+=other.z;
		return *this;
	}
};


static Coord3D operator-(const Coord3D &a, const Coord3D &b)
{
	return Coord3D(a.x - b.x, a.y - b.y, a.z - b.z);
}


static Coord3D operator*(const Coord3D &a, float b)
{
	return Coord3D(a.x*b, a.y*b, a.z*b);
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


////////////////////////////////////////////////////////////////////////////////

static float distance(const float a, const float b)
{
	return fabsf(a-b);
}

////////////////////////////////////////////////////////////////////////////////


template<class X>
class SlideCamera
{
public:
	X position;
	float maxDistance;
	X target;
	
	SlideCamera(X initialPos = X())
	: position(initialPos)
	, target(initialPos)
	, maxDistance(1)
	{}
	
	void update(float diff)
	{
		float d = distance(position, target);
		if (d > maxDistance*diff)
		{
			position += maxDistance*diff*(target-position)/d;
		}
		else
		{
			position = target;
		}
	}
	
	X& operator()()	{	return position;	}
};

typedef SlideCamera<float>		SlideCamera1D;
typedef SlideCamera<Coord2D>	SlideCamera2D;
typedef SlideCamera<Coord3D>	SlideCamera3D;

#endif
