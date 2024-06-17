#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>

// Define which data type is used (useful only for getting synthesis result)
// -------------------------------------------------------
#ifdef __SYNTHESIS__
#define CUSTOM_FLOAT
// #define NORMAL_FLOAT
// #define NORMAL_DOUBLE
#else
#define CUSTOM_FLOAT
#endif
// -------------------------------------------------------

// Choose top function
// -------------------------------------------------------
#define SINGLE_OP_SUM
// #define SINGLE_OP_MUL
// #define VECTOR_OP_VSUM
// #define DOT_PRODUCT
// #define MVM
// #define MATMUL
// -------------------------------------------------------

#if defined(VECTOR_OP_VSUM) or defined(DOT_PRODUCT) or defined(MVM) or defined(MATMUL)
#define NN 8
#endif

#if defined(CUSTOM_FLOAT)
const int W1=32;
const int M1=23;
const int W2=32;
const int M2=23;
const int WR=32;
const int MR=23;
#endif

#if defined(NORMAL_DOUBLE)
typedef double f_type;
#else
typedef float f_type;
#endif


#define __NO_DEBUG__

#if not defined(__SYNTHESIS__) and not defined(__NO_DEBUG__)
#define __ENABLE_PRINT__
#endif

#endif // CONFIG_H
