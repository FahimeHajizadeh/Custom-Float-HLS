#ifndef NORMAL_FLOAT_H
#define NORMAL_FLOAT_H

#include <iostream>
#include "config.h"
#include "array_utils.h"

// Macros to parametrize the pragmas
#define PRAGMA_SUB(x) _Pragma (#x)
#define PRAGMA_HLS(x) PRAGMA_SUB(x)

namespace NoFl {

// Vector Sum (generic elements)
template <int N>
f_type vsum(const f_type x[N])
{
	PRAGMA_HLS(HLS PIPELINE II=1)

	return array_sum<f_type, N>(x);
}

// Dot Product
template <int N>
f_type dp(const f_type a[N], const f_type x[N])
{
	PRAGMA_HLS(HLS inline recursive)
	PRAGMA_HLS(HLS PIPELINE II=1)

	f_type v[N];
	for (int i = 0; i < N; i ++) {
		PRAGMA_HLS(HLS unroll factor=N)
		v[i] = a[i] * x[i];
	}
	return vsum<N>(v);
}

// MVM
template <int N>
void mvm(const f_type x[N][N], const f_type y[N], f_type z[N])
{
	PRAGMA_HLS(HLS inline recursive)
	PRAGMA_HLS(HLS PIPELINE II=1)

	for (int i = 0; i < N; i++) {
		PRAGMA_HLS(HLS unroll factor=N)
		z[i] = dp<N>(&x[i][0], y);
	}
}

// Matrix Multiplication
template <int N>
void matmul(const f_type x[N][N], const f_type y[N][N], f_type z[N][N])
{
	PRAGMA_HLS(HLS inline recursive)
	PRAGMA_HLS(HLS PIPELINE II=1)
	for (int i = 0; i < N; i++) {
		PRAGMA_HLS(HLS PIPELINE II=1)
		for (int j = 0; j < N; j++) {
			PRAGMA_HLS(HLS unroll factor=N)
		    f_type vy[N];
		    for (int k = 0; k < N; k ++) {
		    	PRAGMA_HLS(HLS unroll factor=N)
		    	vy[k] = y[k][j];
		    }
			z[i][j] = dp<N>(&x[i][0], vy);
		}
	}
}

// Multiplier
inline f_type mul(f_type x, f_type y)
{
	return x * y;
}

// Sum
inline f_type sum(const f_type x, const f_type y)
{
	return x + y;
}

}

#endif // NORMAL_FLOAT_H
