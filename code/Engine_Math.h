/* date = March 15th 2022 9:32 am */

#ifndef _ENGINE__MATH_H
#define _ENGINE__MATH_H

#define MAX_S32 (0x7FFFFFFF)
#define MIN_S32 (0x80000000)

#define OneOver255 (1.0f / 255.0f);
#define PI (3.1415926535897932384626433832795028841971693993751)
#define OneOverPI (1.0f / PI)
#define DegreesToRadians(degrees) (degrees) * (PI / 180.0f)
#define SqrtOf2 (1.414213562)

union v2
{
	f32 E[2];
	struct
	{
		f32 X;
		f32 Y;
	};
	struct
	{
		f32 Width;
		f32 Height;
	};
};

inline v2
V2(f32 x, f32 y)
{
	v2 result;
	result.X = x;
	result.Y = y;
	return(result);
}

inline v2
operator-(v2 a, v2 b)
{
	v2 result;
	result.X = a.X - b.X;
	result.Y = a.Y - b.Y;
	return(result);
}

inline v2
operator+(v2 a, v2 b)
{
	v2 result;
	result.X = a.X + b.X;
	result.Y = a.Y + b.Y;
	return(result);
}

inline v2&
operator+=(v2 &a, v2 b)
{
	a = a + b;
	return(a);
}

inline v2
operator*(v2 a, f32 scalar)
{
	v2 result;
	result.X = a.X * scalar;
	result.Y = a.Y * scalar;
	return(result);
}

inline v2
operator*(f32 scalar, v2 a)
{
	v2 result = a * scalar;
	return(result);
}

inline v2&
operator*=(v2 &a, f32 b)
{
	a = a * b;
	return(a);
}

//////////////////////////////// v3s

union v3s
{
	s32 E[3];
	struct
	{
		s32 X;
		s32 Y;
		s32 Z;
	};
	struct
	{
		s32 Width;
		s32 Height;
		s32 Depth;
	};
};

inline v3s
operator-(v3s a, v3s b)
{
	v3s result;
	result.X = a.X - b.X;
	result.Y = a.Y - b.Y;
	result.Z = a.Z - b.Z;
	return(result);
}

inline v3s
operator*(v3s a, f32 scalar)
{
	v3s result;
	result.X = a.X * scalar;
	result.Y = a.Y * scalar;
	result.Z = a.Z * scalar;
	return(result);
}

inline v3s
operator*(f32 scalar, v3s a)
{
	v3s result = a * scalar;
	return(result);
}

//////////////////////////////// v3

union v3
{
	f32 E[3];
	struct
	{
		f32 X;
		f32 Y;
		f32 Z;
	};
	struct
	{
		f32 Width;
		f32 Height;
		f32 Depth;
	};
	struct
	{
		v2 XY;
		f32 Padding0;
	};
	struct
	{
		f32 Padding1;
		v2 YZ;
	};
};

inline v3
V3(f32 x, f32 y, f32 z)
{
	v3 result = { x, y, z };
	return(result);
}

inline v3
V3(v2 a, f32 z = 0)
{
	v3 result = { a.X, a.Y, z };
	return(result);
}

inline v3
operator-(v3 a, v3 b)
{
	v3 result;
	result.X = a.X - b.X;
	result.Y = a.Y - b.Y;
	result.Z = a.Z - b.Z;
	return(result);
}

inline v3
operator-(v3 a)
{
	v3 result;
	result.X = -a.X;
	result.Y = -a.Y;
	result.Z = -a.Z;
	return(result);
}

inline v3&
operator-=(v3 &a, v3 b)
{
	a = a - b;
	return(a);
}

inline v3
operator+(v3 a, v3 b)
{
	v3 result;
	result.X = a.X + b.X;
	result.Y = a.Y + b.Y;
	result.Z = a.Z + b.Z;
	return(result);
}

inline v3&
operator+=(v3 &a, v3 b)
{
	a = a + b;
	return(a);
}

inline v3
operator*(v3 a, f32 scalar)
{
	v3 result;
	result.X = a.X * scalar;
	result.Y = a.Y * scalar;
	result.Z = a.Z * scalar;
	return(result);
}

inline v3
operator*(f32 scalar, v3 a)
{
	v3 result = a * scalar;
	return(result);
}

inline v3&
operator*=(v3 &a, f32 b)
{
	a = a * b;
	return(a);
}

union v4
{
	f32 E[4];
	struct
	{
		f32 X;
		f32 Y;
		f32 Z;
		f32 W;
	};
	struct
	{
		f32 R;
		f32 G;
		f32 B;
		f32 A;
	};
	struct
	{
		v2 XY;
		f32 Padding0;
		f32 padding1;
	};
	struct
	{
		f32 Padding2;
		v2 YZ;
		f32 padding3;
	};
	struct
	{
		f32 Padding4;
		f32 padding5;
		v2 ZW;
	};
	struct
	{
		v3 XYZ;
		f32 Padding6;
	};
	struct
	{
		f32 Padding7;
		v3 YZW;
	};
	
	struct
	{
		v2 RG;
		f32 Padding8;
		f32 padding9;
	};
	struct
	{
		f32 Padding10;
		v2 GB;
		f32 padding11;
	};
	struct
	{
		f32 Padding12;
		f32 padding13;
		v2 BA;
	};
	struct
	{
		v3 RGB;
		f32 Padding14;
	};
	struct
	{
		f32 Padding15;
		v3 GBA;
	};
};

inline v4
V4(v3 vector, f32 w)
{
	v4 result = { vector.X, vector.Y, vector.Z, w };
	return(result);
}

inline v4
V4(f32 x, f32 y, f32 z, f32 w)
{
	v4 result = { x, y, z, w };
	return(result);
}

inline v4
operator+(v4 &a, v4 b)
{
	v4 result = {a.X + b.X, a.Y + b.Y, a.Z + b.Z, a.W + b.W};
	return(result);
}

inline v4&
operator+=(v4 &a, v4 b)
{
	a = a + b;
	return(a);
}

inline v4
operator*(v4 a, f32 scalar)
{
	v4 result;
	result.X = a.X * scalar;
	result.Y = a.Y * scalar;
	result.Z = a.Z * scalar;
	result.W = a.W * scalar;
	return(result);
}

union m4x4
{
	f32 E[16];
	struct
	{
		v4 Row1;
		v4 Row2;
		v4 Row3;
		v4 Row4;
	};
};


struct clipped_triangle
{
	v3 Vertex0;
	v3 Vertex1;
	v3 Vertex2;
};

struct cube_bounds
{
	v3 Min;
	v3 Max;
};
#endif //_ENGINE__MATH_H
