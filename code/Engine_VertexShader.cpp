
void
VertexShader_MainPass(fragment_group* group,  engine_mesh* mesh, 
					  m4x4* worldMatrix, m4x4* mvpMatrix, m4x4* shadowMVP)
{
	for(s32 index = 0; index < mesh->VertexCount; index += 3)
	{
		
		render_fragment* fragments = FragmentGroup_PushFragment(group, 3);
		for(s32 i = 0; i < 3; ++i)
		{
			vertex_attribute* attribute = mesh->Attributes + index + i;
			
			fragments[i].Position = Math_MultiplyM4x4(mvpMatrix, attribute->Vertex);
			
			v3 normal = Math_MultiplyM4x4(worldMatrix, attribute->Normal, 0).XYZ;
			fragments[i].Normal = normal;
			fragments[i].UV = attribute->UV;
			
			v4 tangent = mesh->Tangents[index + i];
			fragments[i].Tangent = Math_MultiplyM4x4(worldMatrix, tangent.XYZ, 0).XYZ;
			fragments[i].Bitangent = Math_CrossProductV3(normal, tangent.XYZ) * tangent.W;
			fragments[i].ShadowMapCoord = Math_MultiplyM4x4(shadowMVP, attribute->Vertex);
		}
	}
}

void
VertexShader_DepthMap(render_bitmap* depthMap, engine_mesh* mesh, m4x4* mvp)
{
	
	for(s32 index = 0; index < mesh->VertexCount; index += 3)
	{
		vertex_attribute* attributes[3];
		attributes[0] = mesh->Attributes + index + 0;
		attributes[1] = mesh->Attributes + index + 1;
		attributes[2] = mesh->Attributes + index + 2;
		
		v4 position0 = Math_MultiplyM4x4(mvp, attributes[0]->Vertex);
		v4 position1 = Math_MultiplyM4x4(mvp, attributes[1]->Vertex);
		v4 position2 = Math_MultiplyM4x4(mvp, attributes[2]->Vertex);
		
		FragmentShader_ComputeDepth(depthMap, position0, position1, position2);
	}
}