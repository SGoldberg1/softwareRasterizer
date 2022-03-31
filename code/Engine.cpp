
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

void
ProcessVertexShader(engine_buffer* buffer, render_matrial* material,
					engine_mesh* mesh, m4x4* worldMatrix, 
					m4x4* cameraMatrix, m4x4* perspectiveMatrix)
{
	
	m4x4 viewMatrix = Math_MultiplyM4x4(cameraMatrix, worldMatrix);
	m4x4 clipMatrix = Math_MultiplyM4x4(perspectiveMatrix, &viewMatrix); 
	
	for(s32 index = 0; index < mesh->VertexCount; index += 3)
	{
		vertex_attribute* attributes[3];
		attributes[0] = mesh->Attributes + index + 0;
		attributes[1] = mesh->Attributes + index + 1;
		attributes[2] = mesh->Attributes + index + 2;
		
		v3 worldPosition[3];
		worldPosition[0] = Math_MultiplyM4x4(worldMatrix, attributes[0]->Vertex).XYZ;
		worldPosition[1] = Math_MultiplyM4x4(worldMatrix, attributes[1]->Vertex).XYZ;
		worldPosition[2] = Math_MultiplyM4x4(worldMatrix, attributes[2]->Vertex).XYZ;
		
		v4 clip0 = Math_MultiplyM4x4(&clipMatrix, attributes[0]->Vertex);
		v4 clip1 = Math_MultiplyM4x4(&clipMatrix, attributes[1]->Vertex);
		v4 clip2 = Math_MultiplyM4x4(&clipMatrix, attributes[2]->Vertex);
		
		clip0.XYZ *= 1.0f / clip0.W;
		clip1.XYZ *= 1.0f / clip1.W;
		clip2.XYZ *= 1.0f / clip2.W;
		
		clip0.X = (clip0.X + 1) * 0.5f * buffer->Color.Width;
		clip0.Y = (clip0.Y + 1) * 0.5f * buffer->Color.Height;
		
		clip1.X = (clip1.X + 1) * 0.5f * buffer->Color.Width;
		clip1.Y = (clip1.Y + 1) * 0.5f * buffer->Color.Height;
		
		clip2.X = (clip2.X + 1) * 0.5f * buffer->Color.Width;
		clip2.Y = (clip2.Y + 1) * 0.5f * buffer->Color.Height;
		
		if(Math_SignedAreaOfTriangle(clip0.XY, clip1.XY, clip2.XY) >= 0.0f)
		{
			f32 cameraZ0 = clip0.W;
			f32 cameraZ1 = clip1.W;
			f32 cameraZ2 = clip2.W;
			
			ComputeDepthForTriangle(&buffer->Depth, clip0.XY, clip1.XY, clip2.XY,
									cameraZ0, cameraZ1, cameraZ2);
			
			v2 uv0 = attributes[0]->UV;
			v2 uv1 = attributes[1]->UV;
			v2 uv2 = attributes[2]->UV;
			
			v3 fragNormal0 = Math_MultiplyM4x4(worldMatrix, attributes[0]->Normal, 0).XYZ;
			v3 fragNormal1 = Math_MultiplyM4x4(worldMatrix, attributes[1]->Normal, 0).XYZ;
			v3 fragNormal2 = Math_MultiplyM4x4(worldMatrix, attributes[2]->Normal, 0).XYZ;
			
			v3 normals[3];
			normals[0] = fragNormal0;
			normals[1] = fragNormal1;
			normals[2] = fragNormal2;
			
			v4 tangent[3];
			v3 bitangent[3];
			for(s32 i = 0; i < ArrayLength(tangent); ++i)
			{
				tangent[i] = mesh->Tangents[index + i];
				tangent[i] = Math_MultiplyM4x4(worldMatrix, tangent[i].XYZ, 0);
				bitangent[i] = Math_CrossProductV3(normals[i], tangent[i].XYZ) * tangent[i].W;
			}
			
			DrawTriangle(&buffer->Color, &buffer->Depth, 
						 material,
						 clip0.XY, clip1.XY, clip2.XY, 
						 uv0, uv1, uv2,
						 cameraZ0, cameraZ1, cameraZ2,
						 worldPosition[0], worldPosition[1], worldPosition[2],
						 fragNormal0, fragNormal1, fragNormal2,
						 tangent[0].XYZ, tangent[1].XYZ, tangent[2].XYZ, 
						 bitangent[0], bitangent[1], bitangent[2]);
			
#if 0
			// NOTE(Stephen): Display Tangents and bitangents
			f32 tangentScale = 0.5f;
			v4 t0 = Math_MultiplyM4x4(&clipMatrix, tangent0.XYZ * tangentScale + 
									  attribute0->Vertex);
			v4 b0 = Math_MultiplyM4x4(&clipMatrix, bitangent0 * tangentScale + 
									  attribute0->Vertex);
			
			v4 t1 = Math_MultiplyM4x4(&clipMatrix, tangent1.XYZ * tangentScale + 
									  attribute1->Vertex);
			v4 b1 = Math_MultiplyM4x4(&clipMatrix, bitangent1 * tangentScale + 
									  attribute1->Vertex);
			
			v4 t2 = Math_MultiplyM4x4(&clipMatrix, tangent2.XYZ * tangentScale + 
									  attribute2->Vertex);
			v4 b2 = Math_MultiplyM4x4(&clipMatrix, bitangent2 * tangentScale + 
									  attribute2->Vertex);
			
			t0.XYZ *= 1.0f / t0.W; 
			b0.XYZ *= 1.0f / b0.W; 
			
			t1.XYZ *= 1.0f / t1.W; 
			b1.XYZ *= 1.0f / b1.W; 
			
			t2.XYZ *= 1.0f / t2.W; 
			b2.XYZ *= 1.0f / b2.W; 
			
			t0.X = (t0.X + 1) * 0.5f * buffer->Color.Width;
			t0.Y = (t0.Y + 1) * 0.5f * buffer->Color.Height;
			b0.X = (b0.X + 1) * 0.5f * buffer->Color.Width;
			b0.Y = (b0.Y + 1) * 0.5f * buffer->Color.Height;
			
			t1.X = (t1.X + 1) * 0.5f * buffer->Color.Width;
			t1.Y = (t1.Y + 1) * 0.5f * buffer->Color.Height;
			b1.X = (b1.X + 1) * 0.5f * buffer->Color.Width;
			b1.Y = (b1.Y + 1) * 0.5f * buffer->Color.Height;
			
			t2.X = (t2.X + 1) * 0.5f * buffer->Color.Width;
			t2.Y = (t2.Y + 1) * 0.5f * buffer->Color.Height;
			b2.X = (b2.X + 1) * 0.5f * buffer->Color.Width;
			b2.Y = (b2.Y + 1) * 0.5f * buffer->Color.Height;
			
			v4 tangentColor = { 0.9f, 0.9f, 0.3f, 1.0f };
			v4 bitangentColor = { 0.9f, 0.5f, 0.3f, 1.0f };
			DrawLine2D(&buffer->Color, clip0.XY, t0.XY, tangentColor);
			DrawLine2D(&buffer->Color, clip0.XY, b0.XY, bitangentColor);
			
			DrawLine2D(&buffer->Color, clip1.XY, t1.XY, tangentColor);
			DrawLine2D(&buffer->Color, clip1.XY, b1.XY, bitangentColor);
			
			DrawLine2D(&buffer->Color, clip2.XY, t2.XY, tangentColor);
			DrawLine2D(&buffer->Color, clip2.XY, b2.XY, bitangentColor);
			
#endif
			
#if 0
			// NOTE(Stephen): Display Triangle Normals
			f32 normalScale = 0.25f;
			v4 n0 = Math_MultiplyM4x4(&clipMatrix, attribute0->Normal * normalScale +
									  attribute0->Vertex);
			v4 n1 = Math_MultiplyM4x4(&clipMatrix, attribute1->Normal * normalScale +
									  attribute1->Vertex);
			v4 n2 = Math_MultiplyM4x4(&clipMatrix, attribute2->Normal * normalScale +
									  attribute2->Vertex);
			
			n0.XYZ *= 1.0f / n0.W; 
			n1.XYZ *= 1.0f / n1.W; 
			n2.XYZ *= 1.0f / n2.W;
			
			n0.X = (n0.X + 1) * 0.5f * buffer->Color.Width;
			n0.Y = (n0.Y + 1) * 0.5f * buffer->Color.Height;
			
			n1.X = (n1.X + 1) * 0.5f * buffer->Color.Width;
			n1.Y = (n1.Y + 1) * 0.5f * buffer->Color.Height;
			
			n2.X = (n2.X + 1) * 0.5f * buffer->Color.Width;
			n2.Y = (n2.Y + 1) * 0.5f * buffer->Color.Height;
			
			DrawLine2D(&buffer->Color, clip0.XY, n0.XY, {0.2f, 0.3f, 0.8f, 1});
			DrawLine2D(&buffer->Color, clip1.XY, n1.XY, {0.2f, 0.3f, 0.8f, 1});
			DrawLine2D(&buffer->Color, clip2.XY, n2.XY, {0.2f, 0.3f, 0.8f, 1});
			
#endif
			
#if 0
			// NOTE(Stephen): Display triangle edges
			DrawLine2D(&buffer->Color, clip0.XY, clip1.XY, {0, 1, 0, 1});
			DrawLine2D(&buffer->Color, clip0.XY, clip2.XY, {0, 1, 0, 1});
			DrawLine2D(&buffer->Color, clip1.XY, clip2.XY, {0, 1, 0, 1});
#endif
			
		}
		
	}
	
}

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
		result.Attributes = (vertex_attribute*)(mesh + 1);
		result.Tangents = (v4*)((u8*)mesh + mesh->TangentOffset);
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
		
		
		state->TileMaterial.Diffuse = LoadBitmap(debug->ReadFile, "./assets/images/tile_diff.sr");
		state->TileMaterial.Specular = LoadBitmap(debug->ReadFile, "./assets/images/tile_spec.sr");
		state->TileMaterial.Occlusion = LoadBitmap(debug->ReadFile, "./assets/images/tile_ao.sr");
		state->TileMaterial.Normal = LoadBitmap(debug->ReadFile, "./assets/images/tile_norm.sr");
		
		state->BrickMaterial.Diffuse = LoadBitmap(debug->ReadFile, "./assets/images/brick_diff.sr");
		state->BrickMaterial.Specular = LoadBitmap(debug->ReadFile, "./assets/images/brick_spec.sr");
		state->BrickMaterial.Occlusion = LoadBitmap(debug->ReadFile, "./assets/images/brick_ao.sr");
		state->BrickMaterial.Normal = LoadBitmap(debug->ReadFile, "./assets/images/brick_norm.sr");
		
		state->CheckerBoardBitmap = LoadBitmap(debug->ReadFile, "./assets/images/checkerPattern.sr");
		
		state->Plane = LoadMesh(debug->ReadFile, "./assets/models/plane.sr");
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
	
	ClearBitmap(&buffer->Color, {});
	//ClearBitmap(&buffer->Color, {0.1f, 0.1f, 0.1f, 0});
	ClearDepthBuffer(&buffer->Depth, 0.0f);
	
	local_persist f32 elapsedTime = 0;
	elapsedTime += time->Delta;
	elapsedTime = 0;
	
	v4 color = {0.7f, 0.3f, 0.3f, 1.0f};
	color = {1, 1, 1, 0};
	
	engine_mesh* mesh = &state->Sphere;
	
	f32 scale = 0.75f;
	
	f32 c = Math_Cos(DegreesToRadians(15));
	f32 s = Math_Sin(DegreesToRadians(15));
	m4x4 worldMatrix;
	worldMatrix.Row1 = {c * scale,  0, -s * scale,  -state->CameraPosition.X};
	worldMatrix.Row2 = {0,  1 * scale,  0,  -state->CameraPosition.Y};
	worldMatrix.Row3 = {s * scale,  0,  c * scale,  -state->CameraPosition.Z};
	worldMatrix.Row4 = {0,  0,  0,  1};
	
	state->TileMaterial.SpecularIntensisty = 10.5f;
	state->TileMaterial.SpecularShininess = 15.5f;
	
	ProcessVertexShader(buffer, &state->TileMaterial, 
						mesh, &worldMatrix, 
						&state->Camera, &state->Perspective);
	
	
	c = Math_Cos(elapsedTime * 0);
	s = Math_Sin(elapsedTime * 0);
	
	mesh = &state->Plane;
	worldMatrix.Row1 = {c * 2,  0, -s,  -state->CameraPosition.X};
	worldMatrix.Row2 = {0,  1,  0,  -1 - state->CameraPosition.Y};
	worldMatrix.Row3 = {s,  0,  c * 2,  -state->CameraPosition.Z};
	worldMatrix.Row4 = {0,  0,  0,  1};
	
	state->BrickMaterial.SpecularIntensisty = 1.0f;
	state->BrickMaterial.SpecularShininess = 1.5f;
	
	ProcessVertexShader(buffer, &state->BrickMaterial, 
						mesh, &worldMatrix, 
						&state->Camera, &state->Perspective);
	
	if(0)
	{
		DepthToColor(buffer);
	}
	
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