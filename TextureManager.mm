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

#import "TextureManager.h"
#import <CoreFoundation/CoreFoundation.h>
#import "Immediate.h"

static NSDictionary *g_textureMapPlist = nil;

Texture::Texture(const char *in_textureName)
: m_texID(0)
{
	int szLen = strlen(in_textureName);
	m_fileName.malloc(szLen+1);
	strcpy(m_fileName(), in_textureName);
}


void Texture::lazyLoad()
{
	NSString *filePath = nil;
	
	UIDevice *cd = [UIDevice currentDevice];
	const char *szIdiom;
	if (cd.userInterfaceIdiom == UIUserInterfaceIdiomPad)
		szIdiom = "iPad";
	else
		szIdiom = "iPhone";
	
	int scale = (int)(gl.deviceScale() + 0.1f);
	
	//Enumerate each combination...
	NSBundle *mb = [NSBundle mainBundle];
	if (g_textureMapPlist == nil)
	{
		NSString *dataPath = [mb pathForResource:@"TextureMap" ofType:@"plist"];
		
		NSData *data = nil;
		
		if (dataPath == nil)
			NSLog(@"Unable to find TextureMap.plist");
		else
			data = [NSData dataWithContentsOfMappedFile:dataPath];
	
		if (data == nil)
			NSLog(@"Unable to open TextureMap.plist");
		else
			g_textureMapPlist
				= [NSPropertyListSerialization 	propertyListFromData:data
												mutabilityOption:0
												format:NULL
												errorDescription:nil];
		
		if (g_textureMapPlist == nil)
			NSLog(@"Unable to read TextureMap.plist as a plist");
	}
	
	if (g_textureMapPlist != nil)
	{
		NSString *fileName = nil;
		
		NSString *attempt = [NSString stringWithFormat:@"%s~%s@%i",
							m_fileName(), szIdiom, scale];
		fileName = [g_textureMapPlist valueForKey:attempt];
		
		if (fileName == nil)
		{
			fileName = [g_textureMapPlist valueForKey:
						[NSString stringWithFormat:@"%s~%s",
							m_fileName(), szIdiom]];
		}
		
		if (fileName == nil)
		{
			fileName = [g_textureMapPlist valueForKey:
						[NSString stringWithFormat:@"%s", m_fileName()]];
		}
		
		if (fileName != nil)
		{
			filePath = [mb pathForResource:fileName ofType:@"png"];
		}
	}
	
	if (filePath == nil)
	{
		filePath = [mb pathForResource:
					[NSString stringWithFormat:@"%s", m_fileName()]
					ofType:@"png"];
	}
	
	if (filePath == nil)
	{
		throw "Invalid File Path";
	}
	
	NSLog(@"Loading texture from: %@", filePath);
	
	CGImageRef texImage = [UIImage imageWithContentsOfFile:filePath].CGImage;
	
	if (texImage)
	{
		int width = CGImageGetWidth(texImage);
		int height = CGImageGetHeight(texImage);
		
		GLubyte *imageData = (GLubyte*)malloc(width*height*4);
		
		CGContextRef imageContext
			= CGBitmapContextCreate(imageData, width, height, 8, width*4,
									CGImageGetColorSpace(texImage),
									kCGImageAlphaPremultipliedLast);
		
		CGContextDrawImage(imageContext,
							CGRectMake(0,0,width,height),
							texImage);
		
		CGContextRelease(imageContext);
		
		glGenTextures(1, &m_texID);
		
		gl.useTexture(m_texID);
		gl.uploadImageData(imageData, width, height);
		
		free(imageData);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}
