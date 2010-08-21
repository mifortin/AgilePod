/*
 *  DDTW.h
 *  
 *	Implementation of the Derivative Dynamic Time Warping Algorithm to match
 *	motions.
 */

#ifndef DDTW_H
#define DDTW_H

//Maximum number of entries in signal for processing
#ifndef DDTW_MAX_SIG_LEN
#define DDTW_MAX_SIG_LEN	256
#endif

//Hard-code (this will allow for loop unrolling - better in long run)
#ifndef DDTW_STRIDE
#define DDTW_STRIDE		3
#endif

//Storage for the series of data as it comes in...
static float g_ddtw_dx[DDTW_MAX_SIG_LEN*DDTW_STRIDE] = {0};
static float g_ddtw_dy[DDTW_MAX_SIG_LEN*DDTW_STRIDE] = {0};
static int g_ddtw_lx = 0;
static int g_ddtw_ly = 0;

//Dynamic programming buffer...
static float g_ddtw_d[DDTW_MAX_SIG_LEN*DDTW_MAX_SIG_LEN] = {0};

//Private functions!!!!
static void pvt_ddtwLoadDataN(float *dst, float *src, int amt)
{
	int x;
	int s;
	for (x=1; x<amt-1; x++)
	{
		for (s=0; s<DDTW_STRIDE; s++)
		{
			dst[s + DDTW_STRIDE*(x-1)] = src[s+DDTW_STRIDE*x];
//				= 0.5f * (src[s+DDTW_STRIDE*x] - src[s+DDTW_STRIDE*(x-1)]
//					+ 0.5f * (src[s+DDTW_STRIDE*(x+1)] - src[s+DDTW_STRIDE*(x-1)]));
		}
	}
}

//Functions to load data into the respective buffers
static void ddtwLoadDataA(float *src, int amt)
{
	pvt_ddtwLoadDataN(g_ddtw_dx, src, amt);
	g_ddtw_lx = amt-2;
}

static void ddtwLoadDataB(float *src, int amt)
{
	pvt_ddtwLoadDataN(g_ddtw_dy, src, amt);
	g_ddtw_ly = amt-2;
}


//Compute the distance of an n-sized vector
static float pvt_ddtwDistance(int x, int y)
{
	int s;
	float sum = 0;
	for (s=0; s<DDTW_STRIDE; s++)
	{
		float d = g_ddtw_dx[s+DDTW_STRIDE*x] - g_ddtw_dy[s+DDTW_STRIDE*y];
		sum += d*d;
	}
	
	return sum;
}


//Compute a time-warp...
static float ddtwWarp()
{
	int x,y;
	
	g_ddtw_d[0] = pvt_ddtwDistance(0,0);
	for (x=1; x<g_ddtw_lx; x++)
	{
		g_ddtw_d[x] = pvt_ddtwDistance(x,0) + g_ddtw_d[x-1];
	}
		
	for (y=1; y<g_ddtw_ly; y++)
	{
		g_ddtw_d[y*DDTW_MAX_SIG_LEN]
			= pvt_ddtwDistance(0,y) + g_ddtw_d[(y-1)*DDTW_MAX_SIG_LEN];
		
		for (x=1; x<g_ddtw_lx; x++)
		{
			float min = (g_ddtw_d[(x-1) + y*DDTW_MAX_SIG_LEN] 
					< g_ddtw_d[x + (y-1)*DDTW_MAX_SIG_LEN] ?
							g_ddtw_d[(x-1) + y*DDTW_MAX_SIG_LEN]
							: g_ddtw_d[x + (y-1)*DDTW_MAX_SIG_LEN]);
							
			min = (min 
					< g_ddtw_d[x-1 + (y-1)*DDTW_MAX_SIG_LEN] ?
							min
							: g_ddtw_d[x-1 + (y-1)*DDTW_MAX_SIG_LEN]);
							
			g_ddtw_d[x + y*DDTW_MAX_SIG_LEN]  = pvt_ddtwDistance(x,y) + min;
		}
	}
	
	return g_ddtw_d[g_ddtw_lx -1 + (g_ddtw_ly -1)*DDTW_MAX_SIG_LEN];
}

#endif
