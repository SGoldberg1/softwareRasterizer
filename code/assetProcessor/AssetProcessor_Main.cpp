
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "../Engine_Platform.h"
#include "../Engine_Memory.h"
#include "../Engine_Math.h"
#include "AssetProcessor_Common.h"
#include "AssetProcessor_Main.h"

#include "math.h"

inline f32 
Math_DotProductV3(v3 a, v3 b)
{
	f32 result = (a.X * b.X + a.Y * b.Y + a.Z * b.Z);
	return(result);
}

inline v3
Math_CrossProductV3(v3 a, v3 b)
{
	v3 result;
	result.X = (a.Y * b.Z) - (a.Z * b.Y);
	result.Y = (a.Z * b.X) - (a.X * b.Z);
	result.Z = (a.X * b.Y) - (a.Y * b.X);
	return(result);
}

inline f32
Math_Sqrt(f32 value)
{
	f32 result = sqrtf(value);
	return(result);
}

inline f32 
Math_SquaredMagnitudeV2(v2 a)
{
	f32 result = (a.X * a.X + a.Y * a.Y);
	return(result);
}

inline f32
Math_MagnitudeV2(v2 a)
{
	f32 result = Math_Sqrt(Math_SquaredMagnitudeV2(a));
	return(result);
}

inline f32 
Math_SquaredMagnitudeV3(v3 a)
{
	f32 result = (a.X * a.X + a.Y * a.Y + a.Z * a.Z);
	return(result);
}

inline f32
Math_MagnitudeV3(v3 a)
{
	f32 result = Math_Sqrt(Math_SquaredMagnitudeV3(a));
	return(result);
}


inline v3
Math_NormalizedV3(v3 a, f32 magnitude)
{
	Assert(magnitude != 0.0f);
	v3 result;
	result.X = a.X / magnitude;
	result.Y = a.Y / magnitude;
	result.Z = a.Z / magnitude;
	return(result);
}

inline v3
Math_NormalizedV3(v3 a)
{
	v3 result = Math_NormalizedV3(a, Math_MagnitudeV3(a));
	return(result);
}


#include "../Engine_Memory.cpp"
#include "AssetProcessor_Mesh.cpp"

void
WriteEngineBitmap(raw_file file, char* inFileName)
{
	bitmap_header* header = (bitmap_header*)file.Contents;
	loadable_bitmap result = {};
	result.Height = -header->Height;
	result.Width = header->Width;
	result.Height = header->Height;
	s32 bytesPerPixel = header->BitsPerPixel / 8;
	u32 redChannel = header->RedMask;
	u32 greenChannel = header->GreenMask;
	u32 blueChannel = header->BlueMask;
	u32 alphaChannel = 0xFF000000;
	
	// NOTE(Stephen): Gimp stores bitmaps as ARGB, need to swizzle...
	if(bytesPerPixel == 4)
	{
		u32* startPixel = (u32*)((u8*)header + header->PixelOffset);
		
		for(s32 i = 0; i < (result.Width * result.Height); ++i)
		{
			*startPixel = (((redChannel & *startPixel)   << 8) |
						   ((greenChannel & *startPixel) << 8) |
						   ((blueChannel & *startPixel)  << 8) |
						   ((alphaChannel & *startPixel) >> 24));
			++startPixel;
		}
	}
	
	char path[128] = {};
	char fileName[128] = {};
	ChangeFileExtension(fileName, inFileName, ENGINE_BITMAP_EXT);
	GetLocalFileNamePath(ASSET_IMAGES_DIRECTORY, path, fileName);
	
	WriteFile(path, sizeof(loadable_bitmap), &result, "w");
	WriteFile(path, (result.Width * bytesPerPixel * result.Height), 
			  (u8*)header + header->PixelOffset, "a");
	
}

void
ProcessAllBitmaps(working_directory workingDirectory)
{
	char directoryBuffer[128] = {};
	char filePathBuffer[128] = {};
	
	GetLocalDirectoryPath(directoryBuffer, RAW_IMAGES_FOLDER, workingDirectory);
	
	DIR* bitmapDirectory = opendir(directoryBuffer);
	if(bitmapDirectory)
	{
		struct dirent* directoryEntity;
		while((directoryEntity = readdir(bitmapDirectory)) != 0)
		{
			if(directoryEntity->d_name[0] != '.')
			{
				GetLocalFileNamePath(directoryBuffer, filePathBuffer, directoryEntity->d_name);
				
				raw_file file = ReadBinaryFile(filePathBuffer);
				WriteEngineBitmap(file, directoryEntity->d_name);
				FreeRawFile(&file);
				
			}
		}
		closedir(bitmapDirectory);
	}
}

int
main(int argc, char** args)
{
	if(argc == 2)
	{
		working_directory workingDirectory = BuildProgramFilePath(args[1]);
		ProcessAllBitmaps(workingDirectory);
		ProcessAllWaveFrontOBJs(workingDirectory);
	}
	
	return(0);
}