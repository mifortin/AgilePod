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

Texture::Texture(const char *in_textureName)
: m_texID(0)
{
	int szLen = strlen(in_textureName);
	m_fileName.malloc(szLen+1);
	strcpy(m_fileName(), in_textureName);
}


void Texture::lazyLoad()
{
	CGImageRef texImage = [UIImage imageNamed:
							[NSString 	stringWithCString:m_fileName()
										encoding:NSUTF8StringEncoding]]
									.CGImage;
	
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
