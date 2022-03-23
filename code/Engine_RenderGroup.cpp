
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
	
	f32 a = (2.0f * nearPlane) / (viewRight - viewLeft);
	f32 b = (2.0f * nearPlane) / (viewTop - viewBottom);
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

internal_function inline v3
InverseTransformVertex(render_basis basis, v3 vertex, f32 w = 1.0f)
{
	v3 result;
	result.X = Math_DotProductV3(vertex, 
								 basis.AxisX) + basis.Position.X * w;
	result.Y = Math_DotProductV3(vertex,
								 basis.AxisY) + basis.Position.Y * w;
	result.Z = Math_DotProductV3(vertex, 
								 basis.AxisZ) + basis.Position.Z * w;
	
	return(result);
}

internal_function s32
ComputeTriangles(render_group* group, engine_mesh* mesh,
				 render_basis basis)
{
	s32 result = 0;
	memory_block* triangleStack = &group->ClippedTriangleStack;
	Assert(mesh->TriangleCount >= 3);
	
	for(s32 index = 0; index < mesh->TriangleCount; index += 3)
	{
		v3 vertex0 = mesh->Vertices[mesh->Triangles[index + 0]].Position;
		
		v3 vertex1 = mesh->Vertices[mesh->Triangles[index + 1]].Position;
		v3 vertex2 = mesh->Vertices[mesh->Triangles[index + 2]].Position;
		
		v3 line0 = vertex1 - vertex0;
		v3 line1 = vertex2 - vertex0;
		v3 fragNormal = Math_NormalizedV3(Math_CrossProductV3(line0, line1));
		
		vertex0 = TransformVertex(basis, vertex0);
		vertex1 = TransformVertex(basis, vertex1);
		vertex2 = TransformVertex(basis, vertex2);
		
		vertex0 = InverseTransformVertex(group->CameraBasis, vertex0);
		vertex1 = InverseTransformVertex(group->CameraBasis, vertex1);
		vertex2 = InverseTransformVertex(group->CameraBasis, vertex2);
		
		line0 = vertex1 - vertex0;
		line1 = vertex2 - vertex0;
		v3 normal = Math_CrossProductV3(line0, line1);
		v3 toCamera = -vertex0;
		
		if(Math_DotProductV3(normal, toCamera) > 0.0f)
		{
			fragNormal = Math_NormalizedV3(TransformVertex(basis, fragNormal, 0));
			
			clipped_triangle triangles[2];
			s32 triangleClipCount = Math_ClipTriangleAgainstPlane(vertex0, vertex1, vertex2,
																  {0, 0, -0.2f}, {0, 0, -1},
																  &triangles[0], &triangles[1]);
			
			for(s32 i = 0; i < triangleClipCount; ++i)
			{
				//wX -> Math_DotProductV4(group->Perspective.Project.Row4, vertexX)
				//This just negates the z component and assignes it to wX
				f32 w0 = -triangles[i].Vertex0.Z;
				f32 w1 = -triangles[i].Vertex1.Z;
				f32 w2 = -triangles[i].Vertex2.Z;
				
				f32 x0 = Math_DotProductV4(group->Perspective.Project.Row1, triangles[i].Vertex0) / w0;
				f32 y0 = Math_DotProductV4(group->Perspective.Project.Row2, triangles[i].Vertex0) / w0;
				f32 z0 = Math_DotProductV4(group->Perspective.Project.Row3, triangles[i].Vertex0) / w0;
				
				f32 x1 = Math_DotProductV4(group->Perspective.Project.Row1, triangles[i].Vertex1) / w1;
				f32 y1 = Math_DotProductV4(group->Perspective.Project.Row2, triangles[i].Vertex1) / w1;
				f32 z1 = Math_DotProductV4(group->Perspective.Project.Row3, triangles[i].Vertex1) / w1;
				
				f32 x2 = Math_DotProductV4(group->Perspective.Project.Row1, triangles[i].Vertex2) / w2;
				f32 y2 = Math_DotProductV4(group->Perspective.Project.Row2, triangles[i].Vertex2) / w2;
				f32 z2 = Math_DotProductV4(group->Perspective.Project.Row3, triangles[i].Vertex2) / w2;
				
				// NOTE(Stephen): NEGATE Y DUE TO PROJECTION CAUSING
				// Y AXIS TO BE UPSIDE DOWN
				//THIS IS DUE TO THE +Z AXIS CHANGING FROM OUT OF THE SCREEN
				//TO 'INTO' THE SCREEN
				v3 projected0 = { x0, -y0, z0 };
				v3 projected1 = { x1, -y1, z1 };
				v3 projected2 = { x2, -y2, z2 };
				
				clipped_triangle* a = MemoryBlock_PushStruct(triangleStack, clipped_triangle);
				*a = { projected0, projected1, projected2 };
			}
			
			for(s32 plane = 0; plane < 5; ++plane)
			{
				s32 triangleCount = triangleStack->Count;
				
				for(s32 t = 0; t < triangleCount; ++t)
				{
					clipped_triangle* triangle = MemoryBlock_PopStruct(triangleStack, clipped_triangle);
					
					switch(plane)
					{
						case 0:
						{
							triangleClipCount = Math_ClipTriangleAgainstPlane(triangle->Vertex0, 
																			  triangle->Vertex1, 
																			  triangle->Vertex2,
																			  {-1, 0, 0}, {1, 0, 0},
																			  &triangles[0],
																			  &triangles[1]);
						}break;
						case 1:
						{
							triangleClipCount = Math_ClipTriangleAgainstPlane(triangle->Vertex0, 
																			  triangle->Vertex1, 
																			  triangle->Vertex2,
																			  {0, -1, 0}, {0, 1, 0},
																			  &triangles[0],
																			  &triangles[1]);
						}break;
						case 2:
						{
							triangleClipCount = Math_ClipTriangleAgainstPlane(triangle->Vertex0, 
																			  triangle->Vertex1, 
																			  triangle->Vertex2,
																			  {0, 1, 0}, {0, -1, 0},
																			  &triangles[0],
																			  &triangles[1]);
						}break;
						case 3:
						{
							triangleClipCount = Math_ClipTriangleAgainstPlane(triangle->Vertex0, 
																			  triangle->Vertex1, 
																			  triangle->Vertex2,
																			  {1, 0, 0}, {-1, 0, 0},
																			  &triangles[0],
																			  &triangles[1]);
						}break;
						case 4:
						{
							triangleClipCount = Math_ClipTriangleAgainstPlane(triangle->Vertex0, 
																			  triangle->Vertex1, 
																			  triangle->Vertex2,
																			  {0, 0, 1}, {0, 0, -1},
																			  &triangles[0],
																			  &triangles[1]);
						}break;
						
					}//switch(plane)
					
					for(s32 c = 0; c < triangleClipCount; ++c)
					{
						clipped_triangle* a = MemoryBlock_PushStruct(triangleStack, clipped_triangle);
						*a = triangles[c];
					}//for(s32 c = 0; c < triangleClipCount; ++c)
				}//for(s32 t = 0; t < triangleCount; ++t)
			}//for(s32 plane = 0; plane < 5; ++plane)
			
			s32 length = triangleStack->Count;
			result += length;
			for(s32 i = 0; i < length; ++i)
			{
				clipped_triangle* source = MemoryBlock_PopStruct(triangleStack, clipped_triangle);
				RenderGroup_PushProjectedTriangle(group,
												  source->Vertex0, 
												  source->Vertex1, 
												  source->Vertex2,
												  fragNormal);
			}
			
		}//if(Math_DotProductV3(normal, toCamera) > 0.0f)
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
