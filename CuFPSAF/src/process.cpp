#include "process.h"


CuFPSAF_t vsum_fold(const CuFPSAF_t x[KFOLD])
{
#pragma HLS PIPELINE
	return CuFl::vsum<SAF_WM, SAF_WE, SAF_L, SAF_B, KFOLD>(x);
}

CuFPSAF_t dp_fold(const CuFPSAF_t x[KFOLD], const CuFPSAF_t y[KFOLD])
{
#pragma HLS PIPELINE
	return CuFl::dp<SAF_WM, SAF_WE, SAF_L, SAF_B, KFOLD>(x, y);
}

#if defined(SUM)
CuFPSAF_t process_vector(const CuFPSAF_t x, const CuFPSAF_t y)
{
#pragma HLS PIPELINE II=1
	return CuFl::sum<SAF_WM, SAF_WE, SAF_L, SAF_B>(x, y);
}

#elif defined(MUL)
CuFPSAF_t process_vector(const CuFPSAF_t x, const CuFPSAF_t y)
{
#pragma HLS PIPELINE II=1
	return CuFl::mul<SAF_WM, SAF_WE, SAF_L, SAF_B>(x, y);
}

#elif defined(VSUM)
CuFPSAF_t process_vector(const CuFPSAF_t x[VSIZE])
{
#if defined(C_LATENCY)
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=x type=complete
	return CuFl::vsum<SAF_WM, SAF_WE, SAF_L, SAF_B, VSIZE>(x);

#elif defined(C_RESOURCE)
#pragma HLS PIPELINE
#pragma HLS ALLOCATION function instances=vsum_fold limit=1
#pragma HLS ARRAY_PARTITION variable=x type=complete
	CuFPSAF_t sum(0.0);
	for (int i = 0; i < VSIZE; i += KFOLD) {
#pragma HLS UNROLL factor=VSIZE/KFOLD
		sum = CuFl::sum<SAF_WM, SAF_WE, SAF_L, SAF_B>(sum, vsum_fold(&x[i]));
	}
	return sum;
#endif
}

#elif defined(DP)
CuFPSAF_t process_vector(const CuFPSAF_t x[VSIZE], const CuFPSAF_t y[VSIZE])
{
#if defined(C_LATENCY)
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=x type=complete
#pragma HLS ARRAY_PARTITION variable=y type=complete
	return CuFl::dp<SAF_WM, SAF_WE, SAF_L, SAF_B, VSIZE>(x, y);

#elif defined(C_RESOURCE)
#pragma HLS PIPELINE
#pragma HLS ALLOCATION function instances=dp_fold limit=1
#pragma HLS ARRAY_PARTITION variable=x type=complete
#pragma HLS ARRAY_PARTITION variable=y type=complete
	CuFPSAF_t sum(0.0);
	for (int i = 0; i < VSIZE; i += KFOLD) {
#pragma HLS UNROLL factor=VSIZE/KFOLD
		sum = CuFl::sum<SAF_WM, SAF_WE, SAF_L, SAF_B>(sum, dp_fold(&x[i], &y[i]));
	}
	return sum;
#endif
}
#endif
