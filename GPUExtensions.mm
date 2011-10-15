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

#include "GPUExtensions.h"
#include <string.h>
#import <Foundation/Foundation.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

namespace GPU {
	namespace Support {

		static NSSet *g_exts			= nil;

		static void initExtensionsDictionary()
		{
			if (g_exts == nil)
			{
				NSArray *tmp;
				tmp = [[NSString stringWithCString:(const char*)glGetString(GL_EXTENSIONS) encoding:NSASCIIStringEncoding] componentsSeparatedByString:@" "];
				
				if (strncmp((const char*)glGetString(GL_VERSION), "OpenGL ES 2", 11) == 0)
				{
					tmp = [tmp arrayByAddingObjectsFromArray:[NSArray arrayWithObjects:@"GL_APPLE_texture_2D_limited_npot",nil]];
				}
				
				g_exts = [NSSet setWithArray:tmp];

#ifdef NO_ARC
				[g_exts retain];
#endif
			}
		}

		bool NPOT()
		{
			initExtensionsDictionary();
			return [g_exts containsObject:@"GL_APPLE_texture_2D_limited_npot"];
		}


		bool BGRATexture()
		{
			initExtensionsDictionary();
			return [g_exts containsObject:@"GL_APPLE_texture_format_BGRA8888"];
		}
		
		
		bool MapBuffer()
		{
			initExtensionsDictionary();
			return [g_exts containsObject:@"GL_OES_mapbuffer"];
		}
	} 
}