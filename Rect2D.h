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
	Coord2D	corner;
	
	//! Second coordinate
	Coord2D size;

	//! Initialize a rect2d
	/*! \param	in_x[in]		The x-coordinate of the 2D rect
		\param	in_y[in]		The y-coordinate of the 2D rect
		\param	in_width[in]	The width of the rectangle
		\param	in_height[in]	The height of the rectangle
	*/
	Rect2D(float in_x, float in_y, float in_width, float in_height)
	: corner(in_x, in_y)
	, size(in_width, in_height)
	{}

	//! Return point in the middle
	Coord2D center() const
	{
		return corner + size/2;
	}

	//! True if the given point falls within bounds on the x-axis
	inline bool containsPointXRange(Coord2D in_c2d) const
	{
		Coord2D a = corner;
		Coord2D b = corner + size;
	
		return (in_c2d.x < a.x && in_c2d.x > b.x)
			|| (in_c2d.x > a.x && in_c2d.x < b.x);
	}
	
	
	//! True if the given point falls within bounds on the y-axis
	inline bool containsPointYRange(Coord2D in_c2d) const
	{
		Coord2D a = corner;
		Coord2D b = corner + size;
		
		return (in_c2d.y < a.y && in_c2d.y > b.y)
			|| (in_c2d.y > a.y && in_c2d.y < b.y);
	}
	
	
	//! True if the given point falls within bounds of the rectangle
	inline bool containsPoint(Coord2D in_c2d) const
	{
		return containsPointXRange(in_c2d) && containsPointYRange(in_c2d);
	}
};

#endif
