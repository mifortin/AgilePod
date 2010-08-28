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

#include "Particle.h"

#ifndef FLOW_H
#define FLOW_H

////////////////////////////////////////////////////////////////////////////////
static float lerp(const float a, const float v1, const float v2)
{
	return (1-a)*v1 + a*v2;
}


static float lerp2x2(const float dx, const float dy,
						const float v11, const float v21,
						const float v12, const float v22)
{
	return lerp(dy,lerp(dx, v11, v21), lerp(dx, v12, v22));
}


template<int WIDTH, int HEIGHT, int COMPONENTS, class TYPE>
class Field
{
private:
	TYPE m_data[WIDTH*HEIGHT*COMPONENTS];
	
public:
	Field()	{	clear();	}
	
	inline void clear()
	{	memset(m_data, 0, sizeof(TYPE)*WIDTH*HEIGHT*COMPONENTS); }
	
	inline TYPE &operator()(int x, int y, int c=0)
	{	return m_data[x*COMPONENTS + y*COMPONENTS*WIDTH + c];	}
	
	inline int width()	const		{	return WIDTH;		}
	inline int height()	const		{	return HEIGHT;		}
	inline int components() const	{	return components;	}
	
	template<class CONV_FN, class BORDER_FN>
	void convolve(CONV_FN&in_stencil, BORDER_FN&in_b, int quality = 1)
	{
		int x,y,z,c;
		for (z=0; z<quality; z++)
		{
			for (x=1; x<WIDTH-1; x++)
			{
				for (c=0; c<COMPONENTS; c++)
					(*this)(x,0,c) = in_b.border((*this)(x,1,c));
			}
		
			for (y=1; y<HEIGHT-1; y++)
			{
				for (c=0; c<COMPONENTS; c++)
					(*this)(0,y,c) = in_b.border((*this)(1,y,c));
				
				for (x=1; x<WIDTH-1; x++)
				{
					for (c=0; c<COMPONENTS; c++)
					{
						(*this)(x,y,c)
							= in_stencil.stencil(*this, x, y, c);
					}
				} // end x
				
				for (c=0; c<COMPONENTS; c++)
					(*this)(WIDTH-1,y,c) = in_b.border((*this)(WIDTH-2,y,c));
			} // end y
			
			for (x=1; x<WIDTH-1; x++)
			{
				for (c=0; c<COMPONENTS; c++)
					(*this)(x,HEIGHT-1,c) = in_b.border((*this)(x,HEIGHT-2,c));
			}
		} // end z
	}
	
	
	template<class CONV_FN, class BORDER_FN_IN, class BORDER_FN_OUT, class BORDER_TYPE>
	void convolveBorder(CONV_FN&in_stencil,
						BORDER_FN_IN&in_bin,
						BORDER_FN_OUT&in_b,
						Field<WIDTH, HEIGHT, 1, BORDER_TYPE> &border,
						int quality = 1)
	{
		int x,y,z,c;
		for (z=0; z<quality; z++)
		{
			for (x=1; x<WIDTH-1; x++)
			{
				for (c=0; c<COMPONENTS; c++)
					(*this)(x,0,c) = in_b.border((*this)(x,1,c));
			}
		
			for (y=1; y<HEIGHT-1; y++)
			{
				for (c=0; c<COMPONENTS; c++)
					(*this)(0,y,c) = in_b.border((*this)(1,y,c));
				
				for (x=1; x<WIDTH-1; x++)
				{
					if (border(x,y) == 0)
					{
						for (c=0; c<COMPONENTS; c++)
						{
							(*this)(x,y,c)
								= in_stencil.stencil(*this, x, y, c);
						}
					}
					else
					{
						if (border(x-1,y) > 0)
						{
							for (c=0; c<COMPONENTS; c++)
								(*this)(x,y,c) = in_bin.border((*this)(x-1,y,c));
						}
						
						else if (border(x+1,y) > 0)
						{
							for (c=0; c<COMPONENTS; c++)
								(*this)(x,y,c) = in_bin.border((*this)(x+1,y,c));
						}
						
						else if (border(x,y-1) > 0)
						{
							for (c=0; c<COMPONENTS; c++)
								(*this)(x,y,c) = in_bin.border((*this)(x,y-1,c));
						}
						
						else if (border(x,y+1) > 0)
						{
							for (c=0; c<COMPONENTS; c++)
								(*this)(x,y,c) = in_bin.border((*this)(x,y+1,c));
						}
						else
						{
							for (c=0; c<COMPONENTS; c++)
								(*this)(x,y,c) = 0;
						}
							
					}
				} // end x
				
				for (c=0; c<COMPONENTS; c++)
					(*this)(WIDTH-1,y,c) = in_b.border((*this)(WIDTH-2,y,c));
			} // end y
			
			for (x=1; x<WIDTH-1; x++)
			{
				for (c=0; c<COMPONENTS; c++)
					(*this)(x,HEIGHT-1,c) = in_b.border((*this)(x,HEIGHT-2,c));
			}
		} // end z
	}
	
	
	
	
	
	template<class CONV_FN, class BORDER_FN, int C2>
	void convolve(Field<WIDTH, HEIGHT, C2, TYPE> &support,
					CONV_FN&in_stencil, BORDER_FN&in_b, int quality = 1)
	{
		int x,y,z,c;
		for (z=0; z<quality; z++)
		{
			for (x=1; x<WIDTH-1; x++)
			{
				for (c=0; c<COMPONENTS; c++)
					(*this)(x,0,c) = in_b.border((*this)(x,1,c));
			}
		
			for (y=1; y<HEIGHT-1; y++)
			{
				for (c=0; c<COMPONENTS; c++)
					(*this)(0,y,c) = in_b.border((*this)(1,y,c));
				
				for (x=1; x<WIDTH-1; x++)
				{
					for (c=0; c<COMPONENTS; c++)
					{
						(*this)(x,y,c)
							= in_stencil.stencil(*this, support, x, y, c);
					}
				} // end x
				
				for (c=0; c<COMPONENTS; c++)
					(*this)(WIDTH-1,y,c) = in_b.border((*this)(WIDTH-2,y,c));
			} // end y
			
			for (x=1; x<WIDTH-1; x++)
			{
				for (c=0; c<COMPONENTS; c++)
					(*this)(x,HEIGHT-1,c) = in_b.border((*this)(x,HEIGHT-2,c));
			}
		} // end z
	}
};


template<int WIDTH, int HEIGHT, int COMPONENTS, class TYPE>
class FlowViscosity
{
private:
	TYPE m_alpha;
	TYPE m_beta;
	
public:
	FlowViscosity(TYPE in_alpha, TYPE in_beta)
	: m_alpha(in_alpha)
	, m_beta(in_beta)
	{}


	inline float stencil(Field<WIDTH, HEIGHT, COMPONENTS, TYPE> &f,
								int x, int y, int c)
	{
		return  ((f(x, y, c))*m_alpha
				+ f(x+1, y, c)
				+ f(x-1, y, c)
				+ f(x, y+1, c)
				+ f(x, y-1, c))*m_beta;
	}
};


template<int WIDTH, int HEIGHT, int COMPONENTS, class TYPE>
class FlowPressure
{
public:
	inline float stencil(	Field<WIDTH, HEIGHT, 1, TYPE> &P,
							Field<WIDTH, HEIGHT, 2, TYPE> &V,
							int x, int y, int c)
	{
		return (	P(x+1,y)
				+ P(x-1,y)
				+ P(x,y+1)
				+ P(x,y-1)
				- (V(x+1,y,0)
					- V(x-1,y,0)
					+ V(x, y+1,1)
					- V(x, y-1,1) ) ) /4.0f;
	}
};


template<int WIDTH, int HEIGHT, int COMPONENTS, class TYPE>
class FlowBorder
{
private:
	TYPE m_border;

public:
	FlowBorder(TYPE in_border)
	: m_border(in_border)
	{}
	
	
	//Explanation: we are computing the inner value, but only know the value
	//outside the border.  This allows us to do very simple borders...
	inline float border(float outside)
	{
		return m_border * outside;
	}
};


////////////////////////////////////////////////////////////////////////////////
template<int FLOW_WIDTH, int FLOW_HEIGHT, int PRESSURE_QUALITY, int VISCOSITY_QUALITY>
class Flow
{
public:
	Field<FLOW_WIDTH, FLOW_HEIGHT, 2, float> m_velocity;
	Field<FLOW_WIDTH, FLOW_HEIGHT, 2, float> m_velocity2;
	Field<FLOW_WIDTH, FLOW_HEIGHT, 1, float> m_pressure;
	Field<FLOW_WIDTH, FLOW_HEIGHT, 1, float> m_density;

	int g_flowCurVel;
	
	Flow()
	{
		g_flowCurVel = 0;
	}

////////////////////////////////////////////////////////////////////////////////
	inline Field<FLOW_WIDTH, FLOW_HEIGHT, 2, float> &curField()
	{
		if (g_flowCurVel)
			return m_velocity;
		else
			return m_velocity2;
	}


	inline Field<FLOW_WIDTH, FLOW_HEIGHT, 2, float> &otherField()
	{
		if (g_flowCurVel)
			return m_velocity2;
		else
			return m_velocity;
	}


	inline void swapVelocity()
	{
		g_flowCurVel = 1 - g_flowCurVel;
	}
	
////////////////////////////////////////////////////////////////////////////////
	void advectSL(const float timestep)
	{
		Field<FLOW_WIDTH, FLOW_HEIGHT, 2, float> &src = curField();
		Field<FLOW_WIDTH, FLOW_HEIGHT, 2, float> &dst = otherField();
		
		int x;
		int y;
		
		float fx;
		float fy;
		
		for (y=0, fy=0; y<FLOW_HEIGHT; y++, fy++)
		{
			for (x=0, fx=0; x<FLOW_WIDTH; x++, fx++)
			{
				float dx = -timestep * src(x,y,0) + fx;
				float dy = -timestep * src(x,y,1) + fy;
				
				if (dx < 0)	dx = 0;
				if (dy < 0) dy = 0;
				
				if (dx >= FLOW_WIDTH-1)		dx = FLOW_WIDTH  - 1.001f;
				if (dy >= FLOW_HEIGHT-1)	dy = FLOW_HEIGHT - 1.001f;
				
				int ix = (int)dx;
				int iy = (int)dy;
				
				float sx = dx-ix;
				float sy = dy-iy;
				
				dst(x,y,0)
					= lerp2x2(sx, sy,
						src(ix, iy, 0),	src((ix+1), iy, 0),
						src(ix, (iy+1), 0),	src((ix+1), (iy+1), 0));
				
				dst(x,y,1)
					= lerp2x2(sx, sy,
						src(ix, iy, 1),	src((ix+1), iy, 1),
						src(ix, (iy+1), 1),	src((ix+1), (iy+1), 1));
			}
		}
		
		swapVelocity();
	}


////////////////////////////////////////////////////////////////////////////////
	template<class COL, int NUM>
	inline void updateParticles(ParticleSystem<Coord2D, COL, NUM> &particles,
						float multiplier, float densityMult)
	{
		Field<FLOW_WIDTH, FLOW_HEIGHT, 2, float> &field = curField();
	
		if (densityMult > 0)
			m_density.clear();
	
		int x;
		for (x=0; x<particles.maximumNumberOfParticles(); x++)
		{
			int px = (FLOW_WIDTH*particles.particles[x].pos.x / 480.0f + 0.5f);
			int py = (FLOW_HEIGHT*particles.particles[x].pos.y / 320.0f + 0.5f);
			
			if (px < 0)	px = 0;
			if (py < 0)	py = 0;
			if (px >= FLOW_WIDTH) px = FLOW_WIDTH-1;
			if (py >= FLOW_HEIGHT) py = FLOW_HEIGHT-1;
			
			particles.particles[x].velocity.x += field(px, py, 0)*multiplier;
			particles.particles[x].velocity.y += field(px, py, 1)*multiplier;
			
			if (densityMult > 0)
			{
				m_density(px, py) += particles.particles[x].size * densityMult;
			}
		}
	}



////////////////////////////////////////////////////////////////////////////////
	void viscosity(float viscosity, float timestep, float border)
	{
		Field<FLOW_WIDTH, FLOW_HEIGHT, 2, float> &src = curField();
		
		float alpha = 1.0f / viscosity * timestep / (float)VISCOSITY_QUALITY;
		float beta = 1.0f / (alpha + 4.0f);
		
		FlowViscosity<FLOW_WIDTH, FLOW_HEIGHT, 2, float> kernel(alpha, beta);
		FlowBorder<FLOW_WIDTH, FLOW_HEIGHT, 2, float> b(border);
		src.convolve(kernel, b, VISCOSITY_QUALITY);
	}
	
	
	void viscosityBorder(float viscosity, float timestep, float out_border, float in_border)
	{
		Field<FLOW_WIDTH, FLOW_HEIGHT, 2, float> &src = curField();
		
		float alpha = 1.0f / viscosity * timestep / (float)VISCOSITY_QUALITY;
		float beta = 1.0f / (alpha + 4.0f);
		
		FlowViscosity<FLOW_WIDTH, FLOW_HEIGHT, 2, float> kernel(alpha, beta);
		FlowBorder<FLOW_WIDTH, FLOW_HEIGHT, 2, float> bin(in_border);
		FlowBorder<FLOW_WIDTH, FLOW_HEIGHT, 2, float> bout(out_border);
		src.convolveBorder(kernel, bin, bout, m_density, VISCOSITY_QUALITY);
	}



////////////////////////////////////////////////////////////////////////////////
	void pressure()
	{
		Field<FLOW_WIDTH, FLOW_HEIGHT, 2, float> &src = curField();
		Field<FLOW_WIDTH, FLOW_HEIGHT, 1, float> &P = m_pressure;
		
		FlowPressure<FLOW_WIDTH, FLOW_HEIGHT, 2, float> kernel;
		FlowBorder<FLOW_WIDTH, FLOW_HEIGHT, 2, float> b(1);
		
		P.convolve(src, kernel, b, PRESSURE_QUALITY);
		
		int x,y;
		
		for (y=1; y<FLOW_HEIGHT-1; y++)
		{
			for (x=1; x<FLOW_WIDTH-1; x++)
			{
				src(x, y, 0) -= P(x+1,y) - P(x-1,y);
				src(x, y, 1) -= P(x,y+1) - P(x,y-1);
			}
		}
	}
};







#endif
