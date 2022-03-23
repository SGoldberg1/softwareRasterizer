
#include <stdio.h>

#include "Engine_Platform.h"
#include "Engine_Memory.h"
#include "Engine_Math.h"
#include "Engine_Random.h"
#include "Engine_Renderer.h"
#include "Engine_RenderGroup.h"
#include "World_Map.h"
#include "Engine.h"

#include "Engine_Memory.cpp"
#include "Engine_Math.cpp"
#include "Engine_RenderGroup.cpp"
#include "Engine_Renderer.cpp"
#include "World_Map.cpp"

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
	
	result.Vertices = MemoryBlock_PushArray(block, 8, engine_vertex);
	
	result.Vertices[0]  = { -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f };//0
	result.Vertices[1] = {  0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f };//1
	result.Vertices[2] = {  0.5f,  0.5f, 0.5f,   0.0f, 0.0f, 1.0f };//2
	
	result.Vertices[0] = { -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f };//0
	result.Vertices[1] = {  0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f };//1
	result.Vertices[2] = {  0.5f,  0.5f, 0.5f,   0.0f, 0.0f, 1.0f };//2
	
	result.Vertices[3] = { -0.5f,  0.5f, 0.5f,   0.0f, 0.0f, 1.0f };//3
	//BACK
	result.Vertices[4] = {  0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f };//4
	result.Vertices[5] = { -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f };//5
	result.Vertices[6] = { -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f };//6
	result.Vertices[7] = {  0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f };//7
	
	return(result);
}

engine_mesh
Mesh_CreatePlane(memory_block* block, s32 width, s32 depth)
{
	engine_mesh result;
	
	result.TriangleCount = 6 * (width) * (depth);
	result.Triangles = MemoryBlock_PushArray(block, result.TriangleCount, s32);
	result.Vertices = MemoryBlock_PushArray(block, (width + 1) * (depth + 1), engine_vertex);
	
	f32 vertexX = -width * 0.5f;
	f32 vertexZ = depth * 0.5f;
	
	s32 vi = 0;
	for(s32 z = 0; z <= depth; ++z)
	{
		for(s32 x = 0; x <= width; ++x)
		{
			result.Vertices[vi++].Position = V3((f32)x + vertexX, 0, vertexZ - (f32)z);
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
	
	result.Vertices = MemoryBlock_PushArray(block, 4, engine_vertex);
	result.Vertices[0].Position = { -0.5f, -0.5f, 0.5f };//0
	result.Vertices[1].Position = {  0.5f, -0.5f, 0.5f };//1
	result.Vertices[2].Position = {  0.5f,  0.5f, 0.5f };//2
	result.Vertices[3].Position = { -0.5f,  0.5f, 0.5f };//3
	
	return(result);
}

inline void
RotateCamera(engine_state* state, v2 rotation)
{
	state->Camera.Rotation += rotation;
	
	f32 pitchSin = Math_Sin(state->Camera.Rotation.Y);
	f32 pitchCos = Math_Cos(state->Camera.Rotation.Y);
	f32 yawSin = Math_Sin(state->Camera.Rotation.X);
	f32 yawCos = Math_Cos(state->Camera.Rotation.X);
	
	state->Camera.AxisX = Math_NormalizedV3({ yawCos, 0, -yawSin });
	state->Camera.AxisZ =  Math_NormalizedV3({ yawSin * pitchCos, -pitchSin, pitchCos * yawCos });
	state->Camera.AxisY = Math_CrossProductV3(state->Camera.AxisZ, state->Camera.AxisX);
}
void
SetCamera(engine_state* state, world_position newCameraPosition)
{
	world_difference cameraDifference = World_Subtract(state->WorldMap, 
													   newCameraPosition, state->Camera.Position);
	state->Camera.Position = newCameraPosition;
	
	v3 dimensions;
	dimensions.Width = state->WorldMap->ChunkSideInMeters * 2;
	dimensions.Height = state->WorldMap->ChunkSideInMeters * 2;
	dimensions.Depth = state->WorldMap->ChunkSideInMeters * 2;
	cube_bounds cameraBounds = Math_GetCubeBoundsCenter({}, dimensions);
	World_OffsetAndValidateEntityFrequency(state, cameraDifference.Delta, cameraBounds);
	
	world_position min = World_MapIntoChunkSpace(state->WorldMap, 
												 newCameraPosition, 
												 Math_GetMinCubeBounds(cameraBounds));
	
	world_position max = World_MapIntoChunkSpace(state->WorldMap, 
												 newCameraPosition, 
												 Math_GetMaxCubeBounds(cameraBounds));
	
	for(s32 z = min.Chunk.Z; z <= max.Chunk.Z; ++z)
	{
		for(s32 y = min.Chunk.Y; y <= max.Chunk.Y; ++y)
		{
			for(s32 x = min.Chunk.X; x <= max.Chunk.X; ++x)
			{
				world_chunk* chunk = World_GetChunkFrom(state->WorldMap, x, y, z);
				
				if(chunk)
				{
					for(world_entity_block* block = &chunk->FirstBlock; 
						block; block = block->Next)
					{
						for(s32 index = 0; index < block->EntityCount; ++index)
						{
							World_MakeEntityHigh(state, block->LowEntityIndex[index]);
						}
					}//for(world_entity_block* block = &chunk->EntityBlock; block; block = block->Next)
				}//if(chunk)
			}//for(s32 x = min.Chunk.X; x <= max.Chunk.X; ++x)
		}//for(s32 y = min.Chunk.Y; y <= max.Chunk.Y; ++y)
	}//for(s32 z = min.Chunk.Z; z <= max.Chunk.Z; ++z)
	
	Assert(World_ValidateEntityPairs(state));
}

inline void
InitializeCamera(engine_state* state,
				 render_group* renderGroup, 
				 s32 screenWidth, s32 screenHeight,
				 v3 position, v2 rotation)
{
	f32 farPlane = 100.0f;
	f32 nearPlane = 0.1f;
	
	f32 fieldOfView = 60.0f;
	RenderGroup_SetPerspectiveProjection(renderGroup, fieldOfView,
										 farPlane, nearPlane,
										 0, screenWidth, 0, screenHeight);
	
	RotateCamera(state, rotation);
	state->Camera.Position = {};
	world_position newPosition = World_MapIntoChunkSpace(state->WorldMap, 
														 {}, 
														 position);
	SetCamera(state, newPosition);
}

extern "C"
ENGINE_UPDATE(EngineUpdate)
{
	engine_state* state = (engine_state*)memory->Persistent;
	render_group* renderGroup = &state->RenderGroup;
	platform_keyboard* keyboard = &input->Keyboard;
	
	if(memory->IsInitialized == FALSE)
	{
		MemoryBlock_Initialize(&state->WorldMemory, 
							   memory->PersistentSize - sizeof(engine_state),
							   (u8*)memory->Persistent + sizeof(engine_state));
		
		state->Cube = Mesh_CreateCube(&state->WorldMemory);
		state->Rectangle = Mesh_CreateRectangle(&state->WorldMemory);
		state->Plane = Mesh_CreatePlane(&state->WorldMemory, 10, 10);
		
		RenderGroup_Initialize(renderGroup, 
							   memory->TransientSize,
							   memory->Transient);
		
		World_Initialize(state, 16);
		World_Build(state);
		
		InitializeCamera(state,
						 renderGroup, 
						 buffer->Color.Width, buffer->Color.Height,
						 {0, 2, 4}, {});
		
		memory->IsInitialized = TRUE;
	}
	
	v3 movement = {};
	v2 rotation = {};
	
	if(keyboard->A.IsDown) { movement -= state->Camera.AxisX; }
	if(keyboard->D.IsDown) { movement += state->Camera.AxisX; }
	if(keyboard->E.IsDown) { movement += state->Camera.AxisY; }
	if(keyboard->Q.IsDown) { movement -= state->Camera.AxisY; }
	if(keyboard->S.IsDown) { movement += state->Camera.AxisZ; }
	if(keyboard->W.IsDown) { movement -= state->Camera.AxisZ; }
	
	if(keyboard->Left.IsDown)  { rotation.X =  1; }
	if(keyboard->Right.IsDown) { rotation.X = -1; }
	if(keyboard->Down.IsDown)  { rotation.Y = -1; }
	if(keyboard->Up.IsDown)    { rotation.Y =  1; }
	
	
	f32 rotationSpeed = 2.0f;
	RotateCamera(state, rotation * time->Delta * rotationSpeed);
	
	if((movement.X != 0) || (movement.Y != 0) || (movement.Z != 0) )
	{
		f32 cameraMovementSpeed = 6.0f;
		//cameraMovementSpeed = state->WorldMap->ChunkSideInMeters;
		
		world_position newCameraPosition = World_MapIntoChunkSpace(state->WorldMap, 
																   state->Camera.Position, 
																   (movement * cameraMovementSpeed * time->Delta));
		SetCamera(state, newCameraPosition);
	}
	
	
	renderGroup->CameraBasis.AxisX = state->Camera.AxisX;
	renderGroup->CameraBasis.AxisY = state->Camera.AxisY;
	renderGroup->CameraBasis.AxisZ = state->Camera.AxisZ;
	RenderGroup_Reset(renderGroup);
	RenderGroup_PushClearScreen(renderGroup, {0.1f, 0.1f, 0.1f, 0});
	
	local_persist f32 elapsedTime = 0;
	elapsedTime += time->Delta;
	
	for(s32 index = 1;
		index < state->HighEntityCount;
		++index)
	{
		world_high_entity* entity = (state->HighEntities + index);
		world_low_entity* lowEntity = World_GetLow(state, entity->LowIndex);
		v4 color = lowEntity->Color;
		
		switch(lowEntity->Type)
		{
			case type_entity_floor:
			{
				render_basis basis;
				basis.AxisX = V3(1, 0, 0);
				basis.AxisY = V3(0, 1, 0);
				basis.AxisZ = V3(0, 0, 1);
				basis.Position = entity->Position;
				RenderGroup_PushMesh(renderGroup, &state->Plane, basis, color);
			}break;
			case type_entity_wall:
			{
				render_basis basis;
				basis.AxisX = V3(1, 0, 0);
				basis.AxisY = V3(0, 1, 0);
				basis.AxisZ = V3(0, 0, 1 );
				basis.Position = entity->Position;
				RenderGroup_PushMesh(renderGroup, &state->Cube, basis, color);
			}break;
			InvalidDefaultCase;
		}
	}
	
	Render_Buffer(buffer, renderGroup);
	
#if 0
	local_persist f32 counter = 0.0f;
	counter += time->Delta;
	if(counter > 1)
	{
		counter = 0;
		printf("%f\n", time->FPS);
	}
#endif
	
}