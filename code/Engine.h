/* date = March 15th 2022 9:29 am */

#ifndef _ENGINE_H
#define _ENGINE_H


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
	triangle_index* Triangles;
	s32 VertexCount;
	s32 UVCount;
	s32 NormalCount;
	//vertex_attribute* Vertices;
	v3* Vertices;
	v3* Normals;
	v2* UVs;
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
	engine_mesh Sphere;
	
	render_bitmap BrickDiffuse;
	render_bitmap BrickSpecular;
	render_bitmap BrickOcclusion;
	render_bitmap BrickNormal;
	
	render_bitmap CheckerBoardBitmap;
};

#endif //_ENGINE_H
