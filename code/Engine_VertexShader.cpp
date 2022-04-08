

void
VertexShader_MainPass(fragment_group* group,  engine_mesh* mesh, 
					  m4x4* worldMatrix, m4x4* mvpMatrix, m4x4* shadowMVP)
{
	TIMED_SCOPE(FullScope);
	for(s32 index = 0; index < mesh->VertexCount; index += 3)
	{
		render_fragment* fragments = FragmentGroup_PushFragment(group, 3);
		
		for(s32 i = 0; i < 3; ++i)
		{
			vertex_attribute* attribute = mesh->Attributes + index + i;
			
			fragments[i].Position = Math_MultiplyM4x4(mvpMatrix, attribute->Vertex);
			fragments[i].Normal = Math_MultiplyM4x4(worldMatrix, attribute->Normal, 0);
			fragments[i].UV = attribute->UV;
			fragments[i].Tangent = Math_MultiplyM4x4(worldMatrix, attribute->Tangent, 0);
			fragments[i].Bitangent = Math_MultiplyM4x4(worldMatrix, attribute->Bitangent, 0);
			fragments[i].ShadowMapCoord = Math_MultiplyM4x4(shadowMVP, attribute->Vertex);
		}
	}
}

void
VertexShader_MainPass_4X(fragment_group* group,  engine_mesh_m128* mesh, 
						 m4x4* worldMatrix, m4x4* mvpMatrix, m4x4* shadowMVP)
{
	TIMED_SCOPE(FullScope);
	v4_4x out;
	
	//printf("%d\n", mesh->VertexCount);
	//printf("%d\n", mesh->M128Count);
	for(s32 index = 0; index < mesh->M128Count; ++index)
	{
		render_fragment* fragments = FragmentGroup_PushFragment(group, 4);
		render_fragment* fragment0 = fragments + 0;
		render_fragment* fragment1 = fragments + 1;
		render_fragment* fragment2 = fragments + 2;
		render_fragment* fragment3 = fragments + 3;
		
		vertex_attribute_4x* attribute = mesh->Attributes + index;
		
		//Extract Vertices
		Math_MatrixMultiply(mvpMatrix, &attribute->Vertices, &out);
		__m128 xy0 = _mm_movelh_ps(out.m128_X, out.m128_Y);
		__m128 xy1 = _mm_movehl_ps(out.m128_X, out.m128_Y);
		__m128 zw0 = _mm_movelh_ps(out.m128_Z, out.m128_W);
		__m128 zw1 = _mm_movehl_ps(out.m128_Z, out.m128_W);
		
		fragment0->Position.m128_data = _mm_shuffle_ps(xy0, zw0, ShuffleMask(0, 2, 0, 2));
		fragment1->Position.m128_data = _mm_shuffle_ps(xy0, zw0, ShuffleMask(1, 3, 1, 3));
		fragment2->Position.m128_data = _mm_shuffle_ps(xy1, zw1, ShuffleMask(2, 0, 2, 0));
		fragment3->Position.m128_data = _mm_shuffle_ps(xy1, zw1, ShuffleMask(3, 1, 3, 1));
		
		//Extract Vertices
		Math_MatrixMultiply(worldMatrix, &attribute->Normals, &out);
		xy0 = _mm_movelh_ps(out.m128_X, out.m128_Y);
		xy1 = _mm_movehl_ps(out.m128_X, out.m128_Y);
		zw0 = _mm_movelh_ps(out.m128_Z, out.m128_W);
		zw1 = _mm_movehl_ps(out.m128_Z, out.m128_W);
		
		fragment0->Normal.m128_data = _mm_shuffle_ps(xy0, zw0, ShuffleMask(0, 2, 0, 2));
		fragment1->Normal.m128_data = _mm_shuffle_ps(xy0, zw0, ShuffleMask(1, 3, 1, 3));
		fragment2->Normal.m128_data = _mm_shuffle_ps(xy1, zw1, ShuffleMask(2, 0, 2, 0));
		fragment3->Normal.m128_data = _mm_shuffle_ps(xy1, zw1, ShuffleMask(3, 1, 3, 1));
		
		// TODO(Stephen): Extrave UVs
		fragment0->UV = { attribute->UVs.X[0], attribute->UVs.Y[0] };
		fragment1->UV = { attribute->UVs.X[1], attribute->UVs.Y[1] };
		fragment2->UV = { attribute->UVs.X[2], attribute->UVs.Y[2] };
		fragment3->UV = { attribute->UVs.X[3], attribute->UVs.Y[3] };
		
		//Extract Tangents
		Math_MatrixMultiply(worldMatrix, &attribute->Tangents, &out);
		xy0 = _mm_movelh_ps(out.m128_X, out.m128_Y);
		xy1 = _mm_movehl_ps(out.m128_X, out.m128_Y);
		zw0 = _mm_movelh_ps(out.m128_Z, out.m128_W);
		zw1 = _mm_movehl_ps(out.m128_Z, out.m128_W);
		
		fragment0->Tangent.m128_data = _mm_shuffle_ps(xy0, zw0, ShuffleMask(0, 2, 0, 2));
		fragment1->Tangent.m128_data = _mm_shuffle_ps(xy0, zw0, ShuffleMask(1, 3, 1, 3));
		fragment2->Tangent.m128_data = _mm_shuffle_ps(xy1, zw1, ShuffleMask(2, 0, 2, 0));
		fragment3->Tangent.m128_data = _mm_shuffle_ps(xy1, zw1, ShuffleMask(3, 1, 3, 1));
		
		//Extract Bitangets
		Math_MatrixMultiply(worldMatrix, &attribute->Bitangents, &out);
		xy0 = _mm_movelh_ps(out.m128_X, out.m128_Y);
		xy1 = _mm_movehl_ps(out.m128_X, out.m128_Y);
		zw0 = _mm_movelh_ps(out.m128_Z, out.m128_W);
		zw1 = _mm_movehl_ps(out.m128_Z, out.m128_W);
		
		fragment0->Bitangent.m128_data = _mm_shuffle_ps(xy0, zw0, ShuffleMask(0, 2, 0, 2));
		fragment1->Bitangent.m128_data = _mm_shuffle_ps(xy0, zw0, ShuffleMask(1, 3, 1, 3));
		fragment2->Bitangent.m128_data = _mm_shuffle_ps(xy1, zw1, ShuffleMask(2, 0, 2, 0));
		fragment3->Bitangent.m128_data = _mm_shuffle_ps(xy1, zw1, ShuffleMask(3, 1, 3, 1));
		
		//Extract ShadowMap Coordinates
		Math_MatrixMultiply(shadowMVP, &attribute->Vertices, &out);
		xy0 = _mm_movelh_ps(out.m128_X, out.m128_Y);
		xy1 = _mm_movehl_ps(out.m128_X, out.m128_Y);
		zw0 = _mm_movelh_ps(out.m128_Z, out.m128_W);
		zw1 = _mm_movehl_ps(out.m128_Z, out.m128_W);
		
		fragment0->ShadowMapCoord.m128_data = _mm_shuffle_ps(xy0, zw0, ShuffleMask(0, 2, 0, 2));
		fragment1->ShadowMapCoord.m128_data = _mm_shuffle_ps(xy0, zw0, ShuffleMask(1, 3, 1, 3));
		fragment2->ShadowMapCoord.m128_data = _mm_shuffle_ps(xy1, zw1, ShuffleMask(2, 0, 2, 0));
		fragment3->ShadowMapCoord.m128_data = _mm_shuffle_ps(xy1, zw1, ShuffleMask(3, 1, 3, 1));
	}
	
	// NOTE(Stephen): Discrard empty vertices that were added because of the 4x packing
	if(mesh->Remainder != 0)
	{
		group->Count = group->Count + mesh->Remainder - 4;
	}
	
}

void
DrawMeshAttributes(render_bitmap* buffer, engine_mesh* mesh, m4x4* worldMatrix, m4x4* mvpMatrix)
{
	for(s32 index = 0; index < mesh->VertexCount; index += 3)
	{
		
		for(s32 i = 0; i < 3; ++i)
		{
			vertex_attribute* attribute = mesh->Attributes + index + i;
			v4 vertex = Math_MultiplyM4x4(mvpMatrix, attribute->Vertex, 1);
			
			f32 scale = 1.0f / vertex.W;
			vertex.XYZ *= scale;
			
			vertex.X *= buffer->Width;
			vertex.Y *= buffer->Height;
			
			
			v3 normal = Math_NormalizedV3(Math_MultiplyM4x4(worldMatrix, attribute->Normal, 0).XYZ);
			normal.Y *= -1;
			DrawLine2D(buffer, vertex.XY, vertex.XY + normal.XY * scale * 100, {0, 1, 0, 1});
		}
	}
}

void
VertexShader_DepthMap(render_bitmap* depthMap, engine_mesh* mesh, m4x4* mvp)
{
	TIMED_SCOPE(FullScope);
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