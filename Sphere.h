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

#include "Camera.h"

//Defines a means of doing collisions among spherical objects in 2D
class Sphere2D
{
public:
	Coord2D		position;
	Coord2D		velocity;
	Coord2D		force;
	float		mass;
	float		radius;

	Sphere2D()
	{
		mass = 1;
		radius = 1;
	}

	//F = ma
	inline void addForce(const Coord2D in_force)
	{
		force += in_force / mass;
	}
	
	//Adds a force that slows down an object...
	inline void addResistiveForce(float in_resistance)
	{
		force -= mass * velocity * in_resistance;
	}
	
	//Adds a force that repulses two spheres...
	inline void addRepulsiveForce(const float in_k, const Sphere2D &in_other)
	{
		Coord2D vectorToSelf = position - in_other.position;
		float magnitude = vectorToSelf.magnitude();
		
		if (magnitude > radius +in_other.radius
			|| magnitude < 0.01f)
			return;
		
		addForce(in_k * (radius + in_other.radius - magnitude)
						* vectorToSelf / magnitude);
	}
	
	//Standard explicit euler integration
	inline void integrateEuler(const float in_timestep)
	{
		velocity += force * in_timestep;
		force = Coord2D(0,0);
		
		position += velocity * in_timestep;
	}
};

#endif
