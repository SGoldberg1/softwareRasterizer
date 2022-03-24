

internal_function void
DrawLine2D(render_bitmap* buffer, v2 start, v2 end, v4 color)
{
	u32 pixelColor = PackV4ToU32(color);
	
	f32 x0 = Math_RoundF32(start.X);
	f32 y0 = Math_RoundF32(start.Y);
	f32 x1 = Math_RoundF32(end.X);
	f32 y1 = Math_RoundF32(end.Y);
	
#if 0	
	Assert(Math_IsClampedF32(x0, 0, buffer->Width));
	Assert(Math_IsClampedF32(y0, 0, buffer->Height));
	Assert(Math_IsClampedF32(x1, 0, buffer->Width));
	Assert(Math_IsClampedF32(y1, 0, buffer->Height));
#endif
	
	v2 delta = { (f32)(x1 - x0), (f32)(y1 - y0) };
	
	s32 absoluteX = (s32)Math_AbsoluteF32(delta.X);
	s32 absoluteY = (s32)Math_AbsoluteF32(delta.Y);
	s32 steps = absoluteX > absoluteY ? absoluteX : absoluteY;
	v2 increment = delta * (1.0f / steps);
	u32* pixel;
	
	for(s32 i = 0; i <= steps; ++i)
	{
		
#if 0
		if(Math_IsClampedF32(x0, 0, buffer->Width) == FALSE ||
		   Math_IsClampedF32(y0, 0, buffer->Height) == FALSE)
		{
			break;
		}
		
		pixel = (u32*)buffer->Pixels + (s32)x0 + (s32)y0 * buffer->Width;
		*pixel = pixelColor;
		
#else
		if(Math_IsClampedF32(x0, 0, buffer->Width) &&
		   Math_IsClampedF32(y0, 0, buffer->Height))
		{
			
			pixel = (u32*)buffer->Pixels + (s32)x0 + (s32)y0 * buffer->Width;
			*pixel = pixelColor;
			
		}
#endif
		
		x0 += increment.X;
		y0 += increment.Y;
	}
}

internal_function inline void
ClearBitmap(render_bitmap* buffer, v4 color)
{
	u32 pixelColor = PackV4ToU32(color);
	s32 size = buffer->Width * buffer->Height;
	u32* pixel = (u32*)buffer->Pixels;
	
	for(s32 i = 0; i < size; ++i)
	{
		*pixel++ = pixelColor;
	}
}

internal_function inline void
ClearDepthBuffer(render_bitmap* buffer, f32 value)
{
	s32 size = buffer->Width * buffer->Height;
	f32* pixel = (f32*)buffer->Pixels;
	
	for(s32 i = 0; i < size; ++i)
	{
		*pixel++ = value;
	}
}

internal_function void
DrawRectangle(render_bitmap* buffer,
			  v2 position, v2 dimensions,
			  v4 color)
{
	s32 left = Math_RoundF32ToS32(position.X - dimensions.Width * 0.5f);
	s32 right = (s32)(left + dimensions.Width);
	s32 top = Math_RoundF32ToS32(position.Y - dimensions.Height * 0.5f);
	s32 bottom = (s32)(top + dimensions.Height);
	u32 pixelColor = PackV4ToU32(color);
	
	if(left < 0) { left = 0; }
	if(top < 0) { top = 0; }
	if(right > buffer->Width) { right = buffer->Width; }
	if(bottom > buffer->Height) { bottom = buffer->Height; }
	
	u32* pixel;
	u32* row = (u32*)buffer->Pixels + left + top * buffer->Width;
	
	for(s32 y = top; y < bottom; ++y)
	{
		pixel = row;
		for(s32 x = left; x < right; ++x)
		{
			*pixel++ = pixelColor;
		}
		
		row += buffer->Width;
	}
}

internal_function void
DrawTriangle(render_bitmap* buffer, render_bitmap* depth, v2 pixel0, v2 pixel1, v2 pixel2,
			 f32 oneOverCameraZ0, f32 oneOverCameraZ1, f32 oneOverCameraZ2, v4 color)
{
	
	u32 H = 0xFFFFFFFF;
	local_persist u32 texture[16 * 16] = 
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
		0, 0, 0, 0, 0, 0, 0, 0, H, H, H, H, H, H, H, H,
		0, 0, 0, 0, 0, 0, 0, 0, H, H, H, H, H, H, H, H,
		0, 0, 0, 0, 0, 0, 0, 0, H, H, H, H, H, H, H, H,
		0, 0, 0, 0, 0, 0, 0, 0, H, H, H, H, H, H, H, H,
		0, 0, 0, 0, 0, 0, 0, 0, H, H, H, H, H, H, H, H,
		0, 0, 0, 0, 0, 0, 0, 0, H, H, H, H, H, H, H, H,
	};
	
	s32 left = Math_RoundF32ToS32(Math_MinF32(pixel0.X, Math_MinF32(pixel1.X, pixel2.X)));
	s32 right = Math_RoundF32ToS32(Math_MaxF32(pixel0.X, Math_MaxF32(pixel1.X, pixel2.X)));
	s32 top = Math_RoundF32ToS32(Math_MinF32(pixel0.Y, Math_MinF32(pixel1.Y, pixel2.Y)));
	s32 bottom = Math_RoundF32ToS32(Math_MaxF32(pixel0.Y, Math_MaxF32(pixel1.Y, pixel2.Y)));
	
	u32 pixelColor = PackV4ToU32(color);
	
	//Scissor Rectangle
	if(left < 0) { left = 0; };
	if(top < 0) { top = 0; };
	if(bottom >= buffer->Height) { bottom = (buffer->Height - 1); };
	if(right >= buffer->Width) { right = (buffer->Width - 1); };
	
	u32* colorBufferPixel;
	u32* colorbufferRow = (u32*)buffer->Pixels + left + top * buffer->Width;
	
	f32* depthBufferPixel;
	f32* depthbufferRow = (f32*)depth->Pixels + left + top * depth->Width;
	
	f32 oneOverArea = 1.0f / Math_SignedAreaOfTriangle(pixel0, pixel1, pixel2);
	
	for(s32 y = top; y <= bottom; ++y)
	{
		colorBufferPixel = colorbufferRow;
		depthBufferPixel = depthbufferRow;
		for(s32 x = left; x <= right; ++x)
		{
			v2 pixelPosition = V2(x + 0.5f, y + 0.5f);
			
			f32 testA = Math_SignedAreaOfTriangle(pixel1, pixel2, pixelPosition);
			f32 testB = Math_SignedAreaOfTriangle(pixel2, pixel0, pixelPosition);
			f32 testC = Math_SignedAreaOfTriangle(pixel0, pixel1, pixelPosition);
			
			if(testA >= 0.0f && testB >= 0.0f && testC >= 0.0f)
			{
				testA *= oneOverArea; 
				testB *= oneOverArea; 
				testC *= oneOverArea; 
				f32 z = Math_Clamp01(oneOverCameraZ0 * testA + 
									 oneOverCameraZ1 * testB + 
									 oneOverCameraZ2 * testC);
				
				if(*depthBufferPixel <= z)
				{
					
#if 0
					v3 c0 = {1, 0, 0};
					v3 c1 = {0, 1, 0};
					v3 c2 = {0, 0, 1};
					v4 c;
					c.R = testA * c0.X + testB * c1.X + testC * c2.X;
					c.G = testA * c0.Y + testB * c1.Y + testC * c2.Y;
					c.B = testA * c0.Z + testB * c1.Z + testC * c2.Z;
					u32 p = PackV4ToU32(c);
					
					f32 u = testB;
					f32 v = testC;
					
					s32 xP = (s32)(15.0f * u);
					s32 yP = (s32)(15.0f * v);
					
					u32* texel = texture + xP + yP * 16;
					p = *texel;
					
					*colorBufferPixel = p;
#else
					*colorBufferPixel = pixelColor;
					
#endif
					
				}
				
			}
			++depthBufferPixel;
			++colorBufferPixel;
		}
		
		colorbufferRow += buffer->Width;
		depthbufferRow += depth->Width;
	}
}


internal_function void 
ComputeDepthForTriangle(render_bitmap* buffer, 
						v2 pixel0, v2 pixel1, v2 pixel2,
						f32 OneOverCameraZ0, f32 OneOverCameraZ1, f32 OneOverCameraZ2)
{
	s32 left = Math_RoundF32ToS32(Math_MinF32(pixel0.X, Math_MinF32(pixel1.X, pixel2.X)));
	s32 right = Math_RoundF32ToS32(Math_MaxF32(pixel0.X, Math_MaxF32(pixel1.X, pixel2.X)));
	s32 top = Math_RoundF32ToS32(Math_MinF32(pixel0.Y, Math_MinF32(pixel1.Y, pixel2.Y)));
	s32 bottom = Math_RoundF32ToS32(Math_MaxF32(pixel0.Y, Math_MaxF32(pixel1.Y, pixel2.Y)));
	
	//Scissor Rectangle
	if(left < 0) { left = 0; };
	if(top < 0) { top = 0; };
	if(bottom >= buffer->Height) { bottom = (buffer->Height - 1); };
	if(right >= buffer->Width) { right = (buffer->Width - 1); };
	
	f32* pixel;
	f32* row = (f32*)buffer->Pixels + left + top * buffer->Width;
	
	f32 cameraZA = OneOverCameraZ0;
	f32 cameraZB = OneOverCameraZ1;
	f32 cameraZC = OneOverCameraZ2;
	
	f32 oneOverArea = 1.0f / Math_SignedAreaOfTriangle(pixel0, pixel1, pixel2);
	
	for(s32 y = top; y <= bottom; ++y)
	{
		pixel = row;
		for(s32 x = left; x <= right; ++x)
		{
			v2 pixelPosition = V2(x + 0.5f, y + 0.5f);
			
			f32 testA = Math_SignedAreaOfTriangle(pixel1, pixel2, pixelPosition);
			f32 testB = Math_SignedAreaOfTriangle(pixel2, pixel0, pixelPosition);
			f32 testC = Math_SignedAreaOfTriangle(pixel0, pixel1, pixelPosition);
			
			if(testA >= 0.0f && testB >= 0.0f && testC >= 0.0f)
			{
				testA *= oneOverArea; 
				testB *= oneOverArea; 
				testC *= oneOverArea; 
				f32 z = cameraZA * testA + cameraZB * testB + cameraZC * testC;
				
				if(*pixel < z)
					*pixel = Math_Clamp01(z);
			}
			
			++pixel;
			
		}
		row += buffer->Width;
	}
	
}

internal_function void
DepthToColor(engine_buffer* buffer)
{
	s32 size = buffer->Color.Width * buffer->Color.Height;
	u32* color = (u32*)buffer->Color.Pixels;
	f32* depth = (f32*)buffer->Depth.Pixels;
	
	for(s32 i = 0; i < size; ++i)
	{
		f32 value = *depth;
		*color++ = PackV4ToU32({value, value, value, 1});
		++depth;
	}
}

void
Render_Buffer(engine_buffer* buffer, render_group* group)
{
	render_bitmap* colorBuffer = &buffer->Color;
	f32 halfWidth = buffer->Color.Width * 0.5f;
	f32 halfHeight = buffer->Color.Height * 0.5f;
	
	for(s32 offset = 0; offset < group->Used; offset += sizeof(render_group_header))
	{
		render_group_header* header = (render_group_header*)(group->Base + offset);
		void* data = header + 1;
		
		switch(header->Type)
		{
			case type_render_clear_screen:
			{
				render_clear_screen* clear = (render_clear_screen*)data;
				offset += sizeof(render_clear_screen);
				ClearBitmap(colorBuffer, clear->Color);
				ClearDepthBuffer(&buffer->Depth, 0.0f);
			}break;
			case type_render_rectangle:
			{
				render_rectangle* rectangle = (render_rectangle*)data;
				offset += sizeof(render_rectangle);
				
				v2 position;
				position.X = rectangle->Position.X;
				position.Y = colorBuffer->Height - rectangle->Position.Y;
				
				DrawRectangle(colorBuffer, position, 
							  rectangle->Dimensions, rectangle->Color);
			}break;
			case type_render_line2d:
			{
				render_line2d* line = (render_line2d*)data;
				offset += sizeof(render_line2d);
				
				v2 start;
				start.X = line->Start.X;
				start.Y = colorBuffer->Height - line->Start.Y;
				
				v2 end;
				end.X = line->End.X;
				end.Y = colorBuffer->Height - line->End.Y;
				DrawLine2D(colorBuffer, start, end, line->Color);
			}break;
			case type_render_mesh:
			{
				render_mesh* mesh = (render_mesh*)data;
				offset += sizeof(render_mesh) + mesh->TriangleCount * sizeof(render_projected_triangle);
				render_projected_triangle* triangle = (render_projected_triangle*)(mesh + 1);
				
				//Compute Depth Buffer
				for(s32 i = 0; i < mesh->TriangleCount; ++i)
				{
					triangle->Pixel0.X *= halfWidth;
					triangle->Pixel0.Y *= halfHeight;
					
					triangle->Pixel1.X *= halfWidth;
					triangle->Pixel1.Y *= halfHeight;
					
					triangle->Pixel2.X *= halfWidth;
					triangle->Pixel2.Y *= halfHeight;
					
					ComputeDepthForTriangle(&buffer->Depth, 
											triangle->Pixel0, 
											triangle->Pixel1, 
											triangle->Pixel2,
											triangle->OneOverCameraZ0, 
											triangle->OneOverCameraZ1,
											triangle->OneOverCameraZ2);
					
					triangle++;
				}
				
				//Render Mesh
				triangle = (render_projected_triangle*)(mesh + 1);
				v3 directionalLight = -Math_NormalizedV3({-1, -1, -1});
				v3 directionalColor = {0.7f, 0.7f, 0.7f };
				
				for(s32 i = 0; i < mesh->TriangleCount; ++i)
				{
					v4 fragColor = mesh->Color;
					f32 intensity = Math_DotProductV3(directionalLight, triangle->FragNormal);
					
					fragColor.R *= directionalColor.X * intensity;
					fragColor.G *= directionalColor.Y * intensity;
					fragColor.B *= directionalColor.Z * intensity;
					
					fragColor.R = Math_Clamp01(fragColor.R);
					fragColor.G = Math_Clamp01(fragColor.G);
					fragColor.B = Math_Clamp01(fragColor.B);
					
					v2 pixel0 = triangle->Pixel0;
					v2 pixel1 = triangle->Pixel1;
					v2 pixel2 = triangle->Pixel2;
					
					DrawTriangle(&buffer->Color, &buffer->Depth, pixel0, pixel1, pixel2,
								 triangle->OneOverCameraZ0, 
								 triangle->OneOverCameraZ1,
								 triangle->OneOverCameraZ2, 
								 fragColor);
					
#if 0
					DrawLine2D(&buffer->Color, pixel0, pixel1, {1, 0, 0, 1});
					DrawLine2D(&buffer->Color, pixel0, pixel2, {1, 0, 0, 1});
					DrawLine2D(&buffer->Color, pixel1, pixel2, {1, 0, 0, 1});
#endif
					
					triangle++;
				}
				
				//ComputeTriangles(group, mesh->Mesh, halfWidth, halfHeight, mesh->Basis, mesh->Color);
			}break;
			InvalidDefaultCase;
		}
		
	}
	
	if(0)
	{
		DepthToColor(buffer);
	}
}