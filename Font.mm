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

#include "Font.h"
#include "SmartMM.h"

#ifdef TARGET_OS_IPHONE
#import <CoreFoundation/CoreFoundation.h>
#import <UIKit/UIKit.h>
#else
#endif

class FontDataSource : public IImageDataSource
{
private:
	Many<char>			m_data;
	OneNS<NSString*>	m_characters;

public:

	FontDataSource(NSString *in_characters)
	: m_characters(in_characters)
	{
	}

	////////////////////////////////////////////////////////////////////////////
	//	IImageDataSoure
	virtual Coord2DI size()
	{
		return Coord2DI(512,512);
	}

	////////////////////////////////////////////////////////////////////////////
	//	IDataSource
	virtual void *data()
	{
		if (m_data() == NULL)
		{
			m_data = new char[4*512*512];
			memset(m_data(), 0, sizeof(char)*4*512*512);
			
			CGContextRef imageContext;
			
			imageContext = CGBitmapContextCreate(m_data(), 512, 512, 8, 512*4, CGColorSpaceCreateDeviceRGB(), kCGImageAlphaPremultipliedLast);
			
			UIGraphicsPushContext(imageContext);
			
			int l = [m_characters() length];
			int k;
			NSString *tmp;
			
			Coord2D pt(0,0);
			
			UIFont *font = [UIFont systemFontOfSize:32];
			
			for (k=0; k<l; k++)
			{
				NSRange r = NSMakeRange(k, 1);
				tmp = [m_characters() substringWithRange:r];
				
				CGSize s = [tmp drawAtPoint:pt withFont:font];
				
				if (s.width + pt.x >= 512)
				{
					pt.y += s.height + 2;
					pt.x = 0;
					[tmp drawAtPoint:pt withFont:font];
				}
				
				pt.x += s.width+2;
				
				NSLog(@"%f", pt.x);
			}
			
			UIGraphicsPopContext();
			CGContextRelease(imageContext);
		}
	
		return m_data();
	}
	
	virtual void onLowMemory()
	{
		m_data = NULL;
	}
};


int createFontData()
{
	FontDataSource ds(@"abcdefghijklmnopqrstuvwxyz"
						"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
						"0123456789+-*/="
						",.;:'\"[]{}\\|_&%$#@!");
	ds.data();
	return 0;
}

