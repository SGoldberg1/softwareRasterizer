
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "../Engine_Platform.h"
#include "../Engine_Memory.h"
#include "../Engine_Math.h"
#include "AssetProcessor_Common.h"
#include "AssetProcessor_Main.h"

#include "../Engine_Math.cpp"
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
	
	// NOTE(Stephen): Gimp stores bitmaps as ARGB, need to swizzle...
	u32 redChannel = header->RedMask;
	u32 greenChannel = header->GreenMask;
	u32 blueChannel = header->BlueMask;
	u32 alphaChannel = 0xFF000000;
	
	u32* startPixel = (u32*)((u8*)header + header->PixelOffset);
	
	for(s32 i = 0; i < (result.Width * result.Height); ++i)
	{
		*startPixel = (((redChannel & *startPixel)   << 8) |
					   ((greenChannel & *startPixel) << 8) |
					   ((blueChannel & *startPixel)  << 8) |
					   ((alphaChannel & *startPixel) >> 24));
		++startPixel;
	}
	
	startPixel = (u32*)((u8*)header + header->PixelOffset);
	
	char path[128] = {};
	char fileName[128] = {};
	ChangeFileExtension(fileName, inFileName, ENGINE_BITMAP_EXT);
	GetLocalFileNamePath(ASSET_IMAGES_DIRECTORY, path, fileName);
	
	WriteFile(path, sizeof(loadable_bitmap), &result, "w");
	WriteFile(path, (result.Width * result.Height * sizeof(u32)), 
			  startPixel, "a");
	
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