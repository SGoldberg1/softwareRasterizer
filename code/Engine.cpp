
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
		
		state->Cube = Mesh_CreateCube(&state->WorldMemory);
		state->Rectangle = Mesh_CreateRectangle(&state->WorldMemory);
		state->Plane = Mesh_CreatePlane(&state->WorldMemory, 10, 10);
		
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
	
	
	UpdateCamera(state, movement * 6 * time->Delta, rotation * 2 * time->Delta);
	
	ClearBitmap(&buffer->Color, {});
	ClearDepthBuffer(&buffer->Depth, 0.0f);
	
	engine_mesh* mesh = &state->Rectangle;
	local_persist f32 elapsedTime = 0;
	elapsedTime += time->Delta;
	f32 c = Math_Cos(elapsedTime);
	f32 s = Math_Sin(elapsedTime);
	
	m4x4 model;
	model.Row1 = {1, 0, 0, -state->CameraPosition.X};
	model.Row2 = {0, 1, 0, -state->CameraPosition.Y};
	model.Row3 = {0, 0, 1, -state->CameraPosition.Z};
	model.Row4 = {0, 0, 0, 1};
	
	m4x4 viewBasis = Math_MultiplyM4x4(&state->Camera, &model);
	m4x4 clipBasis = Math_MultiplyM4x4(&state->Perspective, &viewBasis); 
	
	
	v2 uv0 = { 0, 0 };
	v2 uv1 = { 1, 0 };
	v2 uv2 = { 1, 1 };
	v2 uv3 = { 0, 1 };
	
	u32 H = 0xFFFFFFFF;
	u32 R = 0xFF0000FF;
	local_persist u32 pixels[16 * 16] = 
	{
		H, H, H, H, H, H, H, H, 0, 0, 0, 0, 0, 0, 0, 0,
		H, H, H, H, H, H, H, H, 0, 0, 0, 0, 0, 0, 0, 0,
		H, H, H, H, H, H, H, H, 0, 0, 0, 0, 0, 0, 0, 0,
		H, H, H, H, H, H, H, H, 0, 0, 0, 0, 0, 0, 0, 0,
		H, H, H, H, H, H, H, H, 0, 0, 0, 0, 0, 0, 0, 0,
		H, H, H, H, H, H, H, H, 0, 0, 0, 0, 0, 0, 0, 0,
		H, H, H, H, H, H, H, H, 0, 0, 0, 0, 0, 0, 0, 0,
		H, H, H, H, H, H, H, H, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, H, H, H, H, H, H, H, H,
		0, 0, 0, 0, 0, 0, 0, 0, H, H, H, H, H, H, H, H,
		0, 0, 0, 0, 0, 0, 0, 0, H, H, R, R, H, H, H, H,
		0, 0, 0, 0, 0, 0, 0, 0, H, H, R, R, H, H, H, H,
		0, 0, 0, 0, 0, 0, 0, 0, H, H, H, H, H, H, H, H,
		0, 0, 0, 0, 0, 0, 0, 0, H, H, H, H, H, H, H, H,
		0, 0, 0, 0, 0, 0, 0, 0, H, H, H, H, H, H, H, H,
		0, 0, 0, 0, 0, 0, 0, 0, H, H, H, H, H, H, H, H,
	};
	render_bitmap texture;
	texture.Width = 16;
	texture.Height = 16;
	texture.Pixels = pixels;
	
	for(s32 index = 0; index < 3; index += 3)
	{
		//Transform vertices into clipspace
		v4 vertex0 = Math_MultiplyM4x4(&clipBasis, mesh->Vertices[mesh->Triangles[index + 0]]);
		v4 vertex1 = Math_MultiplyM4x4(&clipBasis, mesh->Vertices[mesh->Triangles[index + 1]]);
		v4 vertex2 = Math_MultiplyM4x4(&clipBasis, mesh->Vertices[mesh->Triangles[index + 2]]);
		
		vertex0.XYZ *= 1.0f / vertex0.W;
		vertex1.XYZ *= 1.0f / vertex1.W;
		vertex2.XYZ *= 1.0f / vertex2.W;
		
		f32 oneOverCameraZ0 = 1.0f / vertex0.W;
		f32 oneOverCameraZ1 = 1.0f / vertex1.W;
		f32 oneOverCameraZ2 = 1.0f / vertex2.W;
		
		vertex0.X = (vertex0.X + 1) * 0.5f * buffer->Color.Width;
		vertex0.Y = (vertex0.Y + 1) * 0.5f * buffer->Color.Height;
		
		vertex1.X = (vertex1.X + 1) * 0.5f * buffer->Color.Width;
		vertex1.Y = (vertex1.Y + 1) * 0.5f * buffer->Color.Height;
		
		vertex2.X = (vertex2.X + 1) * 0.5f * buffer->Color.Width;
		vertex2.Y = (vertex2.Y + 1) * 0.5f * buffer->Color.Height;
		
		if(Math_SignedAreaOfTriangle(vertex0.XY, vertex1.XY, vertex2.XY) >= 0.0f)
		{
			
			ComputeDepthForTriangle(&buffer->Depth, vertex0.XY, vertex1.XY, vertex2.XY,
									oneOverCameraZ0, oneOverCameraZ1, oneOverCameraZ2);
			
			DrawTriangle(&buffer->Color, &buffer->Depth, &texture,
						 vertex0.XY, vertex1.XY, vertex2.XY, uv0, uv1, uv2,
						 oneOverCameraZ0, oneOverCameraZ1, oneOverCameraZ2, {0.3f, 0.3f, 0.7f, 1});
			
			
#if 1
			DrawLine2D(&buffer->Color, vertex0.XY, vertex1.XY, {1, 0, 0, 1});
			DrawLine2D(&buffer->Color, vertex0.XY, vertex2.XY, {1, 0, 0, 1});
			DrawLine2D(&buffer->Color, vertex1.XY, vertex2.XY, {1, 0, 0, 1});
#endif
			
		}
		
		
		
		
		
		//clip zed coordinates
		//retriangulate
		//transform into ndc
		//clip x,y
		//triangulate
		//transform into screen space
		//render
		
	}
	
	
	
	
	if(0)
	{
		DepthToColor(buffer);
	}
	
}