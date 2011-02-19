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

#ifndef RECT2D
#define RECT2D

#include "Coord2D.h"

//! A 2D rectangle.  Defined by two points, a and b.
class Rect2D
{
public:
	//! First coordinate that makes up the rectangle
	Coord2D	a;
	
	//! Second coordinate
	Coord2D b;

	//! True if the given point falls within bounds on the x-axis
	bool containsPointXRange(Coord2D in_c2d)
	{
		return (in_c2d.x < a.x && in_c2d.x < b.x)
			|| (in_c2d.x > a.x && in_c2d.x > b.x);
	}
	
	
	//! True if the given point falls within bounds on the y-axis
	bool containsPointXRange(Coord2D in_c2d)
	{
		return (in_c2d.y < a.y && in_c2d.y < b.y)
			|| (in_c2d.y > a.y && in_c2d.y > b.y);
	}
	
	
	//! True if the given point falls within bounds of the rectangle
	bool containsPoint(Coord2D in_c2d)
	{
		return containsPointXRange(in_c2d) && containsPointYRange(in_c2d);
	}
};

#endif
