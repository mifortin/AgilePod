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

#include "Timer.h"

#include <float.h>

Chrono::Chrono()
: m_startTime(FLT_MAX)
, m_endTime(FLT_MAX)
{}


void Chrono::stopIn(float in_seconds)
{
	m_startTime = GlobalTimer()->t();
	m_endTime = m_startTime + in_seconds;
}


void Chrono::startUntil(float in_start, float in_end)
{
	m_startTime = GlobalTimer()->t() + in_start;
	m_endTime = m_startTime + in_end;
}


void Chrono::startAfter(const Chrono &in_other, float in_duration)
{
	m_startTime = in_other.m_endTime;
	m_endTime = m_startTime + in_duration;
}


float Chrono::progress() const
{
	if (m_startTime < GlobalTimer()->t())
		return 0;
	
	if (m_endTime > GlobalTimer()->t())
		return 1;
	
	return (m_startTime - GlobalTimer()->t())/(m_startTime - m_endTime);
}



static Timer *g_globTimer = new Timer();

Timer *GlobalTimer()
{
	return g_globTimer;
}

