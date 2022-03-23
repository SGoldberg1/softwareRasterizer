
#include <math.h>

inline f32
Math_Sin(f32 radians)
{
	f32 result = sinf(radians);
	return(result);
}

inline f32
Math_Cos(f32 radians)
{
	f32 result = cosf(radians);
	return(result);
}

inline f32
Math_Tan(f32 radians)
{
	f32 result = tanf(radians);
	return(result);
}

inline f32
Math_SafeRatioF32(f32 numerator, f32 denominator)
{
	f32 result = denominator != 0.0f ? (numerator / denominator) : 0;
	return(result);
}

inline f32
Math_AbsoluteF32(f32 value)
{
	f32 result = abs(value);
	return(result);
}

inline s32
Math_AbsoluteS32(s32 value)
{
	s32 result = value >= 0 ? value : -value;
	return(result);
}

inline f32
Math_Sqrt(f32 value)
{
	f32 result = sqrtf(value);
	return(result);
}

inline f32
Math_FloorF32(f32 value)
{
	f32 result = floorf(value);
	return(result);
}

inline f32
Math_RoundF32(f32 value)
{
	f32 result = roundf(value);
	return(result);
}

inline u32
Math_RoundF32ToU32(f32 value)
{
	u32 result = (u32)Math_RoundF32(value);
	return(result);
}

inline s32
Math_RoundF32ToS32(f32 value)
{
	s32 result = (s32)Math_RoundF32(value);
	return(result);
}

inline f32
Math_Clamp01(f32 value)
{
	f32 result = ((value < 0.0f) ? 0 :
				  (value > 1.0f) ? 1.0f : value);
	
	return(result);
}

inline b32
Math_IsClampedF32(f32 value, f32 min, f32 max)
{
	b32 result = (min <= value && value < max);
	return(result);
}

inline b32
Math_IsClampedS32(s32 value, s32 min, s32 max)
{
	b32 result = (min <= value && value < max);
	return(result);
}

inline b32
Math_IsClampedInclusiveF32(f32 value, f32 min, f32 max)
{
	b32 result = (min <= value && value <= max);
	return(result);
}

inline f32
Math_MinF32(f32 a, f32 b)
{
	f32 result = a < b ? a : b;
	return(result);
}

inline f32
Math_MaxF32(f32 a, f32 b)
{
	f32 result = a > b ? a : b;
	return(result);
}

inline f32
Math_Lerp(f32 a, f32 b, f32 t)
{
	f32 result = (a + (t * (b - a)));
	return(result);
}

//////////////////////////////// V2

// NOTE(Stephen): Returns +1 if winding order is CCW, 
//Y-Axis points down and X-Axis points to the right.
//Returns -1 if winding order is CW
inline f32
Math_SignedAreaOfTriangle(v2 vertex0, v2 vertex1, v2 vertex2)
{
	f32 result = ((vertex2.X - vertex0.X) * (vertex1.Y - vertex0.Y) - 
				  (vertex1.X - vertex0.X) * (vertex2.Y - vertex0.Y)) * 0.5f;
	return(result);
}

inline f32 
Math_DotProductV2(v2 a, v2 b)
{
	f32 result = (a.X * b.X + a.Y * b.Y);
	return(result);
}

inline f32 
Math_SquaredMagnitudeV2(v2 a)
{
	f32 result = (a.X * a.X + a.Y * a.Y);
	return(result);
}

inline f32
Math_MagnitudeV2(v2 a)
{
	f32 result = Math_Sqrt(Math_SquaredMagnitudeV2(a));
	return(result);
}

inline v2
Math_NormalizedV2(v2 a, f32 magnitude)
{
	Assert(magnitude != 0.0f);
	v2 result;
	result.X = a.X / magnitude;
	result.Y = a.Y / magnitude;
	return(result);
}

inline v2
Math_NormalizedV2(v2 a)
{
	v2 result = Math_NormalizedV2(a, Math_MagnitudeV2(a));
	return(result);
}

// NOTE(Stephen): Result of taking the cross product two vectors that
//both have a z-coordinate of zero.
inline f32
Math_EdgeFunctionV2(v2 a, v2 b)
{
	f32 result = (a.X * b.Y) - (a.Y * b.X);
	return(result);
}

//////////////////////////////// V3

inline f32 
Math_SquaredMagnitudeV3(v3 a)
{
	f32 result = (a.X * a.X + a.Y * a.Y + a.Z * a.Z);
	return(result);
}

inline f32
Math_MagnitudeV3(v3 a)
{
	f32 result = Math_Sqrt(Math_SquaredMagnitudeV3(a));
	return(result);
}

inline v3
Math_NormalizedV3(v3 a, f32 magnitude)
{
	Assert(magnitude != 0.0f);
	v3 result;
	result.X = a.X / magnitude;
	result.Y = a.Y / magnitude;
	result.Z = a.Z / magnitude;
	return(result);
}

inline v3
Math_NormalizedV3(v3 a)
{
	v3 result = Math_NormalizedV3(a, Math_MagnitudeV3(a));
	return(result);
}

inline f32 
Math_DotProductV3(v3 a, v3 b)
{
	f32 result = (a.X * b.X + a.Y * b.Y + a.Z * b.Z);
	return(result);
}

inline v3
Math_CrossProductV3(v3 a, v3 b)
{
    v3 result;
	result.X = (a.Y * b.Z) - (a.Z * b.Y);
	result.Y = (a.Z * b.X) - (a.X * b.Z);
	result.Z = (a.X * b.Y) - (a.Y * b.X);
	return(result);
}


inline u32
PackV4ToU32(v4 a)
{
	u32 result = ((Math_RoundF32ToU32(a.R * 255.0f) << 24) |
				  (Math_RoundF32ToU32(a.G * 255.0f) << 16) |
				  (Math_RoundF32ToU32(a.B * 255.0f) << 8) |
				  (Math_RoundF32ToU32(a.A * 255.0f) << 0));
	return(result);
}

inline f32 
Math_DotProductV4(v4 a, v3 b)
{
	// NOTE(Stephen): B.W = 1.0f
	f32 result = (a.X * b.X + 
				  a.Y * b.Y +
				  a.Z * b.Z +
				  a.W);
	return(result);
}
//////////////////////////////// BOUNDS

inline cube_bounds
Math_GetCubeBoundsCenter(v3 center, v3 dimensions)
{
	cube_bounds result;
	result.Min.X = center.X - dimensions.X * 0.5f;
	result.Min.Y = center.Y - dimensions.Y * 0.5f;
	result.Min.Z = center.Z - dimensions.Z * 0.5f;
	
	result.Max.X = result.Min.X + dimensions.X;
	result.Max.Y = result.Min.Y + dimensions.Y;
	result.Max.Z = result.Min.Z + dimensions.Z;
	
	return(result);
}

inline v3
Math_GetMinCubeBounds(cube_bounds bounds)
{
	v3 result;
	result.X = Math_MinF32(bounds.Min.X, bounds.Max.X);
	result.Y = Math_MinF32(bounds.Min.Y, bounds.Max.Y);
	result.Z = Math_MinF32(bounds.Min.Z, bounds.Max.Z);
	return(result);
}

inline v3
Math_GetMaxCubeBounds(cube_bounds bounds)
{
	v3 result;
	result.X = Math_MaxF32(bounds.Min.X, bounds.Max.X);
	result.Y = Math_MaxF32(bounds.Min.Y, bounds.Max.Y);
	result.Z = Math_MaxF32(bounds.Min.Z, bounds.Max.Z);
	return(result);
}

inline b32
Math_IsWithinCubeBounds(cube_bounds bounds, v3 test)
{
	b32 result = (Math_IsClampedInclusiveF32(test.X, bounds.Min.X, bounds.Max.X) &&
				  Math_IsClampedInclusiveF32(test.Y, bounds.Min.Y, bounds.Max.Y) &&
				  Math_IsClampedInclusiveF32(test.Z, bounds.Min.Z, bounds.Max.Z));
	
	return(result);
}
//////////////////////////////// Intersection

inline b32
Math_LineToLineIntersection(v2 p0, v2 p1, v2 p2, v2 p3, 
							v2* intersection)
{
	b32 result = FALSE;
	
	v2 deltaA = (p1 - p0);
    v2 deltaB = (p3 - p2);
	
	f32 determinant = ((deltaA.X * deltaB.Y) - (deltaA.Y * deltaB.X));
	
	if(determinant != 0.0f)
	{
		f32 determinantA1 = (((p2.X - p0.X) * deltaB.Y) - ((p2.Y - p0.Y) * deltaB.X));
		f32 determinantA2 = ((deltaA.X * (p2.Y - p0.Y)) - (deltaA.Y * (p2.X - p0.X)));
		f32 t =  (determinantA1 / determinant);
		f32 s = -(determinantA2 / determinant);
		
		if(Math_IsClampedInclusiveF32(t, 0.0f, 1.0f) &&
		   Math_IsClampedInclusiveF32(s, 0.0f, 1.0f))
		{
			*intersection = p0 + t * deltaA;
			result = TRUE;
		}
	}
	
	return(result);
}

struct ray_hit
{
    b32 Hit;
    v3 Point;
    f32 Distance;
};

inline ray_hit
Math_RayIntersectPlane(v3 rayDirection, v3 rayOrigin, 
					   f32 rayLength, v3 planeNormal, v3 planePoint)
{
    ray_hit result = {};
    f32 denominator = Math_DotProductV3(rayDirection, planeNormal);
    
    if(denominator != 0.0f)
	{
		f32 t = (Math_DotProductV3(planePoint - rayOrigin, planeNormal) / denominator);
		
		if(t >= 0)
		{
			v3 direction = rayDirection * t;
			f32 magnitudeSquared = Math_SquaredMagnitudeV3(direction);
			
			if(magnitudeSquared)
			{
				result.Point = direction + rayOrigin;
				result.Distance = Math_Sqrt(magnitudeSquared);
				result.Hit = result.Distance <= rayLength;
			}
		}
	}
	
	return(result);
}


inline v3
Math_LineToPlaneIntersection(v3 start, v3 end, v3 planeNormal, v3 planePoint)
{
	v3 result = {};
	v3 delta = end - start;
	f32 numerator = Math_DotProductV3(planeNormal, planePoint - start);
	f32 denominator = Math_DotProductV3(planeNormal, delta);
	Assert(denominator != 0.0f);
	f32 t = numerator / denominator;
	result = start + t * delta;
	return(result);
}

s32
Math_ClipTriangleAgainstPlane(v3 vertex0, v3 vertex1, v3 vertex2, 
							  v3 planePoint, v3 planeNormal,
							  clipped_triangle* triangleA, clipped_triangle* triangleB)
{
	s32 result = 0;
	s32 insideCount = 0;
	s32 outsideCount = 0;
	v3* insideVertices[3];
	v3* outsideVertices[3];
	
	if(Math_DotProductV3(planeNormal, vertex0 - planePoint) >= 0.0f) { insideVertices[insideCount++] = &vertex0; }
	else { outsideVertices[outsideCount++] = &vertex0; }
	if(Math_DotProductV3(planeNormal, vertex1 - planePoint) >= 0.0f) { insideVertices[insideCount++] = &vertex1; }
	else { outsideVertices[outsideCount++] = &vertex1; }
	if(Math_DotProductV3(planeNormal, vertex2 - planePoint) >= 0.0f) { insideVertices[insideCount++] = &vertex2; }
	else { outsideVertices[outsideCount++] = &vertex2; }
	
	if(outsideCount == 3) 
	{
		result = 0;
	}
	if(insideCount == 3)
	{
		triangleA->Vertex0 = vertex0;
		triangleA->Vertex1 = vertex1;
		triangleA->Vertex2 = vertex2;
		result = 1;
	}
	else if(insideCount == 1)
	{
		//TO MAINTAIN CCW WINDING ORDER OF VERTICES
		if(outsideVertices[0] == &vertex0 && outsideVertices[1] == &vertex2)
		{
			outsideVertices[0] = &vertex2;
			outsideVertices[1] = &vertex0;
		}
		
		v3 outside0 = *outsideVertices[0];
		v3 outside1 = *outsideVertices[1];
		
		v3 inside = *insideVertices[0];
		v3 intersectionA = Math_LineToPlaneIntersection(inside, outside0, planeNormal, planePoint);
		v3 intersectionB = Math_LineToPlaneIntersection(inside, outside1, planeNormal, planePoint);
		
		triangleA->Vertex0 = inside;
		triangleA->Vertex1 = intersectionA;
		triangleA->Vertex2 = intersectionB;
		result = 1;
	}
	else if(insideCount == 2)
	{
		//TO MAINTAIN CCW WINDING ORDER OF VERTICES
		if(insideVertices[0] == &vertex0 && insideVertices[1] == &vertex2)
		{
			insideVertices[0] = &vertex2;
			insideVertices[1] = &vertex0;
		}
		
		v3 outside = *outsideVertices[0];
		v3 inside0 = *insideVertices[0];
		v3 inside1 = *insideVertices[1];
		v3 intersectionA = Math_LineToPlaneIntersection(inside0, outside, planeNormal, planePoint);
		v3 intersectionB = Math_LineToPlaneIntersection(inside1, outside, planeNormal, planePoint);
		
		triangleA->Vertex0 = inside0;
		triangleA->Vertex1 = intersectionB;
		triangleA->Vertex2 = intersectionA;
		
		triangleB->Vertex0 = inside0;
		triangleB->Vertex1 = inside1;
		triangleB->Vertex2 = intersectionB;
		
		result = 2;
	}
	
	
	return(result);
}
