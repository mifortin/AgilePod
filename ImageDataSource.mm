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
#import "SmartMM.h"
#import "Immediate.h"



/*!	\file ImageDataSource.cpp
	\brief Specification of the image data source for images stored in files.
	
	Simply, we route as much as possible to the OS.  It does a good job!
*/

//! Mapping from texture names to texture data.
static OneNS<NSDictionary*> g_textureMapPlist;

//! Object that represents an image file source
/*! This object is an opaque data type representing an image.
*/
class ImageFileDataSource : public IImageDataSource
{
private:
	OneNS<NSString*>		m_fileName;
	Many<GLubyte>			m_fileData;
	Coord2DI				m_size;

public:
	ImageFileDataSource(const char *in_szFile)
	: m_size(0,0)
	{
		m_fileName = [NSString stringWithUTF8String:in_szFile];
	}
	
	void loadData(NSString *in_fileName)
	{
		NSBundle *mb = [NSBundle mainBundle];
		
		NSString *filePath = [mb pathForResource:in_fileName ofType:@"png"];
		if (filePath == nil)
			return;
		
		NSLog(@"Loading texture from: %@", filePath);
		UIImage *uiImg = [UIImage imageWithContentsOfFile:filePath];
		if (uiImg == nil)
			throw "File not loaded";
		
		CGImageRef texImage = uiImg.CGImage;
		if (texImage == nil)
			throw "Underlying CGImageRef was NULL";
		
		int width = CGImageGetWidth(texImage);
		int height = CGImageGetHeight(texImage);
		
		m_size.x = width;
		m_size.y = height;
		
		m_fileData = new GLubyte[width*height*4];
		
		CGContextRef imageContext
			= CGBitmapContextCreate(m_fileData(), width, height, 8, width*4,
									CGImageGetColorSpace(texImage),
									kCGImageAlphaPremultipliedLast);
		
		CGContextDrawImage(imageContext,
							CGRectMake(0,0,width,height),
							texImage);
		
		CGContextRelease(imageContext);
	}
	
////////////////////////////////////////////////////////////////////////////////
//	IDataSource

	virtual void *data()
	{
		//Do we have have any data on file?
		if (m_fileData() != NULL)
			return m_fileData();
		
		//Determine device
		UIDevice *cd = [UIDevice currentDevice];
		const char *szIdiom = "iPhone";
		
		if ([cd respondsToSelector:@selector(userInterfaceIdiom)])
		{
			if (cd.userInterfaceIdiom == UIUserInterfaceIdiomPad)
				szIdiom = "iPad";
			else
				szIdiom = "iPhone";
		}
		
		NSBundle *mb = [NSBundle mainBundle];
		
		//See if we should load any blending modes
		NSDictionary *blend = nil;
		if (g_textureMapPlist() == nil)
		{
			NSString *path = [mb pathForResource:@"TextureMap" ofType:@"plist"];
			
			NSData *rootData = nil;
			if (path != nil)
				rootData = [NSData dataWithContentsOfMappedFile:path];
			
			if (rootData != nil)
				g_textureMapPlist
					= [NSPropertyListSerialization	propertyListFromData:rootData
													mutabilityOption:0
													format:NULL
													errorDescription:nil];
		}
		
		if (g_textureMapPlist() != nil)
			blend = [g_textureMapPlist() objectForKey:m_fileName()];
		
		int scale = (int)(gl.deviceScale() + 0.1f);
		loadData([NSString stringWithFormat:@"%@~%s@%i",
								m_fileName(), szIdiom, scale]);
		
		if (m_fileData() == NULL)
			loadData([NSString stringWithFormat:@"%@~%s", m_fileName(), szIdiom]);
		
		if (m_fileData() == NULL)
			loadData(m_fileName());
		
		return m_fileData();
	}
	
	
	virtual void onLowMemory()
	{
		m_fileData = NULL;	//Trash the image data
	}


////////////////////////////////////////////////////////////////////////////////
//	IImageDataSource
	virtual Coord2DI size()
	{
		if (m_size.x == 0)
			data();			//Load up the image if it has not already been loaded
		return m_size;		//Return the size.
	}
};


IImageDataSource *CreateImageDataSourceFromFile(const char* in_szFile)
{
	return new ImageFileDataSource(in_szFile);
}
