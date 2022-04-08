
#include <stdio.h>

#include "Engine_Platform.h"
#include "Engine_Memory.h"
#include "Engine_Math.h"
#include "Engine_Intrinsics.h"
#include "Engine_Random.h"
#include "Engine_Renderer.h"
#include "Engine.h"

#include "Engine_Memory.cpp"
#include "Engine_Math.cpp"
#include "Engine_Intrinsics.cpp"
#include "Engine_Renderer.cpp"
#include "Engine_FragmentShader.cpp"
#include "Engine_VertexShader.cpp"

void
WriteBitmap(debug_file_write* writeFile, char* fileName,
			render_bitmap* bitmap, s32 bitsPerPixel,
			u32 redMask, u32 greenMask, u32 blueMask)
{
	s32 bytesPerPixel = bitsPerPixel / 8;
	bitmap_header header = {};
	header.ID = 0x4D42;
	header.PixelOffset = sizeof(bitmap_header);
	header.HeaderSize = sizeof(bitmap_header) - Bytes(14);
	header.Planes = 1;
	header.Compression = 3;
	header.PixelPerMeterX = 2835;
	header.PixelPerMeterY = 2835;
	header.RedMask   = redMask;
	header.GreenMask = greenMask;
	header.BlueMask  = blueMask;
	header.BitsPerPixel = bitsPerPixel;
	header.FileSize = sizeof(bitmap_header) + bytesPerPixel * (bitmap->Width *
															   bitmap->Height);
	header.Width = bitmap->Width;
	header.Height = -bitmap->Height;
	header.ImageSize = bytesPerPixel * bitmap->Width * bitmap->Height;
	
	
	writeFile(fileName, sizeof(bitmap_header), &header, "w");
	writeFile(fileName, header.ImageSize, bitmap->Pixels, "a");
}

void
WriteEngineBuffers(debug_file_write* writeFile, 
				   engine_buffer* buffers,
				   render_bitmap* shadowMap = 0, 
				   memory_block* temp = 0)
{
	WriteBitmap(writeFile, "../misc/colorBuffer.bmp",
				&buffers->Color, 32,
				0xFF000000, 0x00FF0000, 0x0000FF00);
	
	WriteBitmap(writeFile, "../misc/colorBuffer.bmp",
				&buffers->Color, 32,
				0xFF000000, 0x00FF0000, 0x0000FF00);
	
	DepthToColor(&buffers->Color, &buffers->Depth);
	WriteBitmap(writeFile, "../misc/depthBuffer.bmp",
				&buffers->Color, 32,
				0xFF000000, 0x00FF0000, 0x0000FF00);
	
	WriteBitmap(writeFile, "../misc/depthBuffer(f32).bmp",
				&buffers->Depth, 32,
				0xFF0000, 0x00FF00, 0x0000FF);
	
	if(shadowMap)
	{
		WriteBitmap(writeFile, "../misc/shadowMap(f32).bmp",
					shadowMap, 32,
					0xFF0000, 0x00FF00, 0x0000FF);
		if(temp)
		{
			render_bitmap shadowColor;
			s32 width = shadowMap->Width;
			s32 height = shadowMap->Height;
			shadowColor.Width = width;
			shadowColor.Height = height;
			shadowColor.Pixels = MemoryBlock_PushArray(temp, height * width, u32);
			
			DepthToColor(&shadowColor, shadowMap);
			
			WriteBitmap(writeFile, "../misc/shadowMap.bmp",
						&shadowColor, 32,
						0xFF000000, 0x00FF0000, 0x0000FF00);
			
			MemoryBlock_PushArray(temp, height * width, u32);
		}
	}
}

render_bitmap
LoadBitmap(debug_file_read* readFile, char* fileName)
{
	debug_file file = readFile(fileName);
	render_bitmap result = {};
	
	if(file.IsValid)
	{
		loadable_bitmap* bitmap = (loadable_bitmap*)file.Contents;
		result.Width = bitmap->Width;
		result.Height = bitmap->Height;
		result.Pixels = (bitmap + 1);
	}
	
	return(result);
}

engine_mesh
LoadMesh(engine_debug* debug, char* fileName, engine_mesh_m128* out = 0, memory_block* memory = 0)
{
	debug_file file = debug->ReadFile(fileName);
	engine_mesh result = {};
	
	if(file.IsValid)
	{
		loadable_mesh* mesh = (loadable_mesh*)file.Contents;
		result.VertexCount = mesh->VertexCount;
		result.Attributes = (vertex_attribute*)(mesh + 1);
		
		s32 remainder = mesh->VertexCount % 4;
		s32 vertexCount = mesh->VertexCount / 4;
		s32 m128Count = vertexCount;
		
		if(remainder)
		{
			++m128Count;
		}
		
		out->M128Count = m128Count;
		out->Remainder = remainder;
		
		vertex_attribute* source = (vertex_attribute*)(mesh + 1);
		out->Attributes = MemoryBlock_PushArray(memory, m128Count, vertex_attribute_4x, 16);
		vertex_attribute_4x* destination = out->Attributes;
		
		s32 count = 0;
		
		for(s32 i = 0; i < vertexCount; ++i)
		{
			for(s32 j = 0; j < 4; ++j)
			{
				vertex_attribute* s =  source++;
				destination->Vertices.X[j] = s->Vertex.X;
				destination->Vertices.Y[j] = s->Vertex.Y;
				destination->Vertices.Z[j] = s->Vertex.Z;
				destination->Vertices.W[j] = 1;
				++count;
				
				destination->Normals.X[j] = s->Normal.X;
				destination->Normals.Y[j] = s->Normal.Y;
				destination->Normals.Z[j] = s->Normal.Z;
				destination->Normals.W[j] = 0;
				
				destination->UVs.X[j] = s->UV.X;
				destination->UVs.Y[j] = s->UV.Y;
				
				destination->Tangents.X[j] = s->Tangent.X;
				destination->Tangents.Y[j] = s->Tangent.Y;
				destination->Tangents.Z[j] = s->Tangent.Z;
				destination->Tangents.W[j] = 0;
				
				destination->Bitangents.X[j] = s->Bitangent.X;
				destination->Bitangents.Y[j] = s->Bitangent.Y;
				destination->Bitangents.Z[j] = s->Bitangent.Z;
				destination->Bitangents.W[j] = 0;
			}
			
			++destination;
		}
		
		for(s32 i = 0; i < remainder; ++i)
		{
			vertex_attribute* s =  source++;
			destination->Vertices.X[i] = s->Vertex.X;
			destination->Vertices.Y[i] = s->Vertex.Y;
			destination->Vertices.Z[i] = s->Vertex.Z;
			destination->Vertices.W[i] = 1;
			
			destination->Normals.X[i] = s->Normal.X;
			destination->Normals.Y[i] = s->Normal.Y;
			destination->Normals.Z[i] = s->Normal.Z;
			
			destination->UVs.X[i] = s->UV.X;
			destination->UVs.Y[i] = s->UV.Y;
			
			destination->Tangents.X[i] = s->Tangent.X;
			destination->Tangents.Y[i] = s->Tangent.Y;
			destination->Tangents.Z[i] = s->Tangent.Z;
			
			destination->Bitangents.X[i] = s->Bitangent.X;
			destination->Bitangents.Y[i] = s->Bitangent.Y;
			destination->Bitangents.Z[i] = s->Bitangent.Z;
		}
	}//if(file.IsValid)
	
	return(result);
}

inline void
UpdateCamera(engine_state* state, v3 movement, v2 rotation)
{
	state->CameraPosition += movement;
	state->CameraRotation += rotation;
	
	f32 pitchSin = Math_Sin(state->CameraRotation.Y);
	f32 pitchCos = Math_Cos(state->CameraRotation.Y);
	f32 yawSin = Math_Sin(state->CameraRotation.X);
	f32 yawCos = Math_Cos(state->CameraRotation.X);
	
	v3 axisX = Math_NormalizedV3({ yawCos, 0, -yawSin });
	v3 axisZ = Math_NormalizedV3({ yawSin * pitchCos, -pitchSin, pitchCos * yawCos });
	v3 axisY = Math_CrossProductV3(axisZ, axisX);
	
	state->Camera.Row0 = V4(axisX, -Math_DotProductV3(axisX, state->CameraPosition));
	state->Camera.Row1 = V4(axisY, -Math_DotProductV3(axisY, state->CameraPosition));
	state->Camera.Row2 = V4(axisZ, -Math_DotProductV3(axisZ, state->CameraPosition));
	state->Camera.Row3 = {0, 0, 0, 1};
}

inline void
ConstructOrthographicMatrix(m4x4* orthographic,
							f32 nearPlane, f32 farPlane,
							s32 screenLeft, s32 screenRight,
							s32 screenTop, s32 screenBottom)
{
	f32 screenWidth = (screenRight - screenLeft);
    f32 screenHeight = (screenBottom - screenTop);
	
	//f32 windowAspectRatio = (f32)screenWidth / (f32)screenHeight;
	
	f32 a = 2.0f / screenWidth;
	f32 b = -2.0f / screenHeight;
	f32 c =  -farPlane / (farPlane - nearPlane);
	f32 d =  (-farPlane * nearPlane) / (farPlane - nearPlane);
	
	orthographic->Row0 = { a,  0,  0,  0  };
	orthographic->Row1 = { 0,  b,  0,  0  };
	orthographic->Row2 = { 0,  0,  c,  d  };
	orthographic->Row3 = { 0,  0,  0,  1  };
	
	m4x4 offset = 
	{
		0.5f, 0,    0,    0.5f,  
		0,    0.5f, 0,    0.5f,  
		0,    0,    0.5f, 0.5f,  
		0,    0,    0,    1
	};
	
	*orthographic = Math_MultiplyM4x4(&offset, orthographic);
}

inline void
ConstructPerspectiveMatrix(m4x4* perspective,
						   f32 fieldOfView, f32 nearPlane, f32 farPlane,
						   s32 screenLeft, s32 screenRight,
						   s32 screenTop, s32 screenBottom)
{
	f32 screenWidth = (screenRight - screenLeft);
    f32 screenHeight = (screenBottom - screenTop);
	
	f32 windowAspectRatio = (f32)screenWidth / (f32)screenHeight;
	f32 scale = (nearPlane * Math_Tan(DegreesToRadians(fieldOfView * 0.5f)));
	f32 viewRight = windowAspectRatio * scale;
	f32 viewLeft = -viewRight;
	f32 viewTop = scale;
	f32 viewBottom = -viewTop;
	
	f32 a =  (2.0f * nearPlane) / (viewRight - viewLeft);
	f32 b = -(2.0f * nearPlane) / (viewTop - viewBottom);
	f32 c =  -farPlane / (farPlane - nearPlane);
	f32 d =  (-farPlane * nearPlane) / (farPlane - nearPlane);
	
	perspective->Row0 = { a,  0,  0,  0  };
	perspective->Row1 = { 0,  b,  0,  0  };
	perspective->Row2 = { 0,  0,  c,  d  };
	perspective->Row3 = { 0,  0, -1,  0  };
	
	m4x4 offset = 
	{
		0.5f, 0,    0,    0.5f,  
		0,    0.5f, 0,    0.5f,  
		0,    0,    0.5f, 0.5f,  0, 0, 0, 1};
	
	*perspective = Math_MultiplyM4x4(&offset, perspective);
}

inline void
InitializeCamera(engine_state* state,
				 f32 fieldOfView, f32 nearPlane, f32 farPlane,
				 s32 screenLeft, s32 screenRight,
				 s32 screenTop, s32 screenBottom,
				 v3 position, v2 rotation)
{
	ConstructPerspectiveMatrix(&state->Perspective,
							   fieldOfView, nearPlane, farPlane,
							   screenLeft, screenRight,
							   screenTop, screenBottom);
	UpdateCamera(state, position, rotation);
}

extern "C"
ENGINE_UPDATE(EngineUpdate)
{
	engine_state* state = (engine_state*)memory->Persistent;
	platform_keyboard* keyboard = &input->Keyboard;
	
	if(memory->IsInitialized == FALSE)
	{
		MemoryBlock_Initialize(&state->WorldMemory, 
							   memory->PersistentSize - sizeof(engine_state),
							   (u8*)memory->Persistent + sizeof(engine_state));
		
		state->TileMaterial.Diffuse = LoadBitmap(debug->ReadFile, "./assets/images/tile_diff.sr");
		state->TileMaterial.Specular = LoadBitmap(debug->ReadFile, "./assets/images/tile_spec.sr");
		state->TileMaterial.Occlusion = LoadBitmap(debug->ReadFile, "./assets/images/tile_ao.sr");
		state->TileMaterial.Normal = LoadBitmap(debug->ReadFile, "./assets/images/tile_norm.sr");
		
		state->BrickMaterial.Diffuse = LoadBitmap(debug->ReadFile, "./assets/images/brick_diff.sr");
		state->BrickMaterial.Specular = LoadBitmap(debug->ReadFile, "./assets/images/brick_spec.sr");
		state->BrickMaterial.Occlusion = LoadBitmap(debug->ReadFile, "./assets/images/brick_ao.sr");
		state->BrickMaterial.Normal = LoadBitmap(debug->ReadFile, "./assets/images/brick_norm.sr");
		
		state->CheckerBoardBitmap = LoadBitmap(debug->ReadFile, "./assets/images/checkerPattern.sr");
		
		state->Plane = LoadMesh(debug, "./assets/models/plane.sr", 
								&state->Plane_M128, &state->WorldMemory);
		
		state->Sphere = LoadMesh(debug, "./assets/models/sphere.sr",
								 &state->Sphere_M128, &state->WorldMemory);
		state->Cube = LoadMesh(debug, "./assets/models/cube.sr",
							   &state->Cube_M128, &state->WorldMemory);
		
		s32 fragmentGroupSize = memory->TransientSize / ArrayLength(state->FragmentGroups);
		
		for(s32 i = 0; i < ArrayLength(state->FragmentGroups); ++i)
		{
			FragmentGroup_Initialize(state->FragmentGroups + i, 
									 fragmentGroupSize, 
									 (u8*)memory->Transient + fragmentGroupSize * i);
		}
		
		s32 width = 1024;
		s32 height = 1024;
		state->ShadowMap.Width = width;
		state->ShadowMap.Height = height;
		state->ShadowMap.Pixels = MemoryBlock_PushArray(&state->WorldMemory,
														(width * height * sizeof(f32)), f32);
		
		ConstructOrthographicMatrix(&state->ShadowMapProjection, 0.1f, 100.0f,
									0, 20,
									0, 20);
		
		InitializeCamera(state, 60, 0.1f, 100.0f,
						 0, buffer->Color.Width,
						 0, buffer->Color.Height,
						 {0, 0, 10}, {});
		
		memory->IsInitialized = TRUE;
	}
	
	v3 movement = {};
	v2 rotation = {};
	
	if(keyboard->A.IsDown) { movement -= state->Camera.Row0.XYZ; }
	if(keyboard->D.IsDown) { movement += state->Camera.Row0.XYZ; }
	if(keyboard->E.IsDown) { movement += state->Camera.Row1.XYZ; }
	if(keyboard->Q.IsDown) { movement -= state->Camera.Row1.XYZ; }
	if(keyboard->S.IsDown) { movement += state->Camera.Row2.XYZ; }
	if(keyboard->W.IsDown) { movement -= state->Camera.Row2.XYZ; }
	
	if(keyboard->Left.IsDown)  { rotation.X =  1; }
	if(keyboard->Right.IsDown) { rotation.X = -1; }
	if(keyboard->Down.IsDown)  { rotation.Y = -1; }
	if(keyboard->Up.IsDown)    { rotation.Y =  1; }
	
	if(keyboard->F1.IsUp) 
	{ WriteEngineBuffers(debug->WriteFile, buffer, &state->ShadowMap, &state->WorldMemory); }
	
	UpdateCamera(state, movement * 6 * time->Delta, rotation * 2 * time->Delta);
	
	//ClearBitmap(&buffer->Color, {0.1f, 0.1f, 0.1f, 1});
	ClearBitmap(&buffer->Color, {});
	ClearDepthBuffer(&buffer->Depth, 0.0f);
	ClearDepthBuffer(&state->ShadowMap, 0.0f);
	
	local_persist f32 elapsedTime = 0;
	elapsedTime += time->Delta;
	
	v4 color = {0.7f, 0.3f, 0.3f, 1.0f};
	color = {1, 1, 1, 0};
	
#define MeshCount 2
	engine_mesh mesh[MeshCount];
	mesh[0] = state->Sphere;
	mesh[1] = state->Plane;
	
	engine_mesh_m128 mesh128[MeshCount];
	mesh128[0] = state->Sphere_M128;
	mesh128[1] = state->Plane_M128;
	
	f32 c = Math_Cos(elapsedTime * 0);
	f32 s = Math_Sin(elapsedTime * 0);
	m4x4 worldMatrix[2];
	worldMatrix[0].Row0 = {c * 0.75f,  0, -s * 0.75f,  1};
	worldMatrix[0].Row1 = {0,  1 * 0.75f,  0, 0};
	worldMatrix[0].Row2 = {s * 0.75f,  0,  c * 0.75f, 1};
	worldMatrix[0].Row3 = {0,  0,  0, 1};
	
	c = Math_Cos(elapsedTime * 0);
	s = Math_Sin(elapsedTime * 0);
	
	worldMatrix[1].Row0 = {c * 3,  0, -s,  0};
	worldMatrix[1].Row1 = {0,  1,  0,     -2};
	worldMatrix[1].Row2 = {s,  0,  c * 3,  0};
	worldMatrix[1].Row3 = {0,  0,  0,      1};
	
	state->BrickMaterial.SpecularIntensity = 0.1f;
	state->BrickMaterial.SpecularShininess = 0.5f;
	
	state->BrickMaterial.Color = {0.7f, 0.6f, 0.7f, 0.25f};
	
	state->TileMaterial.SpecularIntensity = 2.0f;
	state->TileMaterial.SpecularShininess = 100.5f;
	state->TileMaterial.Color = {0.7f, 0.7f, 0.3f, 0.0f};
	
	render_matrial materials[2];
	materials[0] = state->BrickMaterial;
	materials[1] = state->TileMaterial;
	
	cameraPosition = state->CameraPosition;
	
	v3 lightPosition = V3(10, 10, 10);
	M4x4_LookAtViewMatrix(&state->ShadowMapMatrix, lightPosition, {}, {0, 1, 0});
	
	m4x4 viewProjection = Math_MultiplyM4x4(&state->Perspective, &state->Camera);
	m4x4 shadowVP = Math_MultiplyM4x4(&state->ShadowMapProjection, &state->ShadowMapMatrix);
	
	for(s32 i = 0; i < MeshCount; ++i)
	{
		m4x4 shadowMVP = Math_MultiplyM4x4(&shadowVP, worldMatrix + i);
		m4x4 mvpMatrix = Math_MultiplyM4x4(&viewProjection, worldMatrix + i);
		
		FragmentGroup_Reset(state->FragmentGroups + i);
		state->FragmentGroups[i].Material = materials[i];
		
#if 1
		VertexShader_MainPass_4X(state->FragmentGroups + i,  mesh128 + i, 
								 worldMatrix + i, &mvpMatrix, &shadowMVP);
#else
		VertexShader_MainPass(state->FragmentGroups + i,  mesh + i, 
							  worldMatrix + i, &mvpMatrix, &shadowMVP);
#endif
		
	}
	
	for(s32 i = 0; i < MeshCount; ++i)
	{
		fragment_group* group = state->FragmentGroups + i;
		for(s32 i = 0; i < group->Count; i += 3)
		{
			render_fragment* fragment0 = group->Base + i + 0;
			render_fragment* fragment1 = group->Base + i + 1;
			render_fragment* fragment2 = group->Base + i + 2;
			FragmentShader_ComputeDepth(&state->ShadowMap, fragment0->ShadowMapCoord, 
										fragment1->ShadowMapCoord, fragment2->ShadowMapCoord);
			FragmentShader_ComputeDepth(&buffer->Depth, fragment0->Position, 
										fragment1->Position, fragment2->Position);
		}
	}
	
	
	for(s32 i = 0; i < MeshCount; ++i)
	{
		fragment_group* group = state->FragmentGroups + i;
		
		for(s32 i = 0; i < group->Count; i += 3)
		{
			render_fragment* fragment0 = group->Base + i + 0;
			render_fragment* fragment1 = group->Base + i + 1;
			render_fragment* fragment2 = group->Base + i + 2;
			FragmentShader_MainPass(buffer, &state->ShadowMap,
									&group->Material, fragment0, 
									fragment1, fragment2);
			
		}
	}
	
	
	
	if(0)
	{
		DepthToColor(&buffer->Color, &buffer->Depth);
	}
	
#if 0
	render_bitmap colorShadowMap;
	colorShadowMap.Width = state->ShadowMap.Width;
	colorShadowMap.Height = state->ShadowMap.Height;
	colorShadowMap.Pixels = MemoryBlock_PushArray(&state->WorldMemory,
												  colorShadowMap.Width * colorShadowMap.Height,
												  u32);
	
	DepthToColor(&colorShadowMap, &state->ShadowMap);
	
	f32 w = 256;
	f32 h = 256;
	
	DrawBitmap(&buffer->Color, &colorShadowMap, 
			   {buffer->Color.Width - w - 1, 0}, {w, h});
	MemoryBlock_PopArray(&state->WorldMemory, 
						 colorShadowMap.Width * colorShadowMap.Height,
						 u32);
#endif
	
#if 1
	
	local_persist f32 t = 0;
	t += time->Delta;
	
	if(t > 1)
	{
		t = 0;
		printf("FPS: %f\n", time->FPS);
		
		for(s32 i = 0; i < ArrayLength(GlobalDebugTable.Entry); ++i)
		{
			if(GlobalDebugTable.Entry[i].CalledCount)
			{
				u64 count = GlobalDebugTable.Entry[i].CalledCount;
				u64 totalCycles = GlobalDebugTable.Entry[i].Cycles;
				u64 average = totalCycles / count;
				printf("%s(%lu): Total Cycles: %lu | cph %lu\n", 
					   GlobalDebugTable.Entry[i].Name, 
					   count, 
					   totalCycles, average);
				
			}
		}
	}
	
	GlobalDebugTable = {};
#endif
	
}