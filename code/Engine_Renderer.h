/* date = March 22nd 2022 1:36 pm */

#ifndef _ENGINE__RENDERER_H
#define _ENGINE__RENDERER_H

struct vertex_attribute
{
	union __attribute((aligned(16)))
	{
		f32 E[14];
		struct
		{
			v3 Vertex;
			v3 Normal;
			v2 UV;
			v3 Tangent;
			v3 Bitangent;
		};
	};
};

union engine_mesh
{
	struct __attribute((aligned(16)))
	{
		s32 VertexCount;
		vertex_attribute* Attributes;
	};
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

struct render_matrial
{
	f32 SpecularIntensity;
	f32 SpecularShininess;
	render_bitmap Diffuse;
	render_bitmap Normal;
	render_bitmap Specular;
	render_bitmap Occlusion;
	v4 Color;
};

struct render_fragment
{
	v4 Position;
	v4 Normal;
	v2 UV;
	v4 Tangent;
	v4 Bitangent;
	v4 ShadowMapCoord;
};

struct fragment_group
{
	render_matrial Material;
	s32 Count;
	s32 Used;
	s32 Size;
	render_fragment* Base;
};

inline void
FragmentGroup_Initialize(fragment_group* group, s32 size, void* base)
{
	group->Count = 0;
	group->Used = 0;
	group->Size = size;
	group->Base = (render_fragment*)base;
}

inline void
FragmentGroup_Reset(fragment_group* group)
{
	group->Used = 0;
	group->Count = 0;
}

inline render_fragment*
FragmentGroup_PushFragment(fragment_group* group, s32 count)
{
	s32 size = count * sizeof(render_fragment);
	Assert((group->Used + size) <= group->Size);
	render_fragment* result = (group->Base + group->Count);
	group->Used += size;
	group->Count += count;
	return(result);
}

inline render_fragment*
FragmentGroup_PopFragment(fragment_group* group, s32 count)
{
	s32 size = count * sizeof(render_fragment);
	Assert((group->Used - size) >= 0);
	group->Used -= size;
	group->Count -= count;
	render_fragment* result = (group->Base + group->Count);
	return(result);
}

#endif //_ENGINE__RENDERER_H
