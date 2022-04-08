/* date = April 7th 2022 8:49 pm */

#ifndef _ENGINE__INTRINSICS_H
#define _ENGINE__INTRINSICS_H

#define ShuffleMask(a, b, c, d) ((a) << 0 | (b) << 2 | (c) << 4 | (d) << 6)

struct v4_4x
{
	union __attribute((aligned(16)))
	{
		f32 E[16];
		__m128 m128_element[4];
		struct
		{
			f32 X[4];
			f32 Y[4];
			f32 Z[4];
			f32 W[4];
		};
		struct
		{
			__m128 m128_X;
			__m128 m128_Y;
			__m128 m128_Z;
			__m128 m128_W;
		};
	};
};

struct v3_4x
{
	union __attribute((aligned(16)))
	{
		f32 E[12];
		__m128 m128_element[3];
		struct
		{
			f32 X[4];
			f32 Y[4];
			f32 Z[4];
		};
	};
};

struct v2_4x
{
	union __attribute((aligned(16)))
	{
		f32 E[8];
		__m128 m128_element[2];
		struct
		{
			f32 X[4];
			f32 Y[4];
		};
		struct
		{
			__m128 m128_X;
			__m128 m128_Y;
		};
	};
};

struct m4x4
{
	union __attribute__((aligned(16)))
	{
		f32 E[16];
		__m128 m128_element[4];
		struct
		{
			v4 Row0;
			v4 Row1;
			v4 Row2;
			v4 Row3;
		};
		struct
		{
			__m128 m128_0;
			__m128 m128_1;
			__m128 m128_2;
			__m128 m128_3;
		};
	};
};

struct vertex_attribute_4x
{
	v4_4x Vertices;
	v4_4x Normals;
	v4_4x UVs;
	v4_4x Tangents;
	v4_4x Bitangents;
};

#endif //_ENGINE__INTRINSICS_H
