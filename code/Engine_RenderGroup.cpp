
inline void
RenderGroup_Initialize(render_group* group, s32 size, void* base)
{
	group->Used = 0;
	group->Count = 0;
	group->Size = size / 2;
	group->Base = (u8*)base;
	
	MemoryBlock_Initialize(&group->ClippedTriangleStack, size / 2, 
						   (u8*)base + group->Size);
}

inline void
RenderGroup_Reset(render_group* group)
{
	group->Used = 0;
	group->Count = 0;
	MemoryBlock_Reset(&group->ClippedTriangleStack);
}

inline void
RenderGroup_SetPerspectiveProjection(render_group* group,
									 f32 fieldOfView,
									 f32 farPlane, f32 nearPlane,
									 s32 screenLeft, s32 screenRight,
									 s32 screenTop, s32 screenBottom)
{
	group->FieldOfView = fieldOfView;
	group->NearPlane = nearPlane;
	group->FarPlane = farPlane;
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
	f32 c = -farPlane / (farPlane - nearPlane);
	f32 d = -(farPlane * nearPlane) / (farPlane - nearPlane);
	
	group->Perspective.Project.Row1 = { a,  0,  0,  0  };
	group->Perspective.Project.Row2 = { 0,  b,  0,  0  };
	group->Perspective.Project.Row3 = { 0,  0,  c,  d  };
	group->Perspective.Project.Row4 = { 0,  0, -1,  0  };
	
}

#define RenderGroup_PushElement(group, type) \
(type*)RenderGroup_PushElement_(group, sizeof(type), type_##type)

internal_function inline void*
RenderGroup_PushElement_(render_group* group, s32 size, render_group_type type)
{
	s32 newUsed = group->Used + size + sizeof(render_group_header);
	Assert(newUsed <= group->Size);
	render_group_header* header = (render_group_header*)(group->Base + group->Used);
	++group->Count;
	group->Used = newUsed;
	header->Type = type;
	void* result = header + 1;
	return(result);
}

internal_function inline void*
RenderGroup_Size_(render_group* group, s32 size)
{
	Assert((group->Used + size) <= group->Size);
	void* result = (group->Base + group->Used);
	++group->Count;
	group->Used += size;
	return(result);
}

inline f32
NDCToCameraZ(f32 ndcZ, f32 farPlane, f32 nearPlane)
{
	f32 result = -(farPlane * nearPlane) / (ndcZ * (farPlane - nearPlane) - farPlane);
	return(result);
}

internal_function inline void
RenderGroup_PushProjectedTriangle(render_group* group,
								  v3 ndc0, v3 ndc1, v3 ndc2,
								  v3 fragNormal)
{
	render_projected_triangle* triangle = (render_projected_triangle*)
		RenderGroup_Size_(group, sizeof(render_projected_triangle));
	triangle->NDC0 = ndc0;
	triangle->NDC1 = ndc1;
	triangle->NDC2 = ndc2;
	
	triangle->OneOverCameraZ0 = 1.0f / NDCToCameraZ(ndc0.Z, 
													group->FarPlane, group->NearPlane);
	
	triangle->OneOverCameraZ1 = 1.0f / NDCToCameraZ(ndc1.Z, 
													group->FarPlane, group->NearPlane);
	
	triangle->OneOverCameraZ2 = 1.0f / NDCToCameraZ(ndc2.Z, 
													group->FarPlane, group->NearPlane);
	
	triangle->Pixel0.X = (ndc0.X + 1);
	triangle->Pixel0.Y = (ndc0.Y + 1);
	
	triangle->Pixel1.X = (ndc1.X + 1);
	triangle->Pixel1.Y = (ndc1.Y + 1);
	
	triangle->Pixel2.X = (ndc2.X + 1);
	triangle->Pixel2.Y = (ndc2.Y + 1);
	triangle->FragNormal = fragNormal;
}

internal_function inline v3
TransformVertex(render_basis basis, v3 vertex, f32 w = 1.0f)
{
	v3 result;
	result.X = Math_DotProductV3(vertex, 
								 { basis.AxisX.X, basis.AxisY.X, basis.AxisZ.X }) + basis.Position.X * w;
	result.Y = Math_DotProductV3(vertex,
								 { basis.AxisX.Y, basis.AxisY.Y, basis.AxisZ.Y }) + basis.Position.Y * w;
	result.Z = Math_DotProductV3(vertex, 
								 { basis.AxisX.Z, basis.AxisY.Z, basis.AxisZ.Z }) + basis.Position.Z * w;
	
	return(result);
}

void
RenderGroup_UpdateCameraToProjection(render_group* group, 
									 v3 axisX,
									 v3 axisY,
									 v3 axisZ)
{
	
	m4x4* projection = &group->Perspective.Project;
	m4x4* result = &group->CameraToProjection;
	result->Row1.X = projection->Row1.X * axisX.X;
	result->Row1.Y = projection->Row1.X * axisX.Y;
	result->Row1.Z = projection->Row1.X * axisX.Z;
	result->Row1.W = 0;
	
	result->Row2.X = projection->Row2.Y * axisY.X;
	result->Row2.Y = projection->Row2.Y * axisY.Y;
	result->Row2.Z = projection->Row2.Y * axisY.Z;
	result->Row2.W = 0;
	
	result->Row3.X = projection->Row3.Z * axisZ.X;
	result->Row3.Y = projection->Row3.Z * axisZ.Y;
	result->Row3.Z = projection->Row3.Z * axisZ.Z;
	result->Row3.W = projection->Row3.W;
	
	result->Row4.X = projection->Row4.Z * axisZ.X;
	result->Row4.Y = projection->Row4.Z * axisZ.Y;
	result->Row4.Z = projection->Row4.Z * axisZ.Z;
	result->Row4.W = projection->Row4.W;
	
}

m4x4
Math_ComputeClipSpaceMatrix(m4x4* projection, render_basis* model)
{
	m4x4 result;
	result.Row1.X = Math_DotProductV4(projection->Row1, model->AxisX, 0);
	result.Row1.Y = Math_DotProductV4(projection->Row1, model->AxisY, 0);
	result.Row1.Z = Math_DotProductV4(projection->Row1, model->AxisZ, 0);
	result.Row1.W = Math_DotProductV4(projection->Row1, model->Position);
	
	result.Row2.X = Math_DotProductV4(projection->Row2, model->AxisX, 0);
	result.Row2.Y = Math_DotProductV4(projection->Row2, model->AxisY, 0);
	result.Row2.Z = Math_DotProductV4(projection->Row2, model->AxisZ, 0);
	result.Row2.W = Math_DotProductV4(projection->Row2, model->Position);
	
	result.Row3.X = Math_DotProductV4(projection->Row3, model->AxisX, 0);
	result.Row3.Y = Math_DotProductV4(projection->Row3, model->AxisY, 0);
	result.Row3.Z = Math_DotProductV4(projection->Row3, model->AxisZ, 0);
	result.Row3.W = Math_DotProductV4(projection->Row3, model->Position);
	
	result.Row4.X = Math_DotProductV4(projection->Row4, model->AxisX, 0);
	result.Row4.Y = Math_DotProductV4(projection->Row4, model->AxisY, 0);
	result.Row4.Z = Math_DotProductV4(projection->Row4, model->AxisZ, 0);
	result.Row4.W = Math_DotProductV4(projection->Row4, model->Position);
	return(result);
}

internal_function s32
ComputeTriangles(render_group* group, engine_mesh* mesh,
				 render_basis basis)
{
	s32 result = 0;
	//memory_block* triangleStack = &group->ClippedTriangleStack;
	Assert(mesh->TriangleCount >= 3);
	m4x4 clipSpaceMatrix = Math_ComputeClipSpaceMatrix(&group->CameraToProjection, &basis);
	
	for(s32 index = 0; index < mesh->TriangleCount; index += 3)
	{
		v3 vertex0 = mesh->Vertices[mesh->Triangles[index + 0]].Position;
		v3 vertex1 = mesh->Vertices[mesh->Triangles[index + 1]].Position;
		v3 vertex2 = mesh->Vertices[mesh->Triangles[index + 2]].Position;
		
		// TODO(Stephen): Remove when we have per vertex normal
		v3 line0 = vertex1 - vertex0;
		v3 line1 = vertex2 - vertex0;
		v3 fragNormal = Math_NormalizedV3(Math_CrossProductV3(line0, line1));
		
		v4 projected0 = Math_MultiplyM4x4(&clipSpaceMatrix, vertex0, 1);
		v4 projected1 = Math_MultiplyM4x4(&clipSpaceMatrix, vertex1, 1);
		v4 projected2 = Math_MultiplyM4x4(&clipSpaceMatrix, vertex2, 1);
		
		projected0.XYZ *= Math_SafeRatioF32(1.0f, projected0.W);
		projected1.XYZ *= Math_SafeRatioF32(1.0f, projected1.W);
		projected2.XYZ *= Math_SafeRatioF32(1.0f, projected2.W);
		
		if(Math_SignedAreaOfTriangle(projected0.XY, projected1.XY, projected2.XY) >= 0.0f)
		{
			fragNormal = Math_NormalizedV3(TransformVertex(basis, fragNormal, 0));
			
			result++;
			RenderGroup_PushProjectedTriangle(group,
											  projected0.XYZ, 
											  projected1.XYZ, 
											  projected2.XYZ, 
											  fragNormal);
		}
		
	}//for(s32 index = 0; index < mesh->TriangleCount; index += 3)
	return(result);
}

void
RenderGroup_PushClearScreen(render_group* group, v4 color)
{
	render_clear_screen* clear = RenderGroup_PushElement(group, render_clear_screen);
	clear->Color = color;
}

void
RenderGroup_PushRectangle(render_group* group, v2 position, 
						  v2 dimensions, v4 color)
{
	render_rectangle* rectangle = RenderGroup_PushElement(group, render_rectangle);
	rectangle->Position = position;
	rectangle->Dimensions = dimensions;
	rectangle->Color = color;
}

void
RenderGroup_PushLine2D(render_group* group, v2 start, 
					   v2 end, v4 color)
{
	render_line2d* line = RenderGroup_PushElement(group, render_line2d);
	line->Start = start;
	line->End = end;
	line->Color = color;
}

void
RenderGroup_PushMesh(render_group* group,
					 engine_mesh* engineMesh,
					 render_basis basis, 
					 v4 color)
{
	render_mesh* mesh = RenderGroup_PushElement(group, render_mesh);
	mesh->Basis = basis;
	mesh->Color = color;
	
	mesh->TriangleCount = ComputeTriangles(group, engineMesh,
										   basis);
}
