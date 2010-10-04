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

#ifndef COORD3D_H
#define COORD3D_H

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
	
	Coord3D &operator-=(const Coord3D &other)
	{
		x-=other.x;
		y-=other.y;
		z-=other.z;
		return *this;
	}
};


static float dot(const Coord3D &a, const Coord3D &b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}


static float distance(const Coord3D &a)
{
	return sqrtf(dot(a,a));
}


static Coord3D operator-(const Coord3D &a, const Coord3D &b)
{
	return Coord3D(a.x - b.x, a.y - b.y, a.z - b.z);
}


static Coord3D operator+(const Coord3D &a, const Coord3D &b)
{
	return Coord3D(a.x + b.x, a.y + b.y, a.z + b.z);
}


static Coord3D operator*(const Coord3D &a, float b)
{
	return Coord3D(a.x*b, a.y*b, a.z*b);
}

static Coord3D operator*(const Coord3D &a, const Coord3D &b)
{
	return Coord3D(a.x*b.x, a.y*b.y, a.z*b.z);
}

static Coord3D operator/(const Coord3D &a, float b)
{
	return Coord3D(a.x/b, a.y/b, a.z/b);
}

static Coord3D operator*(float b, const Coord3D &a)
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

#endif
