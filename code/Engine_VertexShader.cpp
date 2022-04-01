

void
VertexShader_MainPass(engine_buffer* buffer, render_bitmap* shadowMap, render_matrial* material,
					  engine_mesh* mesh, m4x4* worldMatrix, 
					  m4x4*lightProjection, m4x4* lightMatrix, m4x4* cameraMatrix, m4x4* projectionMatrix)
{
	m4x4 viewMatrix = Math_MultiplyM4x4(cameraMatrix, worldMatrix);
	m4x4 clipMatrix = Math_MultiplyM4x4(projectionMatrix, &viewMatrix); 
	
	m4x4 lightView = Math_MultiplyM4x4(lightMatrix, worldMatrix);
	m4x4 lightClipMatrix = Math_MultiplyM4x4(lightProjection, &lightView);
	
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
		
		if(Math_SignedAreaOfTriangle(clip0.XY, clip1.XY, clip2.XY) >= 0.0f)
		{
			
			clip0.X = (clip0.X) * buffer->Color.Width;
			clip0.Y = (clip0.Y) * buffer->Color.Height;
			
			clip1.X = (clip1.X) * buffer->Color.Width;
			clip1.Y = (clip1.Y) * buffer->Color.Height;
			
			clip2.X = (clip2.X) * buffer->Color.Width;
			clip2.Y = (clip2.Y) * buffer->Color.Height;
			
			f32 w0 = clip0.Z / clip0.W;
			f32 w1 = clip1.Z / clip1.W;
			f32 w2 = clip2.Z / clip2.W;
			
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
			
			
			v4 lightClip0 = Math_MultiplyM4x4(&lightClipMatrix, attributes[0]->Vertex);
			v4 lightClip1 = Math_MultiplyM4x4(&lightClipMatrix, attributes[1]->Vertex);
			v4 lightClip2 = Math_MultiplyM4x4(&lightClipMatrix, attributes[2]->Vertex);
			
			DrawTriangle(&buffer->Color, &buffer->Depth, shadowMap, material,
						 lightClip0.XYZ, lightClip1.XYZ, lightClip2.XYZ,
						 clip0.XY, clip1.XY, clip2.XY,
						 attributes[0]->UV, attributes[1]->UV, attributes[2]->UV,
						 w0, w1, w2,
						 worldPosition[0], worldPosition[1], worldPosition[2],
						 normals[0], normals[1], normals[2],
						 tangent[0].XYZ, tangent[1].XYZ, tangent[2].XYZ, 
						 bitangent[0], bitangent[1], bitangent[2]);
		}
	}
}

void
VertexShader_DepthMap(render_bitmap* depthMap, engine_mesh* mesh, m4x4* worldMatrix, 
					  m4x4* cameraMatrix, m4x4* projectionMatrix)
{
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
		
		if(Math_SignedAreaOfTriangle(clip0.XY, clip1.XY, clip2.XY) >= 0.0f)
		{
			
			clip0.X = (clip0.X) * depthMap->Width;
			clip0.Y = (clip0.Y) * depthMap->Height;
			
			clip1.X = (clip1.X) * depthMap->Width;
			clip1.Y = (clip1.Y) * depthMap->Height;
			
			clip2.X = (clip2.X) * depthMap->Width;
			clip2.Y = (clip2.Y) * depthMap->Height;
			
			f32 oneOverCameraZ0 = clip0.Z / clip0.W;
			f32 oneOverCameraZ1 = clip1.Z / clip1.W;
			f32 oneOverCameraZ2 = clip2.Z / clip2.W;
			
			ComputeDepthForTriangle(depthMap, clip0.XY, clip1.XY, clip2.XY,
									oneOverCameraZ0, oneOverCameraZ1, oneOverCameraZ2);
		}
	}
}