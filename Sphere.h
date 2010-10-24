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

#ifndef SPHERE_H
#define SPHERE_H

#include "Coord2D.h"

//Defines a means of doing collisions among spherical objects in 2D
//	Using the Verlet integration scheme for second order integration...
class Sphere2DRestorer;
class Sphere2D
{
private:
	Coord2D		m_position;
	Coord2D		m_previous_position;
	Coord2D		m_acceleration;
	Coord2D		m_velocity;
	
	friend class Sphere2DRestorer;

public:
	float		mass;
	float		radius;

	Sphere2D()
	{
		mass = 1;
		radius = 1;
	}
	
	//In verlet integration, velocity can be derived from the positions...
	inline Coord2D velocity() const
	{
		return m_velocity;
	}
	
	inline Coord2D position() const
	{
		return m_position;
	}
	
	inline Coord2D previousPosition() const
	{
		return m_previous_position;
	}
	
	//Set the position - that is move without affecting underlying forces
	inline void setPosition(const Coord2D in_position)
	{
		m_position = in_position;
		m_previous_position = in_position;
		m_velocity = m_acceleration = Coord2D(0,0);
	}

	//F = ma
	inline void addForce(const Coord2D in_force)
	{
		m_acceleration += in_force / mass;
	}
	
	//Adds a force that slows down an object...
	inline void addResistiveForce(float in_resistance)
	{
		addForce(-mass * velocity() * in_resistance);
	}
	
	//Adds a force that repulses two spheres...
	inline void addRepulsiveForce(Sphere2D &in_other)
	{
		Coord2D vectorToSelf = position() - in_other.position();
		float magnitude = vectorToSelf.magnitude();
		
		if (magnitude > radius +in_other.radius
			|| magnitude < 0.01f)
			return;
		
		//Explicitly move the positions so that we never collide
		Coord2D center = (in_other.m_position + m_position)/2.0f;
		
		in_other.m_position += (in_other.m_position - center)
								/ (in_other.m_position - center).magnitude()
								* (radius + in_other.radius - magnitude) * 0.5f;
		
		m_position += (m_position - center)
								/ (m_position - center).magnitude()
								* (radius + in_other.radius - magnitude) * 0.5f;
	}
	
	//Newton's force of attraction.
	//	F = G (m1 * m2) / r^2
	//		F is the force
	//		G is the gravitational constant
	//		m masses of the points
	//		r distance (in meters) between points.
	inline void addNewtonsLawOfUniversalGravitation(Sphere2D &in_other,
													const float in_constant)
	{	
		Coord2D vectorToSelf = position() - in_other.position();
		float magnitude = vectorToSelf.magnitude();
		
		if (magnitude < 0.01f)
			return;
		
		vectorToSelf = (vectorToSelf / magnitude)
						* in_constant
						*(mass * in_other.mass / (magnitude * magnitude));
		
		addForce(-vectorToSelf);
		in_other.addForce(vectorToSelf);
	}
	
	inline void addNewtonsLawOfUniversalGravitationToSelf(const Sphere2D &in_other,
													const float in_constant)
	{	
		Coord2D vectorToSelf = position() - in_other.position();
		float magnitude = vectorToSelf.magnitude();
		
		if (magnitude < 0.01f)
			return;
		
		vectorToSelf = (vectorToSelf / magnitude)
						* in_constant
						*(mass * in_other.mass / (magnitude * magnitude));
		
		addForce(-vectorToSelf);
	}
	
	//Standard explicit euler integration
	//	F = ma
	//	v = da / dt
	//	p = dv / dt
	inline void integrate(const float in_timestep)
	{
		Coord2D current = m_position;
		m_position = 2 * m_position - m_previous_position
					+ in_timestep * in_timestep * m_acceleration;
		m_previous_position = current;
		m_acceleration = Coord2D(0,0);
		
		m_velocity = (m_position - m_previous_position) / in_timestep;
	}
};


////////////////////////////////////////////////////////////////////////////////
//	Sphere2DRestorer
//		Provides a means to serialize and deserialize a Sphere2D object...
class Sphere2DRestorer : public Restorable
{
private:
	Sphere2D			m_default;	
	Sphere2D			*m_toSave;

public:
	Sphere2DRestorer(Sphere2D *io_toSave, const Sphere2D in_default)
	{
		m_toSave = io_toSave;
		m_default = in_default;
	}
	
	void handle(Restorer *in_restore)
	{
		Coord2DRestorer position(&m_toSave->m_position, m_default.m_position);
		in_restore->Object(CFSTR("position"), &position);
		
		Coord2DRestorer previousPosition
							(&m_toSave->m_previous_position,
							  m_default.m_previous_position);
		in_restore->Object(CFSTR("previous position"), &previousPosition);
		
		Coord2DRestorer velocity(&m_toSave->m_velocity, m_default.m_velocity);
		in_restore->Object(CFSTR("velocity"), &velocity);
		
		Coord2DRestorer acceleration
						(&m_toSave->m_acceleration, m_default.m_acceleration);
		in_restore->Object(CFSTR("acceleration"), &acceleration);
		
		in_restore->Float(CFSTR("mass"), &m_toSave->mass, m_default.mass);
		in_restore->Float(CFSTR("radius"), &m_toSave->radius, m_default.radius);
	}
};

#endif
