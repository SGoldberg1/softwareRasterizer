/* date = March 15th 2022 9:29 am */

#ifndef _ENGINE_H
#define _ENGINE_H

struct engine_camera
{
	v3 AxisX;
	v3 AxisY;
	v3 AxisZ;
	v2 Rotation;
	world_position Position;
};

struct engine_state
{
	memory_block WorldMemory;
	
	engine_camera Camera;
	
	render_group RenderGroup;
	engine_mesh Cube;
	engine_mesh Plane;
	engine_mesh Rectangle;
	
	
	world_map* WorldMap;
	
	s32 HighEntityCount;
	s32 LowEntityCount;
	world_low_entity LowEntities[4096 * 2 * 2];
	world_high_entity HighEntities[256 * 2 * 2 * 2 * 2];
};

#endif //_ENGINE_H
