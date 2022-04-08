/* date = March 15th 2022 9:29 am */

#ifndef _ENGINE_H
#define _ENGINE_H

struct engine_mesh_m128
{
	s32 M128Count; // NOTE(Stephen): Count represents 4 packed
	s32 Remainder;
	vertex_attribute_4x* Attributes;
};

struct engine_state
{
	memory_block WorldMemory;
	
	m4x4 Perspective;
	v2 CameraRotation;
	v3 CameraPosition;
	m4x4 Camera;
	
	engine_mesh Cube;
	engine_mesh Plane;
	engine_mesh Sphere;
	
	engine_mesh_m128 Cube_M128;
	engine_mesh_m128 Plane_M128;
	engine_mesh_m128 Sphere_M128;
	
	render_matrial BrickMaterial;
	render_matrial TileMaterial;
	
	render_bitmap ShadowMap;
	m4x4 ShadowMapProjection;
	m4x4 ShadowMapMatrix;
	
	render_bitmap CheckerBoardBitmap;
	
	fragment_group FragmentGroups[128];
};

#endif //_ENGINE_H
