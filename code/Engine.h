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
	render_bitmap TestBitmap;
	
};

#endif //_ENGINE_H
