
#include "Engine_Platform.h"
#include "SDL_Header.h"

#include <sys/stat.h>
#include <stdio.h>

#if ENGINE_DEBUG
#include <stdlib.h>
#define RUNBUILD_SCRIPT system("../code/build.sh") 
#else
#define RUNBUILD_SCRIPT 
#endif

struct linux_engine_library
{
	time_t ModificationDate;
	b32 IsValid;
	void* Handle;
	engine_update* Update;
};

#pragma pack(push, 1)
struct bitmap_header
{
    s16 ID;
    s32 FileSize;
    s16 Reserved1;
    s16 Reserved2;
    s32 PixelOffset;
    s32 HeaderSize;
    s32 Width;
    s32 Height;
    s16 Planes;
    s16 BitsPerPixel;
    s32 Compression;
    s32 ImageSize;
    s32 PixelPerMeterX;
    s32 PixelPerMeterY;
    s32 ColorPalette;
    s32 ImportantColors;
	
	u32 RedMask;
    u32 GreenMask;
    u32 BlueMask;
	
};
#pragma pack(pop)

global_variable s32 GlobalWindowWidth = 1920;
global_variable s32 GlobalWindowHeight = 1080;
global_variable b32 GlobalIsRunning = TRUE;
global_variable engine_buffer GlobalEngineOffscreenBuffer = {};
global_variable u64 GlobalPerformanceFrequency = 0;


internal_function void
WriteGlobalEngineBuffers(void)
{
	FILE* colorFile = fopen("../misc/colorBuffer.bmp", "w");
	FILE* depthFile = fopen("../misc/depthBuffer.bmp", "w");
	FILE* depthF32File = fopen("../misc/depthBuffer(f32).bmp", "w");
	
	bitmap_header bitmap = {};
	bitmap.ID = 0x4D42;
	bitmap.FileSize = sizeof(bitmap_header) + Bytes(4) * (GlobalEngineOffscreenBuffer.Color.Width *
														  GlobalEngineOffscreenBuffer.Color.Height);
	bitmap.PixelOffset = sizeof(bitmap_header);
	
	bitmap.HeaderSize = sizeof(bitmap_header) - Bytes(14);
	bitmap.Width = GlobalEngineOffscreenBuffer.Color.Width;
	bitmap.Height = -GlobalEngineOffscreenBuffer.Color.Height;
	bitmap.Planes = 1;
	bitmap.BitsPerPixel = 32;
	bitmap.Compression = 3;
	bitmap.ImageSize = Bytes(4) * (GlobalEngineOffscreenBuffer.Color.Width *
								   GlobalEngineOffscreenBuffer.Color.Height);
	bitmap.PixelPerMeterX = 2835;
	bitmap.PixelPerMeterY = 2835;
	
	bitmap.RedMask   = 0xFF000000;
	bitmap.GreenMask = 0x00FF0000;
	bitmap.BlueMask  = 0x0000FF00;
	
	if(colorFile)
	{
		fwrite(&bitmap, sizeof(bitmap_header), 1, colorFile);
		fwrite(GlobalEngineOffscreenBuffer.Color.Pixels, bitmap.ImageSize, 1, colorFile);
		fclose(colorFile);
	}
	
	if(depthFile)
	{
		
		s32 size = GlobalEngineOffscreenBuffer.Color.Width * GlobalEngineOffscreenBuffer.Color.Height;
		u32* color = (u32*)GlobalEngineOffscreenBuffer.Color.Pixels;
		f32* depth = (f32*)GlobalEngineOffscreenBuffer.Depth.Pixels;
		
		for(s32 i = 0; i < size; ++i)
		{
			f32 value = *depth++;
			u32 channel = (u32)((value * 255.0f) + 0.5f);
			*color++ = (channel << 24 | channel << 16 | channel << 8 | channel << 0);
		}
		
		fwrite(&bitmap, sizeof(bitmap_header), 1, depthFile);
		fwrite(GlobalEngineOffscreenBuffer.Color.Pixels, bitmap.ImageSize, 1, depthFile);
		fclose(depthFile);
	}
	
	if(depthF32File)
	{
		fwrite(&bitmap, sizeof(bitmap_header), 1, depthF32File);
		fwrite(GlobalEngineOffscreenBuffer.Depth.Pixels, bitmap.ImageSize, 1, depthF32File);
		fclose(depthF32File);
	}
	
}

internal_function void
Linux_ResizeOffscreenBuffer(engine_buffer* buffer,
							s32 width, s32 height)
{
	if(buffer->Color.Pixels)
	{
		SDL_free(buffer->Color.Pixels);
	}
	
	if(buffer->Depth.Pixels)
	{
		SDL_free(buffer->Depth.Pixels);
	}
	
	buffer->Color.Width = width;
	buffer->Color.Height = height;
	buffer->Color.Pixels = SDL_malloc(width * sizeof(u32) * height);
	
	buffer->Depth.Width = width;
	buffer->Depth.Height = height;
	buffer->Depth.Pixels = SDL_malloc(width * sizeof(f32) * height);
	
}

internal_function inline void
Linux_UpdateButton(platform_keyboard_button* button, 
				   b32 isDown)
{
	button->IsUp = !isDown;
	button->IsDown = isDown;
}

internal_function void
Linux_ProcessEventQueue(platform_input* input)
{
	SDL_Event event;
	
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
			{
				GlobalIsRunning = FALSE;
			}break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				b32 isDown = event.key.state == SDL_PRESSED;
				b32 isUp = event.key.state == SDL_RELEASED;
				
				switch(event.key.keysym.sym)
				{
					case SDLK_a: { Linux_UpdateButton(&input->Keyboard.A, isDown); }break; 
					case SDLK_d: { Linux_UpdateButton(&input->Keyboard.D, isDown); }break; 
					case SDLK_e: { Linux_UpdateButton(&input->Keyboard.E, isDown); }break; 
					case SDLK_q: { Linux_UpdateButton(&input->Keyboard.Q, isDown); }break; 
					case SDLK_s: { Linux_UpdateButton(&input->Keyboard.S, isDown); }break; 
					case SDLK_w: { Linux_UpdateButton(&input->Keyboard.W, isDown); }break; 
					
					case SDLK_LEFT: { Linux_UpdateButton(&input->Keyboard.Left, isDown); }break; 
					case SDLK_RIGHT: { Linux_UpdateButton(&input->Keyboard.Right, isDown); }break; 
					case SDLK_UP: { Linux_UpdateButton(&input->Keyboard.Up, isDown); }break; 
					case SDLK_DOWN: { Linux_UpdateButton(&input->Keyboard.Down, isDown); }break; 
					
					case SDLK_F5: { if(isUp) { RUNBUILD_SCRIPT; } }break; 
					case SDLK_F1: { if(isUp) { WriteGlobalEngineBuffers(); } }break; 
					
					EmptyDefaultCase;
				}
				
			}break;
			EmptyDefaultCase;
		}
	}
}

internal_function inline time_t
Linux_GetFileModificationDate(char* fileName)
{
	struct stat s;
	stat(fileName, &s);
	return(s.st_ctime);
}

internal_function linux_engine_library
Linux_LoadEngineLibrary(char* fileName)
{
	linux_engine_library result = {};
	result.Handle = SDL_LoadObject(fileName);
	
	if(result.Handle)
	{
		result.Update = (engine_update*)SDL_LoadFunction(result.Handle,
														 "EngineUpdate");
		result.IsValid = result.Update != 0;
	}
	
	if(result.IsValid == FALSE)
	{
		result.Update = EngineUpdateStub;
	}
	else
	{
		result.ModificationDate = Linux_GetFileModificationDate(fileName);
	}
	
	return(result);
}

internal_function inline void
Linux_UnLoadEngineLibrary(linux_engine_library* library)
{
	if(library->Handle)
	{
		SDL_UnloadObject(library->Handle);
	}
	
	library->IsValid = FALSE;
}

internal_function inline f64
Linux_GetEllapsedSeconds(u64 start, u64 end)
{
	f32 result = ((f64)(end - start) / (f64)GlobalPerformanceFrequency);
	return(result);
}

internal_function inline f32
Linux_GetFPS(u64 start, u64 end)
{
	f32 result = (f64)GlobalPerformanceFrequency / (f64)(end - start);
	return(result);
}

int
main(int argc, char** args)
{
	SDL_Window* window = 0;
	SDL_Renderer* renderer = 0;
	SDL_Texture* texture = 0;
	
	linux_engine_library engineLibrary = {};
	
	platform_memory platformMemory = {};
	platform_time platformTime = {};
	platform_input platformInput = {};
	
	u64 lastCounter = 0;
	GlobalPerformanceFrequency = SDL_GetPerformanceFrequency();
	
	if(SDL_InitSubSystem(SDL_INIT_VIDEO) == FALSE)
	{
		window = SDL_CreateWindow("Software Rasterizer", 
								  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
								  GlobalWindowWidth, GlobalWindowHeight,
								  SDL_WINDOW_RESIZABLE);
		
		if(window)
		{
			renderer = SDL_CreateRenderer(window, -1, 
										  SDL_RENDERER_ACCELERATED |
										  SDL_RENDERER_PRESENTVSYNC);
			
			if(renderer)
			{
				s32 bufferWidth = GlobalWindowWidth / 2;
				s32 bufferHeight = GlobalWindowHeight / 2;
				
				SDL_RenderSetLogicalSize(renderer, 
										 GlobalWindowWidth, 
										 GlobalWindowHeight);
				
				texture = SDL_CreateTexture(renderer,
											SDL_PIXELFORMAT_RGBA8888,
											SDL_TEXTUREACCESS_STREAMING,
											bufferWidth, bufferHeight);
				
				if(texture)
				{
					Linux_ResizeOffscreenBuffer(&GlobalEngineOffscreenBuffer,
												bufferWidth, bufferHeight);
					
					engineLibrary = Linux_LoadEngineLibrary("./engine.so");
					
					platformMemory.PersistentSize = MegaBytes(256);
					platformMemory.TransientSize = MegaBytes(64);
					platformMemory.Persistent = SDL_malloc(platformMemory.PersistentSize);
					platformMemory.Transient = SDL_malloc(platformMemory.TransientSize);
					
					lastCounter = SDL_GetPerformanceCounter();
					
					while(GlobalIsRunning)
					{
						if(engineLibrary.ModificationDate != 
						   Linux_GetFileModificationDate("./engine.so"))
						{
							Linux_UnLoadEngineLibrary(&engineLibrary);
							engineLibrary = Linux_LoadEngineLibrary("./engine.so");
						}
						
						Linux_ProcessEventQueue(&platformInput);
						engineLibrary.Update(&GlobalEngineOffscreenBuffer,
											 &platformMemory,
											 &platformInput,
											 &platformTime);
						
						SDL_UpdateTexture(texture, 0,
										  GlobalEngineOffscreenBuffer.Color.Pixels, 
										  GlobalEngineOffscreenBuffer.Color.Width * sizeof(u32));
						SDL_RenderCopy(renderer, texture, 0, 0);
						SDL_RenderPresent(renderer);
						
						u64 currentCounter = SDL_GetPerformanceCounter();
						
						platformTime.Delta =
							Linux_GetEllapsedSeconds(lastCounter, currentCounter);
						platformTime.EllapsedSeconds = platformTime.Delta * 1000.0f;
						platformTime.FPS = Linux_GetFPS(lastCounter, 
														currentCounter);
						
						lastCounter = currentCounter;
						
					}//while(GlobalIsRunning)
					
				}//if(texture)
			}//if(renderContext)
		}//if(window)
		SDL_Quit();
	}//if(SDL_InitSubSystem(SDL_INIT_VIDEO) == FALSE)
	
	
	return(0);
}