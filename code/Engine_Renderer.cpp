

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
			
			f32 texelY = Math_RoundF32(u * source->Height);
			f32 texelX = Math_RoundF32(v * source->Width);
			
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

v3
ComputeLightByDirection(v3 lightDirection, v3 fragPosition, v3 normal,
						v3 lightColor, render_matrial* material, v2 uv)
{
	v3 result = {};
	f32 diffuseFactor = Math_DotProductV3(lightDirection, normal);
	v3 diffuseColor = lightColor * diffuseFactor;
	
	v3 specularColor = {};
	u32* specularPixels = (u32*)material->Specular.Pixels;
	
	if(diffuseFactor > 0.0f)
	{
		v3 halfwayDirection = Math_NormalizedV3(lightDirection - fragPosition);
		f32 specularFactor = Math_DotProductV3(halfwayDirection, normal);
		//v3 reflect = Math_ReflectV3(lightDirection, fragNormal);
		//specularFactor = Math_DotProductV3(reflect, -fragPosition);
		
		if(specularFactor > 0.0f)
		{
			specularFactor = Math_Pow(Math_Clamp01(specularFactor), material->SpecularShininess);
			specularColor =
				lightColor * Math_MaxF32(specularFactor * material->SpecularIntensity, 0);
			//Apply Specular Texture
			u32 textureX = Math_RoundF32ToS32(uv.X * (material->Specular.Width - 1));
			u32 textureY = Math_RoundF32ToS32((material->Specular.Height - 1) * (uv.Y)); 
			u32 textureSample =
				specularPixels[textureY * material->Specular.Width + textureX];
			
			f32 specR = (f32)((textureSample >> 24) & 0xFF) * OneOver255;
			f32 specG = (f32)((textureSample >> 16) & 0xFF) * OneOver255;
			f32 specB = (f32)((textureSample >> 8 ) & 0xFF) * OneOver255;
			specularColor.X *= specR;
			specularColor.Y *= specG;
			specularColor.Z *= specB;
			
		}
	}
	
	result = (diffuseColor + specularColor);
	return(result);
}

internal_function void
DrawTriangle(render_bitmap* buffer, render_bitmap* depth, render_matrial* material, 
			 v2 pixel0, v2 pixel1, v2 pixel2, v2 uv0, v2 uv1, v2 uv2,
			 f32 cameraZ0, f32 cameraZ1, f32 cameraZ2, 
			 v3 fragPosition0, v3 fragPosition1, v3 fragPosition2, 
			 v3 fragNormal0, v3 fragNormal1, v3 fragNormal2, 
			 v3 tangent0, v3 tangent1, v3 tangent2, 
			 v3 bitangent0, v3 bitangent1, v3 bitangent2)
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
	
	u32* diffusePixels = (u32*)material->Diffuse.Pixels;
	u32* specularPixels = (u32*)material->Specular.Pixels;
	u32* occlusionPixels = (u32*)material->Occlusion.Pixels;
	u8* normalPixels = (u8*)material->Normal.Pixels;
	
	f32 oneOverArea = 1.0f / Math_SignedAreaOfTriangle(pixel0, pixel1, pixel2);
	
	f32 oneOverCameraZ0 = 1.0f / cameraZ0; 
	f32 oneOverCameraZ1 = 1.0f / cameraZ1; 
	f32 oneOverCameraZ2 = 1.0f / cameraZ2;
	
	v3 lightDirection = -Math_NormalizedV3({-1, -1, -1});
	f32 lightIntensityDirection = 1.0f;
	v3 lightColor = V3(0.957f, 0.914f, 0.608f) * lightIntensityDirection;
	lightColor = V3(0, 1, 0) * lightIntensityDirection;
	
	v3 lightDirection2 = -Math_NormalizedV3({1, 0, 1});
	f32 lightIntensityDirection2 = 1.0f;
	v3 lightColor2 = V3(1, 0, 0) * lightIntensityDirection2;
	
	v3 lightDirection3 = -Math_NormalizedV3({1, 0, -1});
	f32 lightIntensityDirection3 = 2.0f;
	v3 lightColor3 = V3(0, 0, 1) * lightIntensityDirection3;
	
	v3 lightDirection4 = -Math_NormalizedV3({-1, -1, 1});
	f32 lightIntensityDirection4 = 1.0f;
	v3 lightColor4 = V3(1, 1, 1) * lightIntensityDirection4;
	
	v3 lights[4] = { lightDirection, lightDirection2, lightDirection3, lightDirection4 };
	v3 lightColors[4] = { lightColor, lightColor2, lightColor3, lightColor4 };
	
	//Perspective correct interpolation per vertex attribute
	uv0 *= oneOverCameraZ0;
	uv1 *= oneOverCameraZ1;
	uv2 *= oneOverCameraZ2;
	
	f32 oneOverPI = (1.0f / PI);
	
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
				s *= oneOverArea;// NOTE(Stephen): 1.0f - u - v
				
				f32 z = (oneOverCameraZ0 * s + oneOverCameraZ1 * u + oneOverCameraZ2 * v);
				
				if(*depthBufferPixel <= z)
				{
					v2 uv;
					uv.X = s * uv0.X + u * uv1.X + v * uv2.X;
					uv.Y = s * uv0.Y + u * uv1.Y + v * uv2.Y;
					z = 1.0f / z;
					uv *= z;
					
					//Get texture sample from diffuse
					s32 textureX = Math_RoundF32ToS32(uv.X * (material->Diffuse.Width - 1));
					s32 textureY = Math_RoundF32ToS32((material->Diffuse.Height - 1) * (uv.Y)); 
					u32 textureSample = diffusePixels[textureY * material->Diffuse.Width + textureX];
					
					v4 texel;
					texel.R = (f32)((textureSample >> 24) & 0xFF) * OneOver255;
					texel.G = (f32)((textureSample >> 16) & 0xFF) * OneOver255;
					texel.B = (f32)((textureSample >> 8 ) & 0xFF) * OneOver255;
					//Apply object color(if any)
					texel.R = Math_Lerp(texel.R, material->Color.R, material->Color.A);
					texel.G = Math_Lerp(texel.G, material->Color.G, material->Color.A);
					texel.B = Math_Lerp(texel.B, material->Color.B, material->Color.A);
					
					//Compute interpolated position
					v3 fragPosition;
					fragPosition.X = s * fragPosition0.X + u * fragPosition1.X + v * fragPosition2.X;
					fragPosition.Y = s * fragPosition0.Y + u * fragPosition1.Y + v * fragPosition2.Y;
					fragPosition.Z = s * fragPosition0.Z + u * fragPosition1.Z + v * fragPosition2.Z;
					fragPosition = Math_NormalizedV3(fragPosition);
					
					//Compute interpolated normal
					v3 fragNormal;
					fragNormal.X = s * fragNormal0.X + u * fragNormal1.X + v * fragNormal2.X;
					fragNormal.Y = s * fragNormal0.Y + u * fragNormal1.Y + v * fragNormal2.Y;
					fragNormal.Z = s * fragNormal0.Z + u * fragNormal1.Z + v * fragNormal2.Z;
					fragNormal = Math_NormalizedV3(fragNormal);
					
					//Normal Mapping
					textureX = Math_RoundF32ToS32(uv.X * (material->Normal.Width - 1));
					textureY = Math_RoundF32ToS32((material->Normal.Height - 1) * (uv.Y)); 
					u8* normalSample = normalPixels + textureY * material->Normal.Width * 3 + 
						textureX * 3;
					
					v3 normalMap;
					normalMap.X = (f32)(normalSample[2]) * OneOver255;
					normalMap.Y = (f32)(normalSample[1]) * OneOver255;
					normalMap.Z = (f32)(normalSample[0]) * OneOver255;
					// TODO(Stephen): Pre compute this in asset processor
					normalMap = Math_NormalizedV3(normalMap * 2.0f - V3(1, 1, 1));
					
					v3 tangent;
					tangent.X = s * tangent0.X + u * tangent1.X + v * tangent2.X;
					tangent.Y = s * tangent0.Y + u * tangent1.Y + v * tangent2.Y;
					tangent.Z = s * tangent0.Z + u * tangent1.Z + v * tangent2.Z;
					//tangent = Math_NormalizedV3(tangent);
					
					v3 bitangent;
					bitangent.X = s * bitangent0.X + u * bitangent1.X + v * bitangent2.X;
					bitangent.Y = s * bitangent0.Y + u * bitangent1.Y + v * bitangent2.Y;
					bitangent.Z = s * bitangent0.Z + u * bitangent1.Z + v * bitangent2.Z;
					//bitangent = Math_NormalizedV3(bitangent);
					
					v3 normal = Math_NormalizedV3(normalMap.X * tangent + 
												  normalMap.Y * bitangent + 
												  normalMap.Z * fragNormal);
					
					
#if 0
					//Compute Specular
					f32 diffuseFactor = Math_DotProductV3(lightDirection, normal);
					
					v3 specularColor = {};
					
					if(diffuseFactor > 0.0f)
					{
						v3 halfwayDirection = Math_NormalizedV3(lightDirection - fragPosition);
						f32 specularFactor = Math_DotProductV3(halfwayDirection, normal);
						//v3 reflect = Math_ReflectV3(lightDirection, fragNormal);
						//specularFactor = Math_DotProductV3(reflect, -fragPosition);
						
						if(specularFactor > 0.0f)
						{
							specularFactor = Math_Pow(Math_Clamp01(specularFactor), 
													  material->SpecularShininess);
							specularColor =
								lightColor * Math_MaxF32(specularFactor * material->SpecularIntensity, 0);
							//Apply Specular Texture
							textureX = Math_RoundF32ToS32(uv.X * (material->Specular.Width - 1));
							textureY = Math_RoundF32ToS32((material->Specular.Height - 1) * (uv.Y)); 
							textureSample =
								specularPixels[textureY * material->Specular.Width + textureX];
							
							f32 specR = (f32)((textureSample >> 24) & 0xFF) * OneOver255;
							f32 specG = (f32)((textureSample >> 16) & 0xFF) * OneOver255;
							f32 specB = (f32)((textureSample >> 8 ) & 0xFF) * OneOver255;
							
#if 1
							specularColor.X *= specR;
							specularColor.Y *= specG;
							specularColor.Z *= specB;
#endif
							
						}
					}
					
					v3 finalLightColor = (lightColor * diffuseFactor + specularColor);
#else
					
					v3 finalLightColor = {};
					for(s32 i = 0; i < ArrayLength(lights); ++i)
					{
						finalLightColor += ComputeLightByDirection(lights[i], fragPosition, normal,
																   lightColors[i], material, uv);
					}
					
#endif
					
					
					//Apply Ambient Occlusion
					textureX = Math_RoundF32ToS32(uv.X * (material->Occlusion.Width - 1));
					textureY = Math_RoundF32ToS32((material->Occlusion.Height - 1) * (uv.Y)); 
					textureSample = occlusionPixels[textureY * material->Occlusion.Width + textureX];
					
					f32 occR = (f32)((textureSample >> 24) & 0xFF) * OneOver255;
					f32 occG = (f32)((textureSample >> 16) & 0xFF) * OneOver255;
					f32 occB = (f32)((textureSample >> 8 ) & 0xFF) * OneOver255;
					
					texel.X *= occR;
					texel.Y *= occG;
					texel.Z *= occB;
					
					//Compute Final Pixel Color
					texel.RGB = Math_HadamardProduct(texel.RGB * oneOverPI, finalLightColor);
					texel.RGB.X = Math_Clamp01(texel.RGB.X);
					texel.RGB.Y = Math_Clamp01(texel.RGB.Y);
					texel.RGB.Z = Math_Clamp01(texel.RGB.Z);
					u8 redChannel = Math_RoundF32ToU32(texel.R * 255.0f);
					u8 greenChannel = Math_RoundF32ToU32(texel.G * 255.0f);
					u8 blueChannel = Math_RoundF32ToU32(texel.B * 255.0f);
					
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
