/*
 Copyright 2012 Michael Fortin
 
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

class PointerDataSource : public IPointerDataSource
{
private:
	//! Reference to next image buffer to present
	void *m_data;
	
	Coord2DI m_size;
	
public:
	PointerDataSource(Coord2DI &in_size)
	: m_size(in_size)
	{
		m_data = malloc(m_size.x*m_size.y*4);
	}
	
	
	virtual void*data()
	{
		return m_data;
	}

	
	virtual void releaseData()
	{
	}
	
	
	virtual void onLowMemory()
	{
		releaseData();
	}
	
	virtual Coord2DI size()
	{
		return m_size;
	}
	
	
	virtual bool hasUpdatedData()
	{
		return (m_data != NULL);
	}
	
	~PointerDataSource()
	{
		if (m_data)	free(m_data);
	}
};



IPointerDataSource *CreatePointerDataSource(Coord2DI in_c2d)
{
	return new PointerDataSource(in_c2d);
}

