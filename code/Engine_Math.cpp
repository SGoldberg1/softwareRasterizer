
#include <math.h>

inline f32
Math_Pow(f32 base, f32 exponent)
{
	f32 result = powf(base, exponent);
	return(result);
}

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
Math_HadamardProduct(v3 a, v3 b)
{
	v3 result = {a.X * b.X, a.Y * b.Y, a.Z * b.Z};
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

inline v3
Math_ReflectV3(v3 incident, v3 normal)
{
	v3 result = 2.0f * Math_DotProductV3(incident, normal) * normal - incident;
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
Math_DotProductV4(v4 a, v3 b, f32 w = 1)
{
	// NOTE(Stephen): B.W = 1.0f
	f32 result = (a.X * b.X + 
				  a.Y * b.Y +
				  a.Z * b.Z +
				  a.W * w);
	return(result);
}

inline f32 
Math_DotProductV4(v4 a, v4 b)
{
	// NOTE(Stephen): B.W = 1.0f
	f32 result = (a.X * b.X + 
				  a.Y * b.Y +
				  a.Z * b.Z +
				  a.W * b.W);
	return(result);
}

//////////////////////////////// m4x4

inline v4
Math_MultiplyM4x4(m4x4* matrix, v3 vector, f32 w = 1)
{
	v4 result;
	result.X = Math_DotProductV4(matrix->Row1, vector, w);
	result.Y = Math_DotProductV4(matrix->Row2, vector, w);
	result.Z = Math_DotProductV4(matrix->Row3, vector, w);
	result.W = Math_DotProductV4(matrix->Row4, vector, w);
	return(result);
}

inline v4
Math_MultiplyM4x4(m4x4* matrix, v4 vector)
{
	v4 result;
	result.X = Math_DotProductV4(matrix->Row1, vector);
	result.Y = Math_DotProductV4(matrix->Row2, vector);
	result.Z = Math_DotProductV4(matrix->Row3, vector);
	result.W = Math_DotProductV4(matrix->Row4, vector);
	return(result);
}

inline m4x4
Math_MultiplyM4x4(m4x4* a, m4x4* b)
{
	m4x4 result;
	result.Row1.X = Math_DotProductV4(a->Row1, {b->Row1.X, b->Row2.X, b->Row3.X, b->Row4.X});
	result.Row1.Y = Math_DotProductV4(a->Row1, {b->Row1.Y, b->Row2.Y, b->Row3.Y, b->Row4.Y});
	result.Row1.Z = Math_DotProductV4(a->Row1, {b->Row1.Z, b->Row2.Z, b->Row3.Z, b->Row4.Z});
	result.Row1.W = Math_DotProductV4(a->Row1, {b->Row1.W, b->Row2.W, b->Row3.W, b->Row4.W});
	
	result.Row2.X = Math_DotProductV4(a->Row2, {b->Row1.X, b->Row2.X, b->Row3.X, b->Row4.X});
	result.Row2.Y = Math_DotProductV4(a->Row2, {b->Row1.Y, b->Row2.Y, b->Row3.Y, b->Row4.Y});
	result.Row2.Z = Math_DotProductV4(a->Row2, {b->Row1.Z, b->Row2.Z, b->Row3.Z, b->Row4.Z});
	result.Row2.W = Math_DotProductV4(a->Row2, {b->Row1.W, b->Row2.W, b->Row3.W, b->Row4.W});
	
	result.Row3.X = Math_DotProductV4(a->Row3, {b->Row1.X, b->Row2.X, b->Row3.X, b->Row4.X});
	result.Row3.Y = Math_DotProductV4(a->Row3, {b->Row1.Y, b->Row2.Y, b->Row3.Y, b->Row4.Y});
	result.Row3.Z = Math_DotProductV4(a->Row3, {b->Row1.Z, b->Row2.Z, b->Row3.Z, b->Row4.Z});
	result.Row3.W = Math_DotProductV4(a->Row3, {b->Row1.W, b->Row2.W, b->Row3.W, b->Row4.W});
	
	result.Row4.X = Math_DotProductV4(a->Row4, {b->Row1.X, b->Row2.X, b->Row3.X, b->Row4.X});
	result.Row4.Y = Math_DotProductV4(a->Row4, {b->Row1.Y, b->Row2.Y, b->Row3.Y, b->Row4.Y});
	result.Row4.Z = Math_DotProductV4(a->Row4, {b->Row1.Z, b->Row2.Z, b->Row3.Z, b->Row4.Z});
	result.Row4.W = Math_DotProductV4(a->Row4, {b->Row1.W, b->Row2.W, b->Row3.W, b->Row4.W});
	
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
	
	if(determinant > 0.0f)
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
