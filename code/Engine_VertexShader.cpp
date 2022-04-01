

void
VertexShader_MainPass(engine_buffer* buffer, render_bitmap* shadowMap, render_matrial* material,
					  engine_mesh* mesh, m4x4* worldMatrix, 
					  m4x4*lightProjection, m4x4* lightMatrix, m4x4* cameraMatrix, m4x4* projectionMatrix)
{
	v2 halfBufferDim = { buffer->Color.Width * 0.5f, buffer->Color.Height * 0.5f };
	m4x4 viewMatrix = Math_MultiplyM4x4(cameraMatrix, worldMatrix);
	m4x4 clipMatrix = Math_MultiplyM4x4(projectionMatrix, &viewMatrix); 
	
	m4x4 lightViewport = { 0.5f, 0, 0, 0.5f,  0, 0.5f, 0, 0.5f,  0, 0, 0.5f, 0.5f,  0, 0, 0, 1};
	m4x4 lightTransform = Math_MultiplyM4x4(&lightViewport, lightProjection);
	lightTransform = Math_MultiplyM4x4(&lightTransform, lightMatrix);
	lightTransform = Math_MultiplyM4x4(&lightTransform, worldMatrix);
	
	for(s32 index = 0; index < mesh->VertexCount; index += 3)
	{
		
		vertex_attribute* attributes[3];
		attributes[0] = mesh->Attributes + index + 0;
		attributes[1] = mesh->Attributes + index + 1;
		attributes[2] = mesh->Attributes + index + 2;
		
		v4 clip0 = Math_MultiplyM4x4(&clipMatrix, attributes[0]->Vertex);
		v4 clip1 = Math_MultiplyM4x4(&clipMatrix, attributes[1]->Vertex);
		v4 clip2 = Math_MultiplyM4x4(&clipMatrix, attributes[2]->Vertex);
		
		clip0.XYZ *= 1.0f / clip0.W;
		clip1.XYZ *= 1.0f / clip1.W;
		clip2.XYZ *= 1.0f / clip2.W;
		
		clip0.X = (clip0.X + 1) * halfBufferDim.Width;
		clip0.Y = (clip0.Y + 1) * halfBufferDim.Height;
		
		clip1.X = (clip1.X + 1) * halfBufferDim.Width;
		clip1.Y = (clip1.Y + 1) * halfBufferDim.Height;
		
		clip2.X = (clip2.X + 1) * halfBufferDim.Width;
		clip2.Y = (clip2.Y + 1) * halfBufferDim.Height;
		
		if(Math_SignedAreaOfTriangle(clip0.XY, clip1.XY, clip2.XY) >= 0.0f)
		{
			v3 worldPosition[3];
			worldPosition[0] = Math_MultiplyM4x4(worldMatrix, attributes[0]->Vertex).XYZ;
			worldPosition[1] = Math_MultiplyM4x4(worldMatrix, attributes[1]->Vertex).XYZ;
			worldPosition[2] = Math_MultiplyM4x4(worldMatrix, attributes[2]->Vertex).XYZ;
			
			v3 normals[3];
			normals[0] = Math_MultiplyM4x4(worldMatrix, attributes[0]->Normal, 0).XYZ;
			normals[1] = Math_MultiplyM4x4(worldMatrix, attributes[1]->Normal, 0).XYZ;
			normals[2] = Math_MultiplyM4x4(worldMatrix, attributes[2]->Normal, 0).XYZ;
			
			v4 tangent[3];
			v3 bitangent[3];
			for(s32 i = 0; i < ArrayLength(tangent); ++i)
			{
				tangent[i] = mesh->Tangents[index + i];
				tangent[i] = Math_MultiplyM4x4(worldMatrix, tangent[i].XYZ, 0);
				bitangent[i] = Math_CrossProductV3(normals[i], tangent[i].XYZ) * tangent[i].W;
			}
			
			v3 lightUV0 = Math_MultiplyM4x4(&lightTransform, attributes[0]->Vertex).XYZ;
			v3 lightUV1 = Math_MultiplyM4x4(&lightTransform, attributes[1]->Vertex).XYZ;
			v3 lightUV2 = Math_MultiplyM4x4(&lightTransform, attributes[2]->Vertex).XYZ;
			
			f32 oneOverCameraZ0 = 1.0f / clip0.W;
			f32 oneOverCameraZ1 = 1.0f / clip1.W;
			f32 oneOverCameraZ2 = 1.0f / clip2.W;
			
			DrawTriangle(&buffer->Color, &buffer->Depth, shadowMap, material,
						 lightUV0, lightUV1, lightUV2,
						 clip0.XY, clip1.XY, clip2.XY,
						 attributes[0]->UV, attributes[1]->UV, attributes[2]->UV,
						 oneOverCameraZ0, oneOverCameraZ1, oneOverCameraZ2,
						 worldPosition[0], worldPosition[1], worldPosition[2],
						 normals[0], normals[1], normals[2],
						 tangent[0].XYZ, tangent[1].XYZ, tangent[2].XYZ, 
						 bitangent[0], bitangent[1], bitangent[2]);
		}
	}
}

void
VertexShader_ShadowMap(render_bitmap* depthMap, engine_mesh* mesh, m4x4* worldMatrix, 
					   m4x4* cameraMatrix, m4x4* projectionMatrix)
{
	v2 halfBufferDim = { depthMap->Width * 0.5f, depthMap->Height * 0.5f };
	m4x4 viewMatrix = Math_MultiplyM4x4(cameraMatrix, worldMatrix);
	m4x4 clipMatrix = Math_MultiplyM4x4(projectionMatrix, &viewMatrix); 
	
	for(s32 index = 0; index < mesh->VertexCount; index += 3)
	{
		vertex_attribute* attributes[3];
		attributes[0] = mesh->Attributes + index + 0;
		attributes[1] = mesh->Attributes + index + 1;
		attributes[2] = mesh->Attributes + index + 2;
		
		v4 clip0 = Math_MultiplyM4x4(&clipMatrix, attributes[0]->Vertex);
		v4 clip1 = Math_MultiplyM4x4(&clipMatrix, attributes[1]->Vertex);
		v4 clip2 = Math_MultiplyM4x4(&clipMatrix, attributes[2]->Vertex);
		
		clip0.X = (clip0.X + 1) * halfBufferDim.Width;
		clip0.Y = (clip0.Y + 1) * halfBufferDim.Height;
		
		clip1.X = (clip1.X + 1) * halfBufferDim.Width;
		clip1.Y = (clip1.Y + 1) * halfBufferDim.Height;
		
		clip2.X = (clip2.X + 1) * halfBufferDim.Width;
		clip2.Y = (clip2.Y + 1) * halfBufferDim.Height;
		
		if(Math_SignedAreaOfTriangle(clip0.XY, clip1.XY, clip2.XY) >= 0.0f)
		{
			v3 cameraPosition[3];
			cameraPosition[0] = Math_MultiplyM4x4(&viewMatrix, attributes[0]->Vertex).XYZ;
			cameraPosition[1] = Math_MultiplyM4x4(&viewMatrix, attributes[1]->Vertex).XYZ;
			cameraPosition[2] = Math_MultiplyM4x4(&viewMatrix, attributes[2]->Vertex).XYZ;
			
			f32 oneOverCameraZ0 = 1.0f / cameraPosition[0].Z;
			f32 oneOverCameraZ1 = 1.0f / cameraPosition[1].Z;
			f32 oneOverCameraZ2 = 1.0f / cameraPosition[2].Z;
			
			ComputeDepthForTriangle(depthMap, clip0.XY, clip1.XY, clip2.XY,
									oneOverCameraZ0, oneOverCameraZ1, oneOverCameraZ2);
			
		}
		
	}
}

void
VertexShader_DepthMap(render_bitmap* depthMap, engine_mesh* mesh, m4x4* worldMatrix, 
					  m4x4* cameraMatrix, m4x4* projectionMatrix)
{
	v2 halfBufferDim = { depthMap->Width * 0.5f, depthMap->Height * 0.5f };
	m4x4 viewMatrix = Math_MultiplyM4x4(cameraMatrix, worldMatrix);
	m4x4 clipMatrix = Math_MultiplyM4x4(projectionMatrix, &viewMatrix); 
	
	for(s32 index = 0; index < mesh->VertexCount; index += 3)
	{
		vertex_attribute* attributes[3];
		attributes[0] = mesh->Attributes + index + 0;
		attributes[1] = mesh->Attributes + index + 1;
		attributes[2] = mesh->Attributes + index + 2;
		
		v4 clip0 = Math_MultiplyM4x4(&clipMatrix, attributes[0]->Vertex);
		v4 clip1 = Math_MultiplyM4x4(&clipMatrix, attributes[1]->Vertex);
		v4 clip2 = Math_MultiplyM4x4(&clipMatrix, attributes[2]->Vertex);
		
		clip0.XYZ *= 1.0f / clip0.W;
		clip1.XYZ *= 1.0f / clip1.W;
		clip2.XYZ *= 1.0f / clip2.W;
		
		clip0.X = (clip0.X + 1) * halfBufferDim.Width;
		clip0.Y = (clip0.Y + 1) * halfBufferDim.Height;
		
		clip1.X = (clip1.X + 1) * halfBufferDim.Width;
		clip1.Y = (clip1.Y + 1) * halfBufferDim.Height;
		
		clip2.X = (clip2.X + 1) * halfBufferDim.Width;
		clip2.Y = (clip2.Y + 1) * halfBufferDim.Height;
		
		if(Math_SignedAreaOfTriangle(clip0.XY, clip1.XY, clip2.XY) >= 0.0f)
		{
			f32 oneOverCameraZ0 = 1.0f / clip0.W;
			f32 oneOverCameraZ1 = 1.0f / clip1.W;
			f32 oneOverCameraZ2 = 1.0f / clip2.W;
			
			ComputeDepthForTriangle(depthMap, clip0.XY, clip1.XY, clip2.XY,
									oneOverCameraZ0, oneOverCameraZ1, oneOverCameraZ2);
		}
	}
}
