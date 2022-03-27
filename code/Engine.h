/* date = March 15th 2022 9:29 am */

#ifndef _ENGINE_H
#define _ENGINE_H



union vertex_attribute
{
	f32 E[8];
	struct
	{
		v3 Vertex;
		v3 Normal;
		v2 UV;
	};
};

struct engine_mesh
{
	s32 TriangleCount;
	s32* Triangles;
	s32 VertexCount;
	vertex_attribute* Vertices;
	v3* Normals;
};


struct engine_state
{
	memory_block WorldMemory;
	
	f32 FieldOfView;
	f32 FarPlane;
	f32 NearPlane;
	m4x4 Perspective;
	v2 CameraRotation;
	v3 CameraPosition;
	m4x4 Camera;
	
	engine_mesh Cube;
	engine_mesh Plane;
	engine_mesh Rectangle;
};

#endif //_ENGINE_H
