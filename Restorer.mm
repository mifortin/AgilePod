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

#import "Restorer.h"

FileData::FileData(char *in_srcFile, bool readonly)
{
	//First, scan in the documents directory...
	NSArray *paths = NSSearchPathForDirectoriesInDomains(
									NSDocumentDirectory,
									NSUserDomainMask,
									YES);
	
	NSString *tmp = [NSString	stringWithCString:in_srcFile
								encoding:NSUTF8StringEncoding];
								
	NSFileManager *fm = [NSFileManager defaultManager];
	
	NSString *foundFile = nil;
	
	for (NSString *path in paths)
	{
		if ([fm	createDirectoryAtPath:path
				withIntermediateDirectories:YES
				attributes:nil
				error:NULL])
		{
			NSString *p = [path stringByAppendingPathComponent:tmp];
			
			if ([fm fileExistsAtPath:p])
			{
				foundFile = p;
				break;
			}
		}
	}
	
	if (foundFile == nil && readonly)
	{
		NSBundle *mb = [NSBundle mainBundle];
		
		foundFile = [mb 	pathForResource:[tmp stringByDeletingPathExtension]
							ofType:[tmp pathExtension]];
	}
	else if (foundFile == nil)
	{
		NSString *path = [paths objectAtIndex:0];
		foundFile = [path stringByAppendingPathComponent:tmp];
	}
	
	if (foundFile == nil)
		throw "File Not Found!";
	
	m_filePtr = fopen([foundFile UTF8String], readonly?"r" : "rw");
	if (m_filePtr() == NULL)
		throw "Unable to open file!";
}
