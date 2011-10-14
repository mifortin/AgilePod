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

#ifndef ANGLE_H
#define ANGLE_H

/*! \file Angle.h
	\brief An easier way to work with angles.
 
	Angles are interesting since they wrap around (logically).  These objects
	work on angles in a space that ranges from -M_PI to M_PI such that
	-M_PI = M_PI. */

#include <cmath>

/*!	Represents an angle and ways to deal with their circular behaviour.
	\tparam S	The representation (usually float) */
template<class S>
class TAngle
{
private:
	//!Internal representation of the angle (always between -PI and PI
	S m;
	
	//! Ensure that a given angle (usually for user input) is in the desired
	//! range...
	void fixAngle(S &in_v)
	{
		while (in_v < -M_PI)
			in_v += 2*M_PI;
		
		while (in_v > M_PI)
			in_v -= 2*M_PI;
	}
	
public:
	//! 0 angle
	TAngle()			{}
	
	//! Angle from scalar
	TAngle(S in_init)
	{
		setAngle(in_init);
	}
	
	//! Assign an angle
	void setAngle(S in_v)
	{
		fixAngle(in_v);
		m = in_v;
	}
	
	//! Nice way to assign an angle
	TAngle&operator=(S in_v)
	{
		fixAngle(in_v);
		m = in_v;
		return *this;
	}
	
	//! Add angles
	TAngle operator+(const TAngle &in_o)
	{
		TAngle r(in_o.m + m);
		return r;
	}
	
	//! Subtract angles
	TAngle operator-(const TAngle &in_o)
	{
		TAngle r(m - in_o.m);
		return r;
	}
	
	//! Increase angle
	TAngle&operator += (const TAngle &in_o)
	{
		m += in_o.m;
		fixAngle(m);
		return *this;
	}
	
	//! Decrease angle
	TAngle&operator -= (const TAngle &in_o)
	{
		m-=in_o.m;
		fixAngle(m);
		return *this;
	}
	
	//! Obtain cos of the value
	S cos()		const		{	return std::cos(m);		}
	S sin()		const		{	return std::sin(m);		}
	
	//! In range?	( normal operators make no sense, always gt and lt)
	/*! Why not angles?  They wrap, this is actually easier for the user... */
	bool inRange(const S in_start, const S in_end) const
	{
		//Achieved by moving the start and end until m should be within.
		S a1 = in_start;
		S a2 = in_end;
		
		while (a1 > m)
		{
			a1 -= 2*M_PI;
			a2 -= 2*M_PI;
		}
		
		while (a2 < m)
		{
			a1 += 2*M_PI;
			a2 += 2*M_PI;
		}
		
		return a1 <= m && m <= a2;
	}
	
	//! Angle in degrees (-180 to 180)
	S degrees()			const	{	return m*180/M_PI;	}
	
	//! Angle in radians
	S radians()			const	{	return m;			}
};

/*! an Angle is a TAngle specialized for float data */
typedef TAngle<float> Angle;

#endif
