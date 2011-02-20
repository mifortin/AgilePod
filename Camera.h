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

#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>
#include "Coord2D.h"
#include "Coord3D.h"

////////////////////////////////////////////////////////////////////////////////

static inline float distance(float in_f)
{
	return in_f;
}



//! Compute the distance between two floating-point values
/*!
	\param in_a[in]		A real number
	\param in_b[in]		A real number
	\return A value, greater than or equal to zero, the distance between a and b.
*/
static float distance(const float in_a, const float in_b)
{
	return fabsf(in_a - in_b);
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
