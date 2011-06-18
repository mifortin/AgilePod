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

#ifndef EVENTS_H
#define EVENTS_H

#include "Coord2D.h"
#include "Coord3D.h"
#include <vector>

//!For objects that can be drawn onto the screen...
class IDrawable
{
public:
	//! Called to do logic
	virtual void onLogic()													= 0;
	
	//! Called to draw
	virtual void onRender()													= 0;
};


//((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))


//! Accelerometer events
/*!	We do not encourage polling as the system should send fewer accelerometer
	events than there are frames. */
class IAccelerometer
{
public:
	//! Called for each event
	/*!	\param in_angle[in]		Raw accelerometer data */
	virtual void onAccelerate(Coord3D in_angle)		{}
	
	//! Determines desired rate of events
	/*!	\return		The frequency which accelerometer events should come in.
					0 to negative values disable the accelerometer.		*/
	virtual float accelerometerFrequency()			{	return -1;	}
};


//((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))


//!The touch events we can get
/*!	We first send a discovery touch event to determine which element is being
	touched.  That element subsequently receives multi-touch events.  This may
	cause nesting issues. */
class ITouchEvent
{
public:

	//! Return a pointer to the object that can handle the event.
	virtual ITouchEvent *canHandleEvent(Coord2D in_pos)						= 0;

	//! Send an event when a finger hits the screen.
	virtual void onPress(Coord2D in_pos)									= 0;
	
	//Position is provided to identify what should be released
	virtual void onRelease(Coord2D in_pos) 									= 0;
	
	//Moves point from source to destination
	virtual void onSwipe(Coord2D in_src, Coord2D in_dest) 					= 0;
};


//((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))


//!Sometimes we need to work in a resolution-independant way.
/*!	There are multiple ways to do this.  One is to take advantage of the extra
	pixels.

	The other is to assume that we should make the existing content bigger.
	This class makes stuff bigger. */
class AdjustForAspect
{
private:
	//!	Height that we wish to work with
	float m_height;
	
	//!	Width (scaled to match ratio)
	float m_width;
	
	//! Scaling ratio (can be used to get real width and height)
	float m_scale;

public:
	//!in_height is the height of the device
	AdjustForAspect(float in_height)
	: m_height(in_height)
	, m_width(in_height)
	, m_scale(1)
	{}
	
	//!Computes aspect-ratio information for future transformations
	void handleScreenSizeChange(float in_width, float in_height)
	{
		m_scale = in_height / m_height;
		
		m_width = in_width / m_scale;
	}
	
	//!Returns the usable range (height)
	float usableHeight()
	{
		return m_height;
	}
	
	//!And the usable width
	float usableWidth()
	{
		return m_width;
	}
	
	//!Looks like ITouchEvents, but isn't.  Translates the touch events...
	Coord2D translate(Coord2D in_pos)	{	return in_pos / m_scale;	}
	
	//! Scale factor
	float scale()
	{
		return m_scale;
	}
};


//((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))


//! TouchEvent distributor
/*!	Sometimes, certain views will scale or distort the touch events.  It is
	not the responsibility of the children to worry about this scaling/skewing
	of the event space.
 
	Note - this distributor may be problematic in multi-touch situations
 */
class TouchEventDistributor
{
private:
	//! Object that will handle the touch events
	ITouchEvent *m_handler;
	
	//! List of potential event handlers
	std::vector<ITouchEvent*> m_handlers;
	
	//! Count of fingers
	int m_fingers;
	
public:
	//! Initialize the distributor
	TouchEventDistributor()
	: m_handler(NULL)
	, m_fingers(0)
	{}
	
	//! Adds in an event handler
	/*!	\param	in_handler[in]	Potential event handler	*/
	void addHandler(ITouchEvent *in_handler)
	{
		m_handlers.push_back(in_handler);
	}
	
	//! Goes through the list of handlers to see who gets the event
	/*!	\param	in_pos[in]	The start position for events
		\return	true if a handler was found, else false.	*/
	bool canHandleEvent(Coord2D in_pos)
	{
		int x;
		for (x=0; x<m_handlers.size(); x++)
		{
			if (m_handlers[x]->canHandleEvent(in_pos))
			{
				m_handler = m_handlers[x];
				return true;
			}
		}
		
		m_handler = NULL;
		
		return false;
	}
	
	//! Called to run the press event
	/*!	\param in_pos[in]	Position from press event
		\return	true if we handled it, else false		*/
	bool onPress(Coord2D in_pos)
	{
		if (m_handler == NULL)		return false;
		m_handler->onPress(in_pos);
		m_fingers++;
		return true;
	}
	
	//! Called to run the release event
	/*!	\param in_pos[in]	Position from the release event
		\return	true if we handled the event			*/
	bool onRelease(Coord2D in_pos)
	{
		if (m_handler == NULL)		return false;
		m_handler->onRelease(in_pos);
		
		m_fingers--;
		if (m_fingers == 0)	m_handler = NULL;
		return true;
	}
	
	//! Called to handle swipe events
	/*!	\param	in_src[in]	Start of swipe
		\param	in_dest[in]	End of swipe
		\return	true if we handled the event		*/
	bool onSwipe(Coord2D in_src, Coord2D in_dest)
	{
		if (m_handler == NULL)		return false;
		m_handler->onSwipe(in_src, in_dest);
		return true;
	}
};

#endif
