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

#ifndef MATRIX2D
#define MATRIX2D

#include "Coord2D.h"
#include "Angle.h"

//! Basic 2D matrix
class Matrix2D
{
public:
	//! First row
	Coord2D rows[2];
	
	//! Initialize as rotation matrix
	Matrix2D(const Angle &in_angle)
	{
		rows[0] = Coord2D(cosf(in_angle.radians()), -sinf(in_angle.radians()));
		rows[1] = Coord2D(sinf(in_angle.radians()), cosf(in_angle.radians()));
	}
	
	//! Basic multiplication
	Coord2D operator*(const Coord2D &in_c2d) const
	{
		return Coord2D(dot(rows[0], in_c2d), dot(rows[1], in_c2d));
	}
};


#endif
