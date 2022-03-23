/* date = March 22nd 2022 9:56 pm */

#ifndef _WORLD__MAP_H
#define _WORLD__MAP_H

#define WORLD_INVALID_CHUNK_COORDINATE (MAX_S32)

struct world_position
{
	v3s Chunk;
	v3 Offset;
};

struct world_difference
{
	v3 Delta;
};

enum world_entity_type
{
	type_entity_null,
	type_entity_floor,
	type_entity_wall
};

struct world_low_entity
{
	world_entity_type Type;
	s32 HighIndex;
	world_position Position;
	v4 Color;
};

struct world_high_entity
{
	s32 LowIndex;
	v3 Position;
};

struct world_entity_block
{
	s32 EntityCount;
	s32 LowEntityIndex[16];
	world_entity_block* Next;
};

struct world_chunk
{
	v3s Position;
	world_entity_block FirstBlock;
	world_chunk* Next;
};

struct world_map
{
	s32 ChunkSideInMeters;
	world_chunk Chunks[256];
	world_entity_block* NextFreeEntityBlock;
};

#endif //_WORLD__MAP_H
