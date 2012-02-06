/*
   Copyright 2010,2011,2012 Michael Fortin

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
#import "SmartMM.h"

//GCC doesn't like ARC-style bridge
#ifdef NO_ARC
#define BRIDGE
#else
#define BRIDGE	__bridge
#endif

#pragma mark Restore
class RestorerRestorePList : public Restorer
{
private:
	OneNS<NSDictionary*>	m_dictionary;

	RestorerRestorePList()
	{}

public:
	RestorerRestorePList(NSString *in_fileName)
	{
		NSData *d = [NSData dataWithContentsOfMappedFile:in_fileName];
		if (d != nil)
			m_dictionary = [NSPropertyListSerialization propertyListFromData:d mutabilityOption:NSPropertyListImmutable format:nil errorDescription:nil];
								
		if (m_dictionary == nil)
			m_dictionary = [NSDictionary dictionary];
	}
	
	
	//Restorer==================================================================
	void Object(CFStringRef in_key, Restorable *in_object)
	{
		assert(in_key && in_object);
		
		RestorerRestorePList r;
		r.m_dictionary = [m_dictionary() objectForKey:(BRIDGE NSString*)in_key];
		if (r.m_dictionary == nil)
			r.m_dictionary = [NSDictionary dictionary];
		
		in_object->handle(&r);
	}
	
	
	void Int(CFStringRef in_key, int *io_value, int in_default)
	{
		assert(io_value && in_key);
		
		NSNumber *n = [m_dictionary() objectForKey:(BRIDGE NSString*)in_key];
		
		if (n)		*io_value = [n intValue];
		else		*io_value = in_default;
	}
	
	
	void Float(CFStringRef in_key, float *io_value, float in_default)
	{
		assert(io_value && in_key);
		
		NSNumber *n = [m_dictionary() objectForKey:(BRIDGE NSString*)in_key];
		
		if (n)		*io_value = [n floatValue];
		else		*io_value = in_default;
	}
};


#pragma mark Save
class RestorerSavePList : public Restorer
{
private:
	OneNS<NSString*>				m_saveTo;
	OneNS<NSMutableDictionary*>		m_dictionary;

	RestorerSavePList()
	{}
	

public:
	RestorerSavePList(NSString *in_fileName)
	{
		m_saveTo = in_fileName;
		m_dictionary = [NSMutableDictionary dictionaryWithCapacity:5];
	}
	
	
	virtual ~RestorerSavePList()
	{
		if (m_saveTo != nil)
		{
			NSData *d = [NSPropertyListSerialization dataFromPropertyList:m_dictionary() format:NSPropertyListBinaryFormat_v1_0 errorDescription:nil];
			if (d == nil)
				throw "Failed obtaining data from save";
			
			if (![d writeToFile:m_saveTo() atomically:YES])
				throw "Failed writing data to file";
		}
	}
	
	
	//Restorer==================================================================
	void Object(CFStringRef in_key, Restorable *in_object)
	{
		assert(in_key && in_object);
		assert([m_dictionary() objectForKey:(BRIDGE NSString*)in_key] == nil);
		
		RestorerSavePList tmp;
		tmp.m_dictionary = [NSMutableDictionary dictionaryWithCapacity:5];
		[m_dictionary() setObject:tmp.m_dictionary() forKey:(BRIDGE NSString*)in_key];
		in_object->handle(&tmp);
	}
	
	
	void Int(CFStringRef in_key, int *io_value, int in_default)
	{
		assert(in_key && io_value);
		assert([m_dictionary() objectForKey:(BRIDGE NSString*)in_key] == nil);
		
		[m_dictionary() setObject:[NSNumber numberWithInt:*io_value]
						forKey:(BRIDGE NSString*)in_key];
	}
	
	
	void Float(CFStringRef in_key, float *io_value, float in_default)
	{
		assert(in_key && io_value);
		assert([m_dictionary() objectForKey:(BRIDGE NSString*)in_key] == nil);
		
		[m_dictionary() setObject:[NSNumber numberWithFloat:*io_value]
						forKey:(BRIDGE NSString*)in_key];
	}
};


static NSString* PathForFile(CFStringRef in_fileName)
{
	//First, scan in the documents directory...
	NSArray *paths = NSSearchPathForDirectoriesInDomains(
									NSDocumentDirectory,
									NSUserDomainMask,
									YES);
	
	NSString *tmp = (BRIDGE NSString*)in_fileName;
								
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
	
	if (foundFile == nil)
	{
		NSString *path = [paths objectAtIndex:0];
		foundFile = [path stringByAppendingPathComponent:tmp];
	}
	
	return foundFile;
}


Restorer *CreateSaveObject(CFStringRef in_fileName)
{
	return new RestorerSavePList(PathForFile(in_fileName));
}


Restorer *CreateRestoreObject(CFStringRef in_fileName)
{
	return new RestorerRestorePList(PathForFile(in_fileName));
}
