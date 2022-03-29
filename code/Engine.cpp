
#include <stdio.h>

#include "Engine_Platform.h"
#include "Engine_Memory.h"
#include "Engine_Math.h"
#include "Engine_Random.h"
#include "Engine_Renderer.h"
#include "Engine.h"

#include "Engine_Memory.cpp"
#include "Engine_Math.cpp"
#include "Engine_Renderer.cpp"


#if 0
engine_mesh
Mesh_CreateCube(memory_block* block)
{
	engine_mesh result;
	
	result.TriangleCount = 36;
	result.Triangles = MemoryBlock_PushArray(block, 36, s32);
	//FRONT
	result.Triangles[0 ] =  0; result.Triangles[1 ] =  1; result.Triangles[2 ] =  2; 
	result.Triangles[3 ] =  0; result.Triangles[4 ] =  2; result.Triangles[5 ] =  3;
	//RIGHT
	result.Triangles[6 ] =  1; result.Triangles[7 ] =  4; result.Triangles[8 ] =  7; 
	result.Triangles[9 ] =  1; result.Triangles[10] =  7; result.Triangles[11] =  2; 
	//LEFT
	result.Triangles[12] =  5; result.Triangles[13] =  0; result.Triangles[14] =  3; 
	result.Triangles[15] =  5; result.Triangles[16] =  3; result.Triangles[17] =  6; 
	//BACK
	result.Triangles[18] =  4; result.Triangles[19] =  5; result.Triangles[20] =  6; 
	result.Triangles[21] =  4; result.Triangles[22] =  6; result.Triangles[23] =  7; 
	//TOP
	result.Triangles[24] =  3; result.Triangles[25] =  2; result.Triangles[26] =  7; 
	result.Triangles[27] =  3; result.Triangles[28] =  7; result.Triangles[29] =  6; 
	//BOTTOM
	result.Triangles[30] =  5; result.Triangles[31] =  4; result.Triangles[32] =  1; 
	result.Triangles[33] =  5; result.Triangles[34] =  1; result.Triangles[35] =  0; 
	
	result.VertexCount = 8;
	result.Vertices = MemoryBlock_PushArray(block, result.VertexCount, v3);
	
	result.Vertices[0]  = { -0.5f, -0.5f, 0.5f };//0
	result.Vertices[1] = {  0.5f, -0.5f, 0.5f };//1
	result.Vertices[2] = {  0.5f,  0.5f, 0.5f };//2
	
	result.Vertices[3] = { -0.5f,  0.5f, 0.5f };//3
	//BACK
	result.Vertices[4] = {  0.5f, -0.5f, -0.5f };//4
	result.Vertices[5] = { -0.5f, -0.5f, -0.5f };//5
	result.Vertices[6] = { -0.5f,  0.5f, -0.5f };//6
	result.Vertices[7] = {  0.5f,  0.5f, -0.5f };//7
	
	return(result);
}

engine_mesh
Mesh_CreatePlane(memory_block* block, s32 width, s32 depth)
{
	engine_mesh result;
	
	result.TriangleCount = 6 * (width) * (depth);
	result.Triangles = MemoryBlock_PushArray(block, result.TriangleCount, s32);
	result.VertexCount = (width + 1) * (depth + 1);
	result.Vertices = MemoryBlock_PushArray(block, result.VertexCount, v3);
	
	f32 vertexX = -width * 0.5f;
	f32 vertexZ = depth * 0.5f;
	
	s32 vi = 0;
	for(s32 z = 0; z <= depth; ++z)
	{
		for(s32 x = 0; x <= width; ++x)
		{
			result.Vertices[vi++] = V3((f32)x + vertexX, 0, vertexZ - (f32)z);
		}
	}
	
	s32 ti = 0;
	vi = 0;
	for(s32 z = 0; z < depth; ++z)
	{
		for(s32 z = 0; z < depth; ++z)
		{
			result.Triangles[ti + 0] = vi;
			result.Triangles[ti + 1] = vi + 1;
			result.Triangles[ti + 2] = vi + width + 2;
			result.Triangles[ti + 3] = vi;
			result.Triangles[ti + 4] = vi + width + 2;
			result.Triangles[ti + 5] = vi + width + 1;
			
			++vi;
			ti += 6;
		}
		++vi;
	}
	
	return(result);
}

engine_mesh
Mesh_CreateRectangle(memory_block* block)
{
	engine_mesh result;
	
	result.TriangleCount = 6;
	result.Triangles = MemoryBlock_PushArray(block, 6, s32);
	//FRONT
	result.Triangles[0 ] =  0; result.Triangles[1 ] =  1; result.Triangles[2 ] =  2; 
	result.Triangles[3 ] =  0; result.Triangles[4 ] =  2; result.Triangles[5 ] =  3;
	
	result.VertexCount = 4;
	result.Vertices = MemoryBlock_PushArray(block, result.VertexCount, v3);
	result.Vertices[0] = { -0.5f, -0.5f, 0.5f };//0
	result.Vertices[1] = {  0.5f, -0.5f, 0.5f };//1
	result.Vertices[2] = {  0.5f,  0.5f, 0.5f };//2
	result.Vertices[3] = { -0.5f,  0.5f, 0.5f };//3
	
	return(result);
}

#endif

void
WriteEngineBuffers(debug_file_write* writeFile, engine_buffer* buffers)
{
	//writeFile(fileName);
	bitmap_header header = {};
	header.ID = 0x4D42;
	header.PixelOffset = sizeof(bitmap_header);
	header.HeaderSize = sizeof(bitmap_header) - Bytes(14);
	header.Planes = 1;
	header.Compression = 3;
	header.PixelPerMeterX = 2835;
	header.PixelPerMeterY = 2835;
	header.RedMask   = 0xFF000000;
	header.GreenMask = 0x00FF0000;
	header.BlueMask  = 0x0000FF00;
	header.BitsPerPixel = 32;
	header.FileSize = sizeof(bitmap_header) + Bytes(4) * (buffers->Color.Width *
														  buffers->Color.Height);
	header.Width = buffers->Color.Width;
	header.Height = -buffers->Color.Height;
	header.ImageSize = Bytes(4) * (buffers->Color.Width *
								   buffers->Color.Height);
	
	writeFile("../misc/colorBuffer.bmp", sizeof(bitmap_header), &header, "w");
	writeFile("../misc/colorBuffer.bmp", buffers->Color.Width * buffers->Color.Height * sizeof(u32), 
			  buffers->Color.Pixels, "a");
	
	DepthToColor(buffers);
	
	writeFile("../misc/depthBuffer.bmp", sizeof(bitmap_header), &header, "w");
	writeFile("../misc/depthBuffer.bmp", buffers->Color.Width * buffers->Color.Height * sizeof(u32), 
			  buffers->Color.Pixels, "a");
	
	writeFile("../misc/depthBuffer(f32).bmp", sizeof(bitmap_header), &header, "w");
	writeFile("../misc/depthBuffer(f32).bmp", buffers->Color.Width * buffers->Color.Height * sizeof(f32), 
			  buffers->Depth.Pixels, "a");
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
LoadMesh(debug_file_read* readFile, char* fileName)
{
	debug_file file = readFile(fileName);
	engine_mesh result = {};
	
	if(file.IsValid)
	{
		loadable_mesh* mesh = (loadable_mesh*)file.Contents;
		result.VertexCount = mesh->VertexCount;
		result.TriangleCount = mesh->TriangleCount;
		result.UVCount = mesh->UVCount;
		result.NormalCount = mesh->NormalCount;
		result.Vertices = (v3*)(mesh + 1);
		result.Triangles = (triangle_index*)((u8*)mesh + mesh->TriangleOffset);
		result.Normals = (v3*)((u8*)mesh + mesh->NormalOffset);
		result.UVs = (v2*)((u8*)mesh + mesh->UVOffset);
		
#if 0		
		for(s32 i = 0; i < result.TriangleCount; ++i)
		{
			printf("%d, %d\n", 
				   result.Triangles[i].Vertex, result.Triangles[i].Normal);
		}
#endif
		
	}
	
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
	
	state->Camera.Row1 = V4(axisX, 0);
	state->Camera.Row2 = V4(axisY, 0);
	state->Camera.Row3 = V4(axisZ, 0);
	state->Camera.Row4 = {0, 0, 0, 1};
}

inline void
InitializeCamera(engine_state* state,
				 f32 fieldOfView, f32 nearPlane, f32 farPlane,
				 s32 screenLeft, s32 screenRight,
				 s32 screenTop, s32 screenBottom,
				 v3 position, v2 rotation)
{
	state->FieldOfView = fieldOfView;
	state->NearPlane = nearPlane;
	state->FarPlane = farPlane;
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
	f32 c =  farPlane / (farPlane - nearPlane);
	f32 d =  (-farPlane * nearPlane) / (farPlane - nearPlane);
	
	state->Perspective.Row1 = { a,  0,  0,  0  };
	state->Perspective.Row2 = { 0,  b,  0,  0  };
	state->Perspective.Row3 = { 0,  0,  c,  d  };
	state->Perspective.Row4 = { 0,  0, -1,  0  };
	
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
		
		//state->Cube = Mesh_CreateCube(&state->WorldMemory);
		//state->Rectangle = Mesh_CreateRectangle(&state->WorldMemory);
		//state->Plane = Mesh_CreatePlane(&state->WorldMemory, 10, 10);
		
		state->BrickDiffuse = LoadBitmap(debug->ReadFile, "./assets/images/brick_diff.sr");
		state->BrickSpecular = LoadBitmap(debug->ReadFile, "./assets/images/brick_spec.sr");
		state->BrickOcclusion = LoadBitmap(debug->ReadFile, "./assets/images/brick_ao.sr");
		state->BrickNormal = LoadBitmap(debug->ReadFile, "./assets/images/brick_norm.sr");
		
		state->CheckerBoardBitmap = LoadBitmap(debug->ReadFile, "./assets/images/checkerPattern.sr");
		
		state->Sphere = LoadMesh(debug->ReadFile, "./assets/models/sphere.sr");
		state->Cube = LoadMesh(debug->ReadFile, "./assets/models/cube.sr");
		
		InitializeCamera(state,
						 60, 0.1f, 100.0f,
						 0, buffer->Color.Width,
						 0, buffer->Color.Height,
						 {0, 0, 10}, {});
		
		memory->IsInitialized = TRUE;
	}
	
	v3 movement = {};
	v2 rotation = {};
	
	if(keyboard->A.IsDown) { movement -= state->Camera.Row1.XYZ; }
	if(keyboard->D.IsDown) { movement += state->Camera.Row1.XYZ; }
	if(keyboard->E.IsDown) { movement += state->Camera.Row2.XYZ; }
	if(keyboard->Q.IsDown) { movement -= state->Camera.Row2.XYZ; }
	if(keyboard->S.IsDown) { movement += state->Camera.Row3.XYZ; }
	if(keyboard->W.IsDown) { movement -= state->Camera.Row3.XYZ; }
	
	if(keyboard->Left.IsDown)  { rotation.X =  1; }
	if(keyboard->Right.IsDown) { rotation.X = -1; }
	if(keyboard->Down.IsDown)  { rotation.Y = -1; }
	if(keyboard->Up.IsDown)    { rotation.Y =  1; }
	
	if(keyboard->F1.IsUp) { WriteEngineBuffers(debug->WriteFile, buffer); }
	
	UpdateCamera(state, movement * 6 * time->Delta, rotation * 2 * time->Delta);
	
	ClearBitmap(&buffer->Color, {0.1f, 0.1f, 0.1f, 0});
	ClearDepthBuffer(&buffer->Depth, 0.0f);
	
	local_persist f32 elapsedTime = 0;
	elapsedTime += time->Delta;
	elapsedTime = 0;
	
	render_bitmap* diffuseTexture = &state->BrickDiffuse;
	render_bitmap* specularTexture = &state->BrickSpecular;
	render_bitmap* occlusionTexture = &state->BrickOcclusion;
	render_bitmap* normalTexture = &state->BrickNormal;
	
	engine_mesh* mesh = &state->Sphere;
	
	f32 c = Math_Cos(elapsedTime);
	f32 s = Math_Sin(elapsedTime);
	m4x4 model;
	model.Row1 = {c, -s,  0,  -state->CameraPosition.X};
	model.Row2 = {s,  c,  0,  -state->CameraPosition.Y};
	model.Row3 = {0,  0,  1,  -state->CameraPosition.Z};
	model.Row4 = {0,  0,  0,  1};
	
	m4x4 viewBasis = Math_MultiplyM4x4(&state->Camera, &model);
	m4x4 clipBasis = Math_MultiplyM4x4(&state->Perspective, &viewBasis); 
	
	for(s32 index = 0; index < mesh->TriangleCount; index += 3)
	{
		triangle_index* index0 = mesh->Triangles + index + 0;
		triangle_index* index1 = mesh->Triangles + index + 1;
		triangle_index* index2 = mesh->Triangles + index + 2;
		v4 vertex0 = Math_MultiplyM4x4(&clipBasis, mesh->Vertices[index0->Vertex]);
		v4 vertex1 = Math_MultiplyM4x4(&clipBasis, mesh->Vertices[index1->Vertex]);
		v4 vertex2 = Math_MultiplyM4x4(&clipBasis, mesh->Vertices[index2->Vertex]);
		
		vertex0.XYZ *= 1.0f / vertex0.W;
		vertex1.XYZ *= 1.0f / vertex1.W;
		vertex2.XYZ *= 1.0f / vertex2.W;
		
		vertex0.X = (vertex0.X + 1) * 0.5f * buffer->Color.Width;
		vertex0.Y = (vertex0.Y + 1) * 0.5f * buffer->Color.Height;
		
		vertex1.X = (vertex1.X + 1) * 0.5f * buffer->Color.Width;
		vertex1.Y = (vertex1.Y + 1) * 0.5f * buffer->Color.Height;
		
		vertex2.X = (vertex2.X + 1) * 0.5f * buffer->Color.Width;
		vertex2.Y = (vertex2.Y + 1) * 0.5f * buffer->Color.Height;
		
		if(Math_SignedAreaOfTriangle(vertex0.XY, vertex1.XY, vertex2.XY) >= 0.0f)
		{
			f32 cameraZ0 = vertex0.W;
			f32 cameraZ1 = vertex1.W;
			f32 cameraZ2 = vertex2.W;
			
			ComputeDepthForTriangle(&buffer->Depth, vertex0.XY, vertex1.XY, vertex2.XY,
									cameraZ0, cameraZ1, cameraZ2);
			
			v2 uv0 = mesh->UVs[index0->UV];
			v2 uv1 = mesh->UVs[index1->UV];
			v2 uv2 = mesh->UVs[index2->UV];
			
			v3 frageNormal0 = Math_MultiplyM4x4(&model, mesh->Normals[index0->Normal], 0).XYZ;
			v3 frageNormal1 = Math_MultiplyM4x4(&model, mesh->Normals[index1->Normal], 0).XYZ;
			v3 frageNormal2 = Math_MultiplyM4x4(&model, mesh->Normals[index2->Normal], 0).XYZ;
			
			v4 color = {0.7f, 0.3f, 0.3f, 1.0f};
			color = {1, 1, 1, 0};
			
			v3 fragPosition0 = Math_MultiplyM4x4(&model, mesh->Vertices[index0->Vertex]).XYZ;
			v3 fragPosition1 = Math_MultiplyM4x4(&model, mesh->Vertices[index1->Vertex]).XYZ;
			v3 fragPosition2 = Math_MultiplyM4x4(&model, mesh->Vertices[index2->Vertex]).XYZ;
			
			DrawTriangle(&buffer->Color, &buffer->Depth, 
						 diffuseTexture, specularTexture, 
						 occlusionTexture, normalTexture,
						 vertex0.XY, vertex1.XY, vertex2.XY, 
						 uv0, uv1, uv2,
						 cameraZ0, cameraZ1, cameraZ2,
						 fragPosition0, fragPosition1, fragPosition2,
						 frageNormal0, frageNormal1, frageNormal2, color);
			
#if 0
			// NOTE(Stephen): Display Triangle Normals
			f32 nScale = 0.25f;
			v4 n0 = Math_MultiplyM4x4(&clipBasis, mesh->Normals[index0->Normal] * nScale +
									  mesh->Vertices[index0->Vertex]);
			v4 n1 = Math_MultiplyM4x4(&clipBasis, mesh->Normals[index1->Normal] * nScale +
									  mesh->Vertices[index1->Vertex]);
			v4 n2 = Math_MultiplyM4x4(&clipBasis, mesh->Normals[index2->Normal] * nScale +
									  mesh->Vertices[index2->Vertex]);
			
			n0.XYZ *= 1.0f / n0.W; 
			n1.XYZ *= 1.0f / n1.W; 
			n2.XYZ *= 1.0f / n2.W;
			
			n0.X = (n0.X + 1) * 0.5f * buffer->Color.Width;
			n0.Y = (n0.Y + 1) * 0.5f * buffer->Color.Height;
			
			n1.X = (n1.X + 1) * 0.5f * buffer->Color.Width;
			n1.Y = (n1.Y + 1) * 0.5f * buffer->Color.Height;
			
			n2.X = (n2.X + 1) * 0.5f * buffer->Color.Width;
			n2.Y = (n2.Y + 1) * 0.5f * buffer->Color.Height;
			
			DrawLine2D(&buffer->Color, vertex0.XY, n0.XY, {0.2f, 0.3f, 0.8f, 1});
			DrawLine2D(&buffer->Color, vertex1.XY, n1.XY, {0.2f, 0.3f, 0.8f, 1});
			DrawLine2D(&buffer->Color, vertex2.XY, n2.XY, {0.2f, 0.3f, 0.8f, 1});
#endif
			
#if 0
			// NOTE(Stephen): Display triangle edges
			DrawLine2D(&buffer->Color, vertex0.XY, vertex1.XY, {0, 1, 0, 1});
			DrawLine2D(&buffer->Color, vertex0.XY, vertex2.XY, {0, 1, 0, 1});
			DrawLine2D(&buffer->Color, vertex1.XY, vertex2.XY, {0, 1, 0, 1});
#endif
			
		}
		
	}
	
	if(0)
	{
		DepthToColor(buffer);
	}
	
#if 0	
	u32* source = (u32*)state->TestBitmap.Pixels;
	u32*destination = (u32*)buffer->Color.Pixels;
	u32* row = destination;
	
	for(s32 y = 0; y < state->TestBitmap.Height; ++y)
	{
		destination = row;
		for(s32 x = 0; x < state->TestBitmap.Width; ++x)
		{
			*destination++ = *source++;
		}
		
		row += buffer->Color.Width;
	}
#endif
	
	
#if 1
	local_persist f32 t = 0;
	t += time->Delta;
	
	if(t > 1)
	{
		t = 0;
		printf("%f\n", time->FPS);
	}
#endif
	
}