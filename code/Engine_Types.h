/* date = March 27th 2022 11:36 pm */

#ifndef _ENGINE__TYPES_H
#define _ENGINE__TYPES_H

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

struct loadable_bitmap
{
	s32 Width;
	s32 Height;
};

struct loadable_mesh
{
	s32 VertexCount;
	// NOTE(Stephen): Vertex offset is right after the loadable mesh
	//s32 VertexOffset;
	s32 TangentOffset;
};

#define TRUE  (1)
#define FALSE (0)

#define internal_function static
#define local_persist     static
#define global_variable   static

#define Bytes(count) (count)
#define KiloBytes(count) (Bytes(count) * 1024)
#define MegaBytes(count) (KiloBytes(count) * 1024)

#define ArrayLength(array) (sizeof(array) / sizeof(array[0]))

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


#endif //_ENGINE__TYPES_H
