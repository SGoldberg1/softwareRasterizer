/* date = March 22nd 2022 1:36 pm */

#ifndef _ENGINE__RENDERER_H
#define _ENGINE__RENDERER_H

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
	v3 Normal;
	v2 UV;
	v3 Tangent;
	v3 Bitangent;
	v4 ShadowMapCoord;
};

struct fragment_group
{
	s32 Count;
	s32 Used;
	s32 Size;
	render_fragment* Base;
};

inline void
FragmentGroup_Initialize(fragment_group* block, s32 size, void* base)
{
	block->Used = 0;
	block->Size = size;
	block->Base = (render_fragment*)base;
}

inline void
FragmentGroup_Reset(fragment_group* block)
{
	block->Used = 0;
	block->Count = 0;
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
