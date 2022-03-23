/* date = March 15th 2022 9:37 am */

#ifndef _ENGINE__RENDER_GROUP_H
#define _ENGINE__RENDER_GROUP_H


union engine_vertex
{
	f32 E[6];
	struct
	{
		v3 Position;
		v3 Normal;
	};
};

struct engine_mesh
{
	s32 TriangleCount;
	s32* Triangles;
	engine_vertex* Vertices;
};


struct render_basis
{
	v3 Position;
	v3 AxisX;
	v3 AxisY;
	v3 AxisZ;
};

enum render_group_type
{
	type_render_clear_screen,
	type_render_rectangle,
	type_render_line2d,
	type_render_mesh,
	type_render_projected_triangle
};

struct render_group_header
{
	render_group_type Type;
};

struct render_projection
{
	m4x4 Project;
	m4x4 UnProject;
};

struct render_group
{
	memory_block ClippedTriangleStack;
	
	render_projection Perspective;
	render_basis CameraBasis;
	f32 FieldOfView;
	f32 FarPlane;
	f32 NearPlane;
	
	s32 Count;
	s32 Used;
	s32 Size;
	u8* Base;
};

struct render_clear_screen
{
	v4 Color;
};

struct render_projected_triangle
{
	f32 OneOverCameraZ0;
	f32 OneOverCameraZ1;
	f32 OneOverCameraZ2;
	
	v3 NDC0;
	v3 NDC1;
	v3 NDC2;
	
	v2 Pixel0;
	v2 Pixel1;
	v2 Pixel2;
	
	v3 FragNormal;
};

struct render_mesh
{
	render_basis Basis;
	s32 TriangleCount;
	render_projected_triangle* Triangles;
	v4 Color;
};

struct render_line2d
{
	v2 Start;
	v2 End;
	v4 Color;
};

struct render_rectangle
{
	v2 Position;
	v2 Dimensions;
	v4 Color;
};

#endif //_ENGINE__RENDER_GROUP_H
