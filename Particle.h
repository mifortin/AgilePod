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

#ifndef PARTICLE_H
#define PARTICLE_H

#include "Immediate.h"
#include "Camera.h"

template<class TYPE, class COLOUR>
struct particle
{
	TYPE pos;		//Position & velocity
	TYPE velocity;
	
	int time;		//Time left (0 = done!)

	COLOUR *map;	//Color
	
	float size;		//Size
	float dSize;	//Change in size
	
	particle()
	: time(0)
	, map(NULL)
	, size(0)
	, dSize(0)
	{}
};

template<class TYPE, class COLOUR, int NUM_PARTICLES>
class ParticleSystem
{
public:
	particle<TYPE, COLOUR> particles[NUM_PARTICLES];	//All particles!
	int c;										//Current particle
	
	ParticleSystem()
	: c(0)
	{}
	
	inline void addDynamic(COLOUR *col, TYPE start, TYPE vel, int timeout,
							float size, float dSize)
	{
		particles[c].pos = start;
		particles[c].velocity = vel;
		particles[c].time = timeout;
		particles[c].map = col;
		particles[c].size = size;
		particles[c].dSize = dSize;
	
		c++;
		if (c == NUM_PARTICLES)
			c = 0;
	}
	
	inline int maximumNumberOfParticles()	const
	{	return NUM_PARTICLES;	}
	
	inline void updateAndRender(float dt, float diffusion)
	{
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDepthMask(GL_FALSE);
		
		COLOUR black(0,0,0,0);
		
		int count;
		int i = c-1;
		for (count=0; count < NUM_PARTICLES; count++)
		{
			i++;
			
			if (i == NUM_PARTICLES)
				i = 0;
			
			if (particles[i].time > 0)
			{
				particles[i].time--;
				particles[i].size += particles[i].dSize;
				
				Coord3D pos = to3Space(particles[i].pos);
				
				float s = particles[i].size;
				
				{
					float cx = pos.x;
					float cy = pos.y;
					float r = s/2;
					
					glTranslatef(cx,cy,pos.z);
					glScalef(1.0f/POSITION_MULT, 1.0f/POSITION_MULT, 1.0f/POSITION_MULT);
					gl.begin(GL_TRIANGLE_FAN);
					{
						//Center
						gl.colour(particles[i].map[particles[i].time]);
						gl.vertex(0,0,0);
						
						//Outside...
						gl.colour(black);
						gl.vertex(sinf(0.0f)*r, cosf(0.0f)*r, 0.0f);
						gl.vertex(sinf(1.04f)*r, cosf(1.04f)*r, 0.0f);
						gl.vertex(sinf(2.08f)*r, cosf(2.08f)*r, 0.0f);
						gl.vertex(sinf(3.12f)*r, cosf(3.12f)*r, 0.0f);
						gl.vertex(sinf(4.20f)*r, cosf(4.20f)*r, 0.0f);
						gl.vertex(sinf(5.32f)*r, cosf(5.32f)*r, 0.0f);
						gl.vertex(sinf(0.0f)*r, cosf(0.0f)*r, 0.0f);
					}
					gl.end();
					glScalef(POSITION_MULT, POSITION_MULT, POSITION_MULT);
					glTranslatef(-cx,-cy,-pos.z);
				}
				
				particles[i].pos += particles[i].velocity*dt;
				particles[i].velocity *= diffusion;
			}
		}
		

		glDepthMask(GL_TRUE);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_TEXTURE_2D);
	}
};


#endif
