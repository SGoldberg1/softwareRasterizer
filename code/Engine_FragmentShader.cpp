
// TODO(Stephen): Remove this
global_variable v3 cameraPosition = {};


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
		v3 fragToEye = cameraPosition - fragPosition;
		v3 halfwayDirection = Math_NormalizedV3(lightDirection + fragToEye);
		f32 specularFactor = Math_DotProductV3(halfwayDirection, normal);
		//v3 reflect = Math_ReflectV3(lightDirection, normal);
		//specularFactor = Math_DotProductV3(reflect, cameraPosition - fragPosition);
		
		if(specularFactor > 0.0f)
		{
			specularFactor = Math_Pow(specularFactor, material->SpecularShininess);
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

void
FragmentShader_MainPass(engine_buffer* buffer, render_bitmap* shadowMap,
						render_matrial* material, render_fragment* fragment0, 
						render_fragment* fragment1, render_fragment* fragment2)
{
	f32 ambientIntensity = 0.1f;
	v3 ambientColor = V3(1, 1, 1) * ambientIntensity;
	
	v3 lightDirection = -Math_NormalizedV3({-1, -1, -1});
	f32 lightIntensityDirection = 0.15f;
	v3 lightColor = V3(0.957f, 0.914f, 0.608f) * lightIntensityDirection;
	lightColor = V3(1, 1, 1) * lightIntensityDirection;
	
#if 1
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
#else
	v3 lightColors[1] = { lightColor };
	v3 lights[1] = { lightDirection };
#endif
	
	f32 z0 = 1.0f / fragment0->Position.Z;
	fragment0->Position.W  = 1.0f / fragment0->Position.W;
	fragment0->Position.XYZ *= fragment0->Position.W;
	
	f32 z1 = 1.0f / fragment1->Position.Z;
	fragment1->Position.W  = 1.0f / fragment1->Position.W;
	fragment1->Position.XYZ *= fragment1->Position.W;
	
	f32 z2 = 1.0f / fragment2->Position.Z;
	fragment2->Position.W  = 1.0f / fragment2->Position.W;
	fragment2->Position.XYZ *= fragment2->Position.W;
	
	//Perspective correct interpolation per vertex attribute
	fragment0->UV *= fragment0->Position.W;
	fragment1->UV *= fragment1->Position.W;
	fragment2->UV *= fragment2->Position.W;
	
	fragment0->Normal *= fragment0->Position.W;
	fragment1->Normal *= fragment1->Position.W;
	fragment2->Normal *= fragment2->Position.W;
	
	fragment0->Tangent *= fragment0->Position.W;
	fragment1->Tangent *= fragment1->Position.W;
	fragment2->Tangent *= fragment2->Position.W;
	
	fragment0->Bitangent *= fragment0->Position.W;
	fragment1->Bitangent *= fragment1->Position.W;
	fragment2->Bitangent *= fragment2->Position.W;
	
	fragment0->ShadowMapCoord *= fragment0->Position.W;
	fragment1->ShadowMapCoord *= fragment1->Position.W;
	fragment2->ShadowMapCoord *= fragment2->Position.W;
	
	v2 screen0;
	screen0.X = fragment0->Position.X * buffer->Color.Width;
	screen0.Y = fragment0->Position.Y * buffer->Color.Height;
	v2 screen1;
	screen1.X = fragment1->Position.X * buffer->Color.Width;
	screen1.Y = fragment1->Position.Y * buffer->Color.Height;
	v2 screen2;
	screen2.X = fragment2->Position.X * buffer->Color.Width;
	screen2.Y = fragment2->Position.Y * buffer->Color.Height;
	
	f32 oneOverArea = 1.0f / Math_SignedAreaOfTriangle(screen0, screen1, screen2);
	if(oneOverArea < 0.0f)
	{
		return;
	}
	
	s32 left = Math_RoundF32ToS32(Math_MinF32(screen0.X, Math_MinF32(screen1.X, screen2.X)));
	s32 right = Math_RoundF32ToS32(Math_MaxF32(screen0.X, Math_MaxF32(screen1.X, screen2.X)));
	s32 top = Math_RoundF32ToS32(Math_MinF32(screen0.Y, Math_MinF32(screen1.Y, screen2.Y)));
	s32 bottom = Math_RoundF32ToS32(Math_MaxF32(screen0.Y, Math_MaxF32(screen1.Y, screen2.Y)));
	
	//Scissor Rectangle
	if(left < 0) { left = 0; };
	if(top < 0) { top = 0; };
	if(bottom >= buffer->Color.Height) { bottom = (buffer->Color.Height - 1); };
	if(right >= buffer->Color.Width) { right = (buffer->Color.Width - 1); };
	
	u32* colorBufferPixel;
	u32* colorbufferRow = (u32*)buffer->Color.Pixels + left + top * buffer->Color.Width;
	
	f32* depthBufferPixel;
	f32* depthbufferRow = (f32*)buffer->Depth.Pixels + left + top * buffer->Depth.Width;
	
	u32* diffusePixels = (u32*)material->Diffuse.Pixels;
	u32* occlusionPixels = (u32*)material->Occlusion.Pixels;
	u8* normalPixels = (u8*)material->Normal.Pixels;
	f32* shadowMapPixels = (f32*)shadowMap->Pixels;
	
	for(s32 y = top; y <= bottom; ++y)
	{
		colorBufferPixel = colorbufferRow;
		depthBufferPixel = depthbufferRow;
		
		for(s32 x = left; x <= right; ++x)
		{
			v2 screenPosition = V2(x + 0.5f, y + 0.5f);
			
			f32 u = Math_SignedAreaOfTriangle(screen0, screenPosition, screen2);
			f32 v = Math_SignedAreaOfTriangle(screen0, screen1, screenPosition);
			f32 s = Math_SignedAreaOfTriangle(screenPosition, screen1, screen2);
			
			if(u >= 0.0f && v >= 0.0f && s >= 0.0f)
			{
				u *= oneOverArea; 
				v *= oneOverArea; 
				s *= oneOverArea;// NOTE(Stephen): 1.0f - u - v
				
				f32 z = (s * z0 + u * z1 + v * z2);
				
				if(*depthBufferPixel <= z)
				{
					f32 w = 1.0f / (s * fragment0->Position.W + 
									u * fragment1->Position.W + v * fragment2->Position.W);
					
					// Compute interpolated uvs
					v2 uv;
					uv.X = s * fragment0->UV.X + u * fragment1->UV.X + v * fragment2->UV.X;
					uv.Y = s * fragment0->UV.Y + u * fragment1->UV.Y + v * fragment2->UV.Y;
					uv *= w;
					
					//Compute interpolated position
					v3 fragPosition = (s * fragment0->Position.XYZ + 
									   u * fragment1->Position.XYZ + v * fragment2->Position.XYZ);
					fragPosition = Math_NormalizedV3(fragPosition);
					
					//Compute interpolated normal
					v3 fragNormal = (s * fragment0->Normal + 
									 u * fragment1->Normal + v * fragment2->Normal);
					fragNormal = Math_NormalizedV3(fragNormal);
					
					//Compute interpolated shadow map coordinate
					v4 shadowCoord = (s * fragment0->ShadowMapCoord + 
									  u * fragment1->ShadowMapCoord + v * fragment2->ShadowMapCoord);
					
					shadowCoord *= w;
					shadowCoord.XY *= 1.0f / shadowCoord.W;
					
					//////////////////////////////// Compute Pixel Color
					
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
					
					//Get texture sample from normal map
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
					
					v3 tangent = (s * fragment0->Tangent + 
								  u * fragment1->Tangent + v * fragment2->Tangent);
					
					v3 bitangent = (s * fragment0->Bitangent + 
									u * fragment1->Bitangent + v * fragment2->Bitangent);
					
					v3 normal = Math_NormalizedV3(normalMap.X * tangent + 
												  normalMap.Y * bitangent + 
												  normalMap.Z * fragNormal);
					//normal = fragNormal;
					
					v3 finalLightColor {};
					
					for(s32 i = 0; i < ArrayLength(lights); ++i)
					{
						finalLightColor += ComputeLightByDirection(lights[i], 
																   fragPosition, normal,
																   lightColors[i], material, uv);
						
					}
					
					if(Math_IsClampedF32(shadowCoord.X, 0.0f, 1.0f) && 
					   Math_IsClampedF32(shadowCoord.Y, 0.0f, 1.0f))
					{
						shadowCoord.Z = 1.0f / shadowCoord.Z;
						
						//Get texture sample from shadowmap
						s32 textureX = Math_RoundF32ToS32(shadowCoord.X * (shadowMap->Width - 1));
						s32 textureY = Math_RoundF32ToS32((shadowMap->Height - 1) * shadowCoord.Y); 
						f32 zValue = shadowMapPixels[textureY * shadowMap->Width + textureX];
						f32 epsilon = 0.01f;
						
						//shadowMapPixels[textureY * shadowMap->Width + textureX] = 1;
						if(shadowCoord.Z + epsilon < zValue)
						{
							finalLightColor *= zValue;
						}
					}
					
					finalLightColor += ambientColor;
					
					//Apply Ambient Occlusion
					textureX = Math_RoundF32ToS32(uv.X * (material->Occlusion.Width - 1));
					textureY = Math_RoundF32ToS32((material->Occlusion.Height - 1) * (uv.Y)); 
					textureSample = occlusionPixels[textureY * material->Occlusion.Width + textureX];
					
					finalLightColor.R *= (f32)((textureSample >> 24) & 0xFF) * OneOver255;
					finalLightColor.G *= (f32)((textureSample >> 16) & 0xFF) * OneOver255;
					finalLightColor.B *= (f32)((textureSample >> 8 ) & 0xFF) * OneOver255;
					
					//Compute Final Pixel Color
					texel.RGB = Math_HadamardProduct(texel.RGB * OneOverPI, finalLightColor);
					
					texel.RGB.R = Math_Clamp01(texel.RGB.R);
					texel.RGB.G = Math_Clamp01(texel.RGB.G);
					texel.RGB.B = Math_Clamp01(texel.RGB.B);
					
					u8 redChannel = Math_RoundF32ToU32(texel.R * 255.0f);
					u8 greenChannel = Math_RoundF32ToU32(texel.G * 255.0f);
					u8 blueChannel = Math_RoundF32ToU32(texel.B * 255.0f);
					
					u32 pixelColor = ((redChannel << 24) |
									  (greenChannel << 16) |
									  (blueChannel << 8 ) |
									  (0));
					
					*colorBufferPixel = pixelColor;
					
					
				}//if(u >= 0.0f && v >= 0.0f && s >= 0.0f)
				
			}
			++depthBufferPixel;
			++colorBufferPixel;
		}
		
		colorbufferRow += buffer->Color.Width;
		depthbufferRow += buffer->Depth.Width;
	}
}


internal_function void 
FragmentShader_ComputeDepth(render_bitmap* buffer, v4 position0, v4 position1, v4 position2)
{
	
	position0.W = 1.0f / position0.W;
	position0.XY *= position0.W;
	
	position1.W = 1.0f / position1.W;
	position1.XY *= position1.W;
	
	position2.W = 1.0f / position2.W;
	position2.XY *= position2.W;
	
	v2 screen0;
	screen0.X = position0.X * buffer->Width;
	screen0.Y = position0.Y * buffer->Height;
	
	v2 screen1;
	screen1.X = position1.X * buffer->Width;
	screen1.Y = position1.Y * buffer->Height;
	
	v2 screen2;
	screen2.X = position2.X * buffer->Width;
	screen2.Y = position2.Y * buffer->Height;
	
	s32 left = Math_RoundF32ToS32(Math_MinF32(screen0.X, Math_MinF32(screen1.X, screen2.X)));
	s32 right = Math_RoundF32ToS32(Math_MaxF32(screen0.X, Math_MaxF32(screen1.X, screen2.X)));
	s32 top = Math_RoundF32ToS32(Math_MinF32(screen0.Y, Math_MinF32(screen1.Y, screen2.Y)));
	s32 bottom = Math_RoundF32ToS32(Math_MaxF32(screen0.Y, Math_MaxF32(screen1.Y, screen2.Y)));
	
	//Scissor Rectangle
	if(left < 0) { left = 0; };
	if(top < 0) { top = 0; };
	if(bottom >= buffer->Height) { bottom = (buffer->Height - 1); };
	if(right >= buffer->Width) { right = (buffer->Width - 1); };
	
	f32* pixel;
	f32* row = (f32*)buffer->Pixels + left + top * buffer->Width;
	
	f32 oneOverArea = 1.0f / Math_SignedAreaOfTriangle(screen0, screen1, screen2);
	
	f32 z0 = 1.0f / position0.Z;
	f32 z1 = 1.0f / position1.Z;
	f32 z2 = 1.0f / position2.Z;
	
	for(s32 y = top; y <= bottom; ++y)
	{
		pixel = row;
		for(s32 x = left; x <= right; ++x)
		{
			v2 screenPosition = V2(x + 0.5f, y + 0.5f);
			
			f32 u = Math_SignedAreaOfTriangle(screen0, screenPosition, screen2);
			f32 v = Math_SignedAreaOfTriangle(screen0, screen1, screenPosition);
			f32 s = Math_SignedAreaOfTriangle(screenPosition, screen1, screen2);
			
			if(u >= 0.0f && v >= 0.0f && s >= 0.0f)
			{
				u *= oneOverArea; 
				v *= oneOverArea; 
				s *= oneOverArea; 
				f32 z = (s * z0 + u * z1 + v * z2);
				
				if(*pixel <= z)
				{
					*pixel = z;
				}
			}
			
			++pixel;
			
		}
		row += buffer->Width;
	}
	
}
