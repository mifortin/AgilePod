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

#include "DataSource.h"
#include "Coord2D.h"

class CVImageBufferDataSource : public ICVImageBufferDataSource
{
private:
	//! Reference to next image buffer to present
	CVImageBufferRef m_ref;
	
	//! Do we need to unlock the buffer?
	bool			m_locked;
	
public:
	CVImageBufferDataSource()
	{
		m_ref = NULL;
		m_locked = false;
	}
	
	
	virtual void*data()
	{
		if (m_ref == NULL)	return NULL;
		
		CVPixelBufferLockBaseAddress(m_ref, 0);
		m_locked = true;
		return CVPixelBufferGetBaseAddress(m_ref);
	}
	
	
	virtual void releaseData()
	{
		if (m_ref)
		{
			if (m_locked)
			{
				CVPixelBufferUnlockBaseAddress(m_ref, 0);
				m_locked = false;
			}
			
			CVPixelBufferRelease(m_ref);
			m_ref = NULL;
		}
	}
	
	
	virtual void onLowMemory()
	{
		releaseData();
	}
	
	virtual Coord2DI size()
	{
		if (m_ref)
		{
			return Coord2DI(CVPixelBufferGetWidth(m_ref),
							CVPixelBufferGetHeight(m_ref));
		}
		else
			return Coord2DI(0,0);
	}
	
	
	virtual bool hasUpdatedData()
	{
		return (m_ref != NULL);
	}
	
	
	virtual void addCVImageBuffer(CVImageBufferRef in_ref)
	{
		if (in_ref)	CVPixelBufferRetain(in_ref);
		releaseData();
		
		m_ref = in_ref;
	}
	
	
	virtual ~CVImageBufferDataSource()
	{
		releaseData();
	}
};

ICVImageBufferDataSource *CreateCVImageBufferDataSource()
{
	return new CVImageBufferDataSource;
}
