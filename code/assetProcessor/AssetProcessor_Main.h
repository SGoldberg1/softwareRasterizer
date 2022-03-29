/* date = March 27th 2022 11:44 pm */

#ifndef MAIN_H
#define MAIN_H


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

#define ASSET_IMAGES_DIRECTORY "../build/assets/images" 
#define ASSET_MESH_DIRECTORY "../build/assets/models" 
#define RAW_IMAGES_FOLDER "gimp_images" 
#define RAW_MESH_FOLDER "blender_models" 
#define ENGINE_BITMAP_EXT ".sr"
#define ENGINE_MESH_EXT ".sr"


#endif //MAIN_H
