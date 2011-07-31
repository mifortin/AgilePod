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

#include <sys/time.h>
#include "Smart.h"

#ifndef TIMER_H
#define TIMER_H

/*!	\file Timer.h
	\brief Simplified methods to time code
	
	Sometimes, an application needs to rely in change in time from the previous
	frame.  Passing the change in time between function calls convolutes the
	code.  This object provides a means to globally (and locally if needed)
	manage timers.	*/

//! Returns the current time in milliseconds
static double x_time()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	
	return (double)t.tv_sec + ((double)t.tv_usec) / 1000000.0;
}

//! Timer management
/*!	The timer is not inheriting from RC since it needs to be managed by the
	object handling the pausing of the game (thanks to modern OSs, this can
	be at any time).  Therefore; the timer is only useful if this state-
	management has a reference to the timer... */
class Timer
{
private:
	//!	Previous time (seconds)
	double	m_previousTime;
	
	//! Change in time since last call to "tick" (seconds)
	float	m_dt;
	
public:
	
	//! Init timer
	Timer()
	: m_dt(0)
	{
		m_previousTime = x_time();
	}
	
	//! Tick the timer (record the passing of a frame)
	void tick()
	{
		double newTime = x_time();
		
		m_dt = newTime - m_previousTime;
		
		m_previousTime = newTime;
	}
	
	//! Get the change in time in seconds
	float dt()	{	return m_dt;	}
	
	//! When the elapsed time will exceed reason (app went in background)
	/*!	Call this upon return from a long pause. */
	void unrelatedPause()	{	m_previousTime = x_time();	}
};


//! Global timer
/*!	Most applications will only need a single global timer.  This global
	timer should suffice.  If more timers are needed, than they can be
	created.	*/
Timer *GlobalTimer();

#endif