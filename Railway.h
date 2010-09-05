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

#ifndef RAILWAY_H
#define RAILWAY_H

#include <vector>

//Many things follow some sort of rail.  Transitions follow a simple piece-wise
//curve; obstacles follow a fixed path (that can be deviated from sometimes).
//
//	These objects attempt to provide abstractions to deal with these objects
//	that should move/animate following a strict path.
//
//	The design is simple:
//		- 	we have tracks.  Tracks can be point-to-point linear, curves
//			(parametric or whatnot), etc.  All that we require is that these
//			track-pieces start at the origin.
//		-	a railroad is a combination of tracks.  Each railroad is a
//			combination of multiple tracks.  Each track can start at different
//			angles.
//		-	a train runs on the tracks at varying speeds.  (note that the
//			track themselves provide travel at a constant speed.  the train
//			can move along the tracks to provide ease-in, ease-out effects.
//			Give the illusion of acceleration.)


//Track
//	Each piece of track needs a way to report a position given a number of
//	pixels travelled and a length in pixels.
template<class C>
class Track
{
public:
	virtual C positionForPixelsTravelled(float in_pixels)			= 0;
	
	virtual float lengthInPixels()									= 0;
};


//LinearTrack
//	Segment that linearly interpolates to 1 point in C-Space from the origin
//		(use an object from Camera.h to specify the space)
template<class C>
class LinearTrack : public Track<C>
{
private:
	C m_endPoint;
	float m_length;

public:
	LinearTrack(C in_endPoint)
	{
		m_endPoint = in_endPoint;
		m_length = sqrtf(dot(C(), m_endPoint));
	}
	
	//Track=====================================================================
	virtual C positionForPixelsTravelled(float in_pixels)
	{
		return m_endPoint * in_pixels / m_length;
	}
	
	virtual float lengthInPixels()
	{
		return m_length;
	}
};


//Railway
//	A collection of tracks.  Where one track end, the next picks up (all
//	positioning is relative)
//
//	We take Track pointers - but assume that the main app actually maintains
//	the memory.
template<class C>
class Railway
{
private:
	std::vector< Track<C>* >		m_tracks;

public:
	void addTrack(Track<C> *in_track)
	{
		m_tracks.push_back(in_track);
	}
	
	
	size_t numberOfTracks()
	{
		return m_tracks.size();
	}
	
	
	Track<C> &operator[](int in_index)
	{
		return *(m_tracks[in_index]);
	}
};


//Train
//	Requires a railway to run upon.  Trains can only move relative to where
//	they are (n pixels forward for now)
template<class C>
class Train
{
private:
	C			m_origin;		//Current origin
	int			m_trackNum;		//Current track index
	float		m_pixels;		//Number of pixels in current track
	
	Track<C>	*m_track;		//The track
	Railway<C>	*m_rail;		//The rail that we run on!
	
public:
	Train(Railway<C> *in_rail)
	: m_rail(in_rail)
	, m_trackNum(0)
	, m_pixels(0)
	{
		m_track = &(*m_rail)[0];
	}
	
	//Advance our "train" by "n" pixels
	C advanceTrainByPixels(float in_pixels)
	{
		while (in_pixels + m_pixels  > m_track->lengthInPixels())
		{
			m_origin += m_track->positionForPixelsTravelled(
									m_track->lengthInPixels());
			in_pixels -= (m_track->lengthInPixels() - m_pixels);
			m_pixels = 0;
			
			if (m_trackNum + 1 < m_rail->numberOfTracks())
				m_trackNum++;
			
			m_track = &(*m_rail)[0];
		}
		
		m_pixels += in_pixels;
		return m_origin + m_track->positionForPixelsTravelled(m_pixels);
	}
};

#endif
