/*
 *	 Copyright 2011 Michael Fortin
 *	 
 *	 Licensed under the Apache License, Version 2.0 (the "License");
 *	 you may not use this file except in compliance with the License.
 *	 You may obtain a copy of the License at
 *	 
 *		 http://www.apache.org/licenses/LICENSE-2.0
 *	 
 *	 Unless required by applicable law or agreed to in writing, software
 *	 distributed under the License is distributed on an "AS IS" BASIS,
 *	 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	 See the License for the specific language governing permissions and
 *	 limitations under the License.
 */

#ifndef FirstTest_Matrix3D_h
#define FirstTest_Matrix3D_h

#include "Coord3D.h"
#include "Angle.h"

//! Abstract object that represents a 3D matrix.
template<class T>
class TMatrix3D
{
private:
	//! Dot along the columns of X
	T dotX(const TCoord3D<T> r) const
	{	return rows[0].x*r.x + rows[1].x*r.y + rows[2].x*r.z;	}
	
	//! Dot along the columns of Y
	T dotY(const TCoord3D<T> r) const
	{	return rows[0].y*r.x + rows[1].y*r.y + rows[2].y*r.z;	}
	
	//! Dot along the columns of Z
	T dotZ(const TCoord3D<T> r) const
	{	return rows[0].z*r.x + rows[1].z*r.y + rows[2].z*r.z;	}

public:
	TCoord3D<T>	rows[3];	//!< The three rows of the matrix
	
	//!Create a rotation matrix for an angle around a vector
	/*! See http://en.wikipedia.org/wiki/Rotation_matrix */
	TMatrix3D(TAngle<T> angle, TCoord3D<T> vector)
	{
		TCoord3D<T> n = normalize(vector);
		
		rows[0].x = angle.cos() + vector.x*vector.x*(1 - angle.cos());
		rows[0].y = vector.x*vector.y*(1-angle.cos()) - vector.z*angle.sin();
		rows[0].z = vector.x*vector.z*(1-angle.cos()) + vector.y*angle.sin();
		
		rows[1].x = vector.y*vector.x*(1-angle.cos()) + vector.z*angle.sin();
		rows[1].y = angle.cos() + vector.y*vector.y*(1-angle.cos());
		rows[1].z = vector.y*vector.z*(1-angle.cos()) - vector.x*angle.sin();
		
		rows[2].x = vector.z*vector.x*(1-angle.cos()) - vector.y*angle.sin();
		rows[2].y = vector.z*vector.y*(1-angle.cos()) + vector.x*angle.sin();
		rows[2].z = angle.cos() + vector.z*vector.z*(1-angle.cos());
	}
	
	//! Scalar scale (can be used to make identity)
	TMatrix3D(T in_scale = 1)
	{
		rows[0].x = in_scale;
		rows[1].y = in_scale;
		rows[2].z = in_scale;
	}
	
	//! Matrix multiplication
	TMatrix3D	operator *(const TMatrix3D &m)		const
	{
		TMatrix3D r;
		
		r.rows[0].x = m.dotX(rows[0]);
		r.rows[0].y = m.dotY(rows[0]);
		r.rows[0].z = m.dotZ(rows[0]);
		
		r.rows[1].x = m.dotX(rows[1]);
		r.rows[1].y = m.dotY(rows[1]);
		r.rows[1].z = m.dotZ(rows[1]);
		
		r.rows[2].x = m.dotX(rows[2]);
		r.rows[2].y = m.dotY(rows[2]);
		r.rows[2].z = m.dotZ(rows[2]);
		
		return r;
	}
	
	
	//! Scalar multiplication
	TMatrix3D	operator *(const T &m)		const
	{
		TMatrix3D r;
		
		r.rows[0] = rows[0] * m;
		r.rows[1] = rows[1] * m;
		r.rows[2] = rows[2] * m;
		
		return r;
	}
	
	
	//! Matrix addition
	TMatrix3D	operator +(const TMatrix3D &m)		const
	{
		TMatrix3D r;
		
		r.rows[0] = rows[0] + m.rows[0];
		r.rows[1] = rows[1] + m.rows[1];
		r.rows[2] = rows[2] + m.rows[2];
		
		return r;
	}
};


typedef TMatrix3D<float>	Matrix3D;

#endif
