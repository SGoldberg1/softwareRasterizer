/* date = March 14th 2022 10:19 pm */

#ifndef _ENGINE__PLATFORM_H
#define _ENGINE__PLATFORM_H

#include <stdint.h>
#include <stddef.h>

typedef int64_t s64; 
typedef int32_t s32; 
typedef int16_t s16; 
typedef int8_t  s8; 

typedef uint64_t u64; 
typedef uint32_t u32; 
typedef uint16_t u16; 
typedef uint8_t  u8; 

typedef int32_t b32;

typedef float  f32;
typedef double f64;

typedef uintptr_t u32ptr;

#define TRUE  (1)
#define FALSE (0)

#define internal_function static
#define local_persist     static
#define global_variable   static

#define Bytes(count) (count)
#define KiloBytes(count) (Bytes(count) * 1024)
#define MegaBytes(count) (KiloBytes(count) * 1024)

#define ArrayLength(array) (sizeof(array) / sizeof(array[0]))

struct debug_file
{
	b32 IsValid;
	s32 FileSize;
	void* Contents;
};

typedef debug_file debug_file_read(char* fileName); 
typedef void debug_file_write(char* fileName, s32 size, void* contents, char* mode); 
typedef void debug_file_free(debug_file* file); 

struct engine_debug
{
	debug_file_free* FreeFile;
	debug_file_read* ReadFile;
	debug_file_write* WriteFile;
};

#if ENGINE_DEBUG
#define Assert(statement) if(!(statement)) { int* x = 0; *x = 0; }
#define ValidateAlignment16(address) Assert(((u32ptr)(address) & 0xF) == 0)
#define InvalidDefaultCase default: { Assert(!"Invalid Default Case") }break;
#define InvalidCodePath Assert(!"Invalid Code Path")
#else
#define InvalidDefaultCase
#define InvalidCodePath
#define Assert(statement)
#define ValidateAlignment16(address)
#endif


#define EmptyDefaultCase default:{}break


struct platform_memory
{
	b32 IsInitialized;
	u32 PersistentSize;
	u32 TransientSize;
	void* Persistent;
	void* Transient;
};

struct platform_time
{
	f32 Delta;
	f32 FPS;
	f32 EllapsedSeconds;
};

struct platform_keyboard_button
{
	b32 IsUp;
	b32 IsDown;
};

#define KEYBOARD_BUTTON_COUNT (11)
union platform_keyboard
{
	platform_keyboard_button Buttons[KEYBOARD_BUTTON_COUNT];
	struct
	{
		platform_keyboard_button A;
		platform_keyboard_button D;
		platform_keyboard_button E;
		platform_keyboard_button Q;
		platform_keyboard_button S;
		platform_keyboard_button W;
		
		platform_keyboard_button Left;
		platform_keyboard_button Right;
		platform_keyboard_button Down;
		platform_keyboard_button Up;
		
		platform_keyboard_button F1;
	};
};

struct platform_input
{
	platform_keyboard Keyboard;
};

struct render_bitmap
{
	s32 Width;
	s32 Height;
	void* Pixels;
};

union engine_buffer
{
	struct
	{
		render_bitmap Color;
		render_bitmap Depth;
	};
};

#define ENGINE_UPDATE(name) void name(engine_buffer* buffer,   \
platform_memory* memory, \
platform_input* input,   \
platform_time* time,     \
engine_debug* debug)

typedef ENGINE_UPDATE(engine_update);

extern "C"
ENGINE_UPDATE(EngineUpdate);

ENGINE_UPDATE(EngineUpdateStub) {}

#endif //_ENGINE__PLATFORM_H
