
inline void
Math_DotProduct(__m128 x0, __m128 y0, __m128 z0, __m128 w0,
				__m128 x1, __m128 y1, __m128 z1, __m128 w1,
				__m128* out)
{
	__m128 dx = _mm_mul_ps(x0, x1);
	__m128 dy = _mm_mul_ps(y0, y1);
	__m128 dz = _mm_mul_ps(z0, z1);
	__m128 dw = _mm_mul_ps(w0, w1);
	
	__m128 a0 = _mm_add_ps(dx, dy);
	__m128 a1 = _mm_add_ps(dz, dw);
	*out = _mm_add_ps(a0, a1);
}

inline void
Math_DotProduct(v4_4x* a, v4_4x* b, __m128* out)
{
	__m128 dx = _mm_mul_ps(a->m128_X, b->m128_X);
	__m128 dy = _mm_mul_ps(a->m128_Y, b->m128_Y);
	__m128 dz = _mm_mul_ps(a->m128_Z, b->m128_Z);
	__m128 dw = _mm_mul_ps(a->m128_W, b->m128_W);
	
	__m128 a0 = _mm_add_ps(dx, dy);
	__m128 a1 = _mm_add_ps(dz, dw);
	*out = _mm_add_ps(a0, a1);
}

//////////////////////////////// m4x4

inline void
Math_MatrixMultiply(m4x4* a, m4x4* b, m4x4* out)
{
	__m128 m00 = _mm_set1_ps(a->Row0.X); __m128 m01 = _mm_set1_ps(a->Row0.Y);
	__m128 m02 = _mm_set1_ps(a->Row0.Z); __m128 m03 = _mm_set1_ps(a->Row0.W);
	
	__m128 m04 = _mm_set1_ps(a->Row1.X); __m128 m05 = _mm_set1_ps(a->Row1.Y);
	__m128 m06 = _mm_set1_ps(a->Row1.Z); __m128 m07 = _mm_set1_ps(a->Row1.W);
	
	__m128 m08 = _mm_set1_ps(a->Row2.X); __m128 m09 = _mm_set1_ps(a->Row2.Y);
	__m128 m10 = _mm_set1_ps(a->Row2.Z); __m128 m11 = _mm_set1_ps(a->Row2.W);
	
	__m128 m12 = _mm_set1_ps(a->Row3.X); __m128 m13 = _mm_set1_ps(a->Row3.Y);
	__m128 m14 = _mm_set1_ps(a->Row3.Z); __m128 m15 = _mm_set1_ps(a->Row3.W);
	
	Math_DotProduct(m00, m01, m02, m03, b->m128_0, b->m128_1, b->m128_2, b->m128_3, &out->m128_0);
	Math_DotProduct(m04, m05, m06, m07, b->m128_0, b->m128_1, b->m128_2, b->m128_3, &out->m128_1);
	Math_DotProduct(m08, m09, m10, m11, b->m128_0, b->m128_1, b->m128_2, b->m128_3, &out->m128_2);
	Math_DotProduct(m12, m13, m14, m15, b->m128_0, b->m128_1, b->m128_2, b->m128_3, &out->m128_3);
}

inline void
Math_MatrixMultiply(m4x4* a, v4_4x* b, v4_4x* out)
{
	Math_MatrixMultiply(a, (m4x4*)b, (m4x4*)out);
}
