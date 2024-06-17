#include "run_operations.h"

// ******************** Custom Float ********************
#if defined(CUSTOM_FLOAT)
using namespace CuFl;

// Matrix Multiplication
#if defined(MATMUL)
void run_experiment(const CuFl::CustomFloat<WR, MR> x[NN][NN], const CuFl::CustomFloat<WR, MR> y[NN][NN], CuFl::CustomFloat<WR, MR> z[NN][NN])
{
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=x complete dim=0)
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=y complete dim=0)
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=z complete dim=0)
	PRAGMA_HLS(HLS inline recursive)
	PRAGMA_HLS(HLS PIPELINE II=1)

	CuFl::matmul<WR, MR, NN>(x, y, z);
}

// MVM
#elif defined(MVM)
void run_experiment(const CustomFloat<WR, MR> x[NN][NN], const CustomFloat<WR, MR> y[NN], CustomFloat<WR, MR> z[NN])
{
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=x complete dim=0)
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=y complete dim=0)
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=z complete dim=0)
	PRAGMA_HLS(HLS inline recursive)
	PRAGMA_HLS(HLS PIPELINE II=1)

	CuFl::mvm<WR, MR, NN>(x, y, z);
}

// Dot Product
#elif defined(DOT_PRODUCT)
CuFl::CustomFloat<WR, MR> run_experiment(const CuFl::CustomFloat<WR, MR> a[NN], const CuFl::CustomFloat<WR, MR> x[NN])
{
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=a complete dim=0)
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=x complete dim=0)
	PRAGMA_HLS(HLS inline recursive)
	PRAGMA_HLS(HLS PIPELINE II=1)

	return CuFl::dp<WR, MR, NN>(a, x);
}

// Vector Sum
#elif defined(VECTOR_OP_VSUM)
CuFl::CustomFloat<WR, MR> run_experiment(const CuFl::CustomFloat<WR, MR> x[NN])
{
	PRAGMA_HLS(HLS inline recursive)
	PRAGMA_HLS(HLS PIPELINE II=1)

	return CuFl::vsum<WR, MR, NN>(x);
}

#else
// Single operator
CustomFloat<WR, MR> run_experiment(CustomFloat<W1, M1> x, CustomFloat<W2, M2> y)
{
	PRAGMA_HLS(HLS PIPELINE II=1)
#if defined(SINGLE_OP_SUM)
	return CuFl::sum<WR, MR>(x, y);
#elif defined(SINGLE_OP_MUL)
	return CuFl::mul<WR, MR>(x, y);
#else
	return 0;
#endif
}
#endif


// *********************** Float ***********************
#elif defined(NORMAL_FLOAT) or defined (NORMAL_DOUBLE)
using namespace NoFl;

// Matrix Multiplication
#if defined(MATMUL)
void run_experiment(const f_type x[NN][NN], const f_type y[NN][NN], f_type z[NN][NN])
{
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=x complete dim=0)
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=y complete dim=0)
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=z complete dim=0)
	PRAGMA_HLS(HLS inline recursive)
	PRAGMA_HLS(HLS PIPELINE II=1)

	NoFl::matmul<NN>(x, y, z);
}

// MVM
#elif defined(MVM)
void run_experiment(const f_type x[NN][NN], const f_type y[NN], f_type z[NN])
{
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=x complete dim=0)
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=y complete dim=0)
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=z complete dim=0)
	PRAGMA_HLS(HLS inline recursive)
	PRAGMA_HLS(HLS PIPELINE II=1)

	NoFl::mvm<NN>(x, y, z);
}

// Dot Product
#elif defined(DOT_PRODUCT)
f_type run_experiment(const f_type a[NN], const f_type x[NN])
{
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=a complete dim=0)
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=x complete dim=0)
	PRAGMA_HLS(HLS inline recursive)
	PRAGMA_HLS(HLS PIPELINE II=1)

	return NoFl::dp<NN>(a, x);
}

// Vector Sum
#elif defined(VECTOR_OP_VSUM)
f_type run_experiment(const f_type x[NN])
{
	PRAGMA_HLS(HLS ARRAY_PARTITION  variable=x complete dim=0)
	PRAGMA_HLS(HLS inline recursive)
	PRAGMA_HLS(HLS PIPELINE II=1)

	return NoFl::vsum<NN>(x);
}

#else
// Single operator
f_type run_experiment(f_type x, f_type y)
{
PRAGMA_HLS(HLS PIPELINE II=1)
#if defined(SINGLE_OP_SUM)
	return NoFl::sum(x, y);
#elif defined(SINGLE_OP_MUL)
	return NoFl::mul(x, y);
#else
	return 0;
#endif
}

#endif

#endif

