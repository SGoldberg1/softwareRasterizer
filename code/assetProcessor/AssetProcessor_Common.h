/* date = March 28th 2022 11:23 am */

#ifndef _ASSET_PROCESSOR__COMMON_H
#define _ASSET_PROCESSOR__COMMON_H


struct raw_file
{
	b32 IsValid;
	s32 FileSize;
	void* Contents;
};

struct working_directory
{
	s32 Length;
	char WorkingDirectory[128];
};

raw_file
ReadBinaryFile(char* fileName)
{
	raw_file result = {};
	FILE* file = fopen(fileName, "r");
	
	if(file)
	{
		fseek(file, 0, SEEK_END);
		result.FileSize = ftell(file);
		fseek(file, 0, SEEK_SET);
		
		result.Contents = malloc(result.FileSize);
		if(result.Contents)
		{
			if(fread(result.Contents, result.FileSize, 1, file) == 1)
			{
				result.IsValid = TRUE;
			}
		}
		
		fclose(file);
	}
	
	return(result);
}

void
FreeRawFile(raw_file* file)
{
	if(file->Contents)
		free(file->Contents);
}

raw_file
ReadTextFileAndNullTerminate(char* fileName)
{
	raw_file result = {};
	FILE* file = fopen(fileName, "r");
	
	if(file)
	{
		fseek(file, 0, SEEK_END);
		result.FileSize = ftell(file);
		fseek(file, 0, SEEK_SET);
		
		result.Contents = malloc(result.FileSize + 1);
		if(result.Contents)
		{
			if(fread(result.Contents, result.FileSize, 1, file) == 1)
			{
				((u8*)result.Contents)[result.FileSize] = 0;
				result.IsValid = TRUE;
			}
		}
		
		fclose(file);
	}
	
	return(result);
}

internal_function void
WriteFile(char* fileName, s32 size, void* contents, char* mode)
{
	FILE* file = fopen(fileName, mode);
	
	if(file)
	{
		fwrite(contents, size, 1, file);
		fclose(file);
	}
	
}


inline void
GetLocalFileNamePath(char* directory, char* out, char* fileName)
{
	while(*directory) { *out++ = *directory++; }
	*out++ = '/';
	while(*fileName) { *out++ = *fileName++; }
	*out = 0;
}

inline void
GetLocalDirectoryPath(char* buffer, char* localDirectory, working_directory workingDirectory)
{
	for(s32 i = 0; i < workingDirectory.Length; ++i)
	{
		*buffer++ = workingDirectory.WorkingDirectory[i];
	}
	
	while(*localDirectory) { *buffer++ = *localDirectory++; }
}

working_directory
BuildProgramFilePath(char* directory)
{
	char* assetProcessor = "/assetProcessor/";
	working_directory result = {};
	char* destination = result.WorkingDirectory;
	while(*directory) { *destination++ = *directory++; }
	while(*assetProcessor) { *destination++ = *assetProcessor++; }
	result.Length = destination - result.WorkingDirectory;
	return(result);
}

inline void
ChangeFileExtension(char* buffer, char* fileName, char* ext)
{
	while(*fileName && *fileName != '.') { *buffer++ = *fileName++; }
	while(*ext) { *buffer++ = *ext++; }
	
}

#endif //_ASSET_PROCESSOR__COMMON_H
