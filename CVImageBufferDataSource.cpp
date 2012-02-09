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
	
	//! Ranges of the colours for normalization purposes.
	unsigned char minB, minG, minR, maxG, maxB, maxR;
	
public:
	CVImageBufferDataSource()
	: minR(0), maxR(0), minG(0), maxG(0), minB(0), maxB(0)
	{
		m_ref = NULL;
		m_locked = false;
	}
	
	
	virtual void*data()
	{
		if (m_ref == NULL)	return NULL;
		
		if (!m_locked)
			CVPixelBufferLockBaseAddress(m_ref, 0);
		
		m_locked = true;
		
		unsigned char*data = (unsigned char*)CVPixelBufferGetBaseAddress(m_ref);
		Coord2DI s = size();
		
//		int i,j;
//		
//		minB = 255;
//		minG = 255;
//		minR = 255;
//		
//		maxG = 0;
//		maxB = 0;
//		maxR = 0;
//		
//		
//		for (j=0; j<s.y; j+=64)
//		{
//			for (i=0; i<s.x*4; i+=4)
//			{
//				int k = i + j*s.x*4;
//				if (data[k] < minB)	minB = data[k];
//				if (data[k+1] < minG)	minG = data[k+1];
//				if (data[k+2] < minR)	minR = data[k+2];
//				
//				if (data[k] > maxB)	maxB = data[k];
//				if (data[k+1] > maxG)	maxG = data[k+1];
//				if (data[k+2] > maxR)	maxR = data[k+2];
//			}
//		}
		
		return (void*)data;
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
			return Coord2DI((short)CVPixelBufferGetWidth(m_ref),
							(short)CVPixelBufferGetHeight(m_ref));
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
	
	virtual Coord3D minRGB()
	{
		return Coord3D((float)minR/255.0f, (float)minG/255.0f, (float)minB/255.0f);
	}
	
	virtual Coord3D maxRGB()
	{
		return Coord3D((float)maxR/255.0f, (float)maxG/255.0f, (float)maxB/255.0f);
	}
};

ICVImageBufferDataSource *CreateCVImageBufferDataSource()
{
	return new CVImageBufferDataSource;
}
