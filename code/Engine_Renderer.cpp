

void
DrawBitmap(render_bitmap* buffer, render_bitmap* source, 
		   v2 position, v2 dimensions)
{
	s32 left = Math_RoundF32ToS32(position.X);
	s32 right = (s32)(left + dimensions.Width);
	s32 top = Math_RoundF32ToS32(position.Y);
	s32 bottom = (s32)(top + dimensions.Height);
	
	if(left < 0) { left = 0; }
	if(top < 0) { top = 0; }
	if(right >= buffer->Width) { right = buffer->Width - 1; }
	if(bottom >= buffer->Height) { bottom = buffer->Height - 1; }
	
	u32* destinationPixel;
	u32* destinationRow = (u32*)buffer->Pixels + left + top * buffer->Width;
	
	v2 axisX = V2(dimensions.Width, 0);
	v2 axisY = V2(0, dimensions.Height);
	f32 oneOverMagnitudeX = 1.0f / Math_SquaredMagnitudeV2(axisX);
	f32 oneOverMagnitudeY = 1.0f / Math_SquaredMagnitudeV2(axisY);
	
	for(s32 y = top; y <= bottom; ++y)
	{
		destinationPixel= destinationRow;
		for(s32 x = left; x <= right; ++x)
		{
			v2 pixelPosition = V2(x, y);
			
			f32 u = Math_DotProductV2(pixelPosition - position, axisY) * oneOverMagnitudeY;
			f32 v = Math_DotProductV2(pixelPosition - position, axisX) * oneOverMagnitudeX;
			
			f32 texelY = Math_RoundF32(u * (source->Height - 1));
			f32 texelX = Math_RoundF32(v * (source->Width - 1));
			
			u32* sourcePixel = (u32*)source->Pixels + (s32)texelY * source->Height + (s32)texelX;
			*destinationPixel++ = *sourcePixel;
		}
		
		destinationRow += buffer->Width;
	}
	
}

void
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

void
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
DepthToColor(render_bitmap* colorMap, render_bitmap* depthMap)
{
	Assert(colorMap->Width == depthMap->Width);
	Assert(colorMap->Height == depthMap->Height);
	s32 size = colorMap->Width * colorMap->Height;
	u32* color = (u32*)colorMap->Pixels;
	f32* depth = (f32*)depthMap->Pixels;
	
	for(s32 i = 0; i < size; ++i)
	{
		// NOTE(Stephen): Only have 255 bits of precision.
		f32 value = Math_Clamp01(*depth);
		*color++ = PackV4ToU32({value, value, value, 1});
		++depth;
	}
}
