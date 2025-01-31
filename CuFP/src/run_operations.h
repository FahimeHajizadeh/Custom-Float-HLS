#ifndef RUN_OPERATIONS_H
#define RUN_OPERATIONS_H

#include "custom_float.h"
#include "normal_float.h"
#include "config.h"

// ******************** Custom Float ********************
#if defined(CUSTOM_FLOAT)
// Matrix Multiplication
#if defined(MATMUL)
void run_experiment(const CuFl::CustomFloat<WR, MR> x[NN][NN], const CuFl::CustomFloat<WR, MR> y[NN][NN], CuFl::CustomFloat<WR, MR> z[NN][NN]);
// MVM
#elif defined(MVM)
void run_experiment(const CuFl::CustomFloat<WR, MR> x[NN][NN], const CuFl::CustomFloat<WR, MR> y[NN], CuFl::CustomFloat<WR, MR> z[NN]);
// Dot Product
#elif defined(DOT_PRODUCT)
CuFl::CustomFloat<WR, MR> run_experiment(const CuFl::CustomFloat<WR, MR> a[NN], const CuFl::CustomFloat<WR, MR> x[NN]);
// Vector Sum (N elements)
#elif defined(VECTOR_OP_VSUM)
CuFl::CustomFloat<WR, MR> run_experiment(const CuFl::CustomFloat<WR, MR> x[NN]);
// Single operator
#else
CuFl::CustomFloat<WR, MR> run_experiment(CuFl::CustomFloat<W1, M1> x, CuFl::CustomFloat<W2, M2> y);
#endif


// *********************** Float or Double ***********************
#elif defined(NORMAL_FLOAT) or defined(NORMAL_DOUBLE)
// Matrix Multiplication
#if defined(MATMUL)
void run_experiment(const f_type x[NN][NN], const f_type y[NN][NN], f_type z[NN][NN]);
// MVM
#elif defined(MVM)
void run_experiment(const f_type x[NN][NN], const f_type y[NN], f_type z[NN]);
// Dot Product
#elif defined(DOT_PRODUCT)
f_type run_experiment(const f_type a[NN], const f_type x[NN]);
// Vector Sum (generic elements)
#elif defined(VECTOR_OP_VSUM)
f_type run_experiment(const f_type x[NN]);
// Single operator
#else
f_type run_experiment(f_type x, f_type y);
#endif


#endif

#endif // RUN_OPERATIONS_H



