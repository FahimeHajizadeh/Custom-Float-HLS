#ifndef PROCESS_H
#define PROCESS_H

#include <ap_int.h>
#include "config.h"
#include "custom_float.h"

using namespace CuFl;
using CuFPSAF_t = CuFl::CuFPSAF<SAF_WM, SAF_WE, SAF_L, SAF_B>;

#if defined(SUM) or defined(MUL)
CuFPSAF_t process_vector(const CuFPSAF_t x, const CuFPSAF_t y);
#elif defined(VSUM)
CuFPSAF_t process_vector(const CuFPSAF_t x[VSIZE]);
#elif defined(DP)
CuFPSAF_t process_vector(const CuFPSAF_t x[VSIZE], const CuFPSAF_t y[VSIZE]);
#endif


#endif // PROCESS_H
