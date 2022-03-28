

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

inline f32
NDCToCameraZ(f32 ndcZ, f32 farPlane, f32 nearPlane)
{
	f32 result = -(farPlane * nearPlane) / (ndcZ * (farPlane - nearPlane) + farPlane);
	return(result);
}

internal_function void
DrawTriangle(render_bitmap* buffer, render_bitmap* depth, render_bitmap* texture,
			 v2 pixel0, v2 pixel1, v2 pixel2, v2 uv0, v2 uv1, v2 uv2,
			 f32 cameraZ0, f32 cameraZ1, f32 cameraZ2, v4 color)
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
	
	u32* colorBufferPixel;
	u32* colorbufferRow = (u32*)buffer->Pixels + left + top * buffer->Width;
	
	f32* depthBufferPixel;
	f32* depthbufferRow = (f32*)depth->Pixels + left + top * depth->Width;
	
	u32* texturePixel = (u32*)texture->Pixels;
	
	f32 oneOverArea = 1.0f / Math_SignedAreaOfTriangle(pixel0, pixel1, pixel2);
	
	f32 oneOverCameraZ0 = 1.0f / cameraZ0; 
	f32 oneOverCameraZ1 = 1.0f / cameraZ1; 
	f32 oneOverCameraZ2 = 1.0f / cameraZ2;
	
	//Perspective correct interpolation per vertex attribute
	uv0 *= oneOverCameraZ0;
	uv1 *= oneOverCameraZ1;
	uv2 *= oneOverCameraZ2;
	
	for(s32 y = top; y <= bottom; ++y)
	{
		colorBufferPixel = colorbufferRow;
		depthBufferPixel = depthbufferRow;
		
		for(s32 x = left; x <= right; ++x)
		{
			v2 pixelPosition = V2(x + 0.5f, y + 0.5f);
			
			f32 u = Math_SignedAreaOfTriangle(pixel0, pixelPosition, pixel2);
			f32 v = Math_SignedAreaOfTriangle(pixel0, pixel1, pixelPosition);
			f32 s = Math_SignedAreaOfTriangle(pixelPosition, pixel1, pixel2);
			
			if(u >= 0.0f && v >= 0.0f && s >= 0.0f)
			{
				u *= oneOverArea; 
				v *= oneOverArea; 
				s *= oneOverArea;
				
				f32 z = (oneOverCameraZ0 * s + oneOverCameraZ1 * u + oneOverCameraZ2 * v);
				if(*depthBufferPixel <= z)
				{
					v2 uv;
					uv.X = s * uv0.X + u * uv1.X + v * uv2.X;
					uv.Y = s * uv0.Y + u * uv1.Y + v * uv2.Y;
					z = 1.0f / z;
					uv *= z;
					uv.X = uv.X * (texture->Width - 1);
					uv.Y = (texture->Height - 1) * (uv.Y);
					
					u32 texelColor = 
						texturePixel[Math_RoundF32ToS32(uv.Y) * texture->Width + 
									 Math_RoundF32ToS32(uv.X)];
					
					v4 texel;
					texel.R = (f32)((texelColor >> 24) & 0xFF) * OneOver255;
					texel.G = (f32)((texelColor >> 16) & 0xFF) * OneOver255;
					texel.B = (f32)((texelColor >> 8 ) & 0xFF) * OneOver255;
					
					u8 redChannel = Math_RoundF32ToU32(Math_Lerp(texel.R, color.R, color.A) * 255.0f);
					u8 greenChannel = Math_RoundF32ToU32(Math_Lerp(texel.G, color.G, color.A) * 255.0f);
					u8 blueChannel = Math_RoundF32ToU32(Math_Lerp(texel.B, color.B, color.A) * 255.0f);
					
					u32 pixelColor = ((redChannel << 24) |
									  (greenChannel << 16) |
									  (blueChannel << 8 ) |
									  (0));
					
					//pixelColor = texelColor;
					
					*colorBufferPixel = pixelColor;
					
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
						f32 cameraZ0, f32 cameraZ1, f32 cameraZ2)
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
	
	f32 oneOverCameraZ0 = 1.0f / cameraZ0; 
	f32 oneOverCameraZ1 = 1.0f / cameraZ1; 
	f32 oneOverCameraZ2 = 1.0f / cameraZ2;
	
	f32 oneOverArea = 1.0f / Math_SignedAreaOfTriangle(pixel0, pixel1, pixel2);
	
	for(s32 y = top; y <= bottom; ++y)
	{
		pixel = row;
		for(s32 x = left; x <= right; ++x)
		{
			v2 pixelPosition = V2(x + 0.5f, y + 0.5f);
			
			f32 u = Math_SignedAreaOfTriangle(pixel0, pixelPosition, pixel2);
			f32 v = Math_SignedAreaOfTriangle(pixel0, pixel1, pixelPosition);
			f32 s = Math_SignedAreaOfTriangle(pixelPosition, pixel1, pixel2);
			
			if(u >= 0.0f && v >= 0.0f && s >= 0.0f)
			{
				u *= oneOverArea; 
				v *= oneOverArea; 
				s *= oneOverArea; 
				
				f32 z = (oneOverCameraZ0 * s + oneOverCameraZ1 * u + oneOverCameraZ2 * v);
				
				if(*pixel < z)
					*pixel = z;
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
		// NOTE(Stephen): Only have 255 bits of precision.
		f32 value = Math_Clamp01(*depth);
		*color++ = PackV4ToU32({value, value, value, 1});
		++depth;
	}
}
