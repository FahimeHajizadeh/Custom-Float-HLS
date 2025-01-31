#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>
#include <cstdarg>
#include <cassert>
#include <cmath>
#include <random>
#include "process.h"
#include "custom_float.h"

float get_random()
{
	ap_uint<32> fp;
	// Dividing by RAND_MAX normalizes the value to the range [0, 1].
	// Multiplying by pow(2, 23) scales the normalized value to fit the 23 bits of the mantissa.
	fp.range(22, 0)  = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * pow(2, 23);

	// (127 - 16) = 111 ensures the exponent is shifted toward smaller numbers.
	//The second part adds a random value in the range [0, 256], ensuring that the exponent varies across a wide range of numbers.
//	fp.range(30, 23) = (127-32) + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * pow(2, 6));
	fp.range(30, 23) = (127-16) + ((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * pow(2, 8));

	// rand() & 1 randomly selects 0 or 1.
	fp[31] = rand() & 1;


	TransferF t;
	t.i = fp.to_uint();
	return t.f;
}

void print_log(float x1, float x2, float z1, float z2, float er, bool sm)
{
	TransferF t1, t2, t3, t4;
	t1.f = x1;
	t2.f = x2;
	t3.f = z1;
	t4.f = z2;
	if (sm) {
		printf("SUM: fp1 = %-30.15f (%x) | fp2 = %-30.15f (%x) | fp = %-30.15f (%x), saf = %-30.15f (%x) | er. = %-30.15f\n", x1, t1.i, x2, t2.i, z1, t3.i, z2, t4.i, er);
	} else {
		printf("MUL: fp1 = %-30.15f (%x) | fp2 = %-30.15f (%x) | fp = %-30.15f (%x), saf = %-30.15f (%x) | er. = %-30.15f\n", x1, t1.i, x2, t2.i, z1, t3.i, z2, t4.i, er);
	}
}

int main()
{

//	srand(static_cast<unsigned>(time(0)));

	// -------------- Random Engine Initialization
	std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(-10000000, 10000000);

#if defined(SUM) or defined(MUL)

//	float rel_err_sum = 0;
//	float rel_err_mul = 0;

	double rel_err_sum = 0;
	double rel_err_mul = 0;

	double sum_ref_sum = 0;
	double sum_ref_mul = 0;

	for (int i = 0; i < TEST_COUNT; i++) {

//		float fp1 = get_random();
//		float fp2 = get_random();
		double db1 = (1.0 * (long long)dist(rng)) / (long long)dist(rng);  // get_random();
		double db2 = (1.0 * (long long)dist(rng)) / (long long)dist(rng); // get_random();
		float fp1 = db1;
		float fp2 = db2;
		float fp3 = fp1 + fp2;
		float fp4 = fp1 * fp2;
		double db3 = db1 + db2;
		double db4 = db1 * db2;

		CuFPSAF_t saf1(fp1);
		CuFPSAF_t saf2(fp2);
		CuFPSAF_t saf3 = CuFl::sum<SAF_WM, SAF_WE, SAF_L, SAF_B>(saf1, saf2);
		CuFPSAF_t saf4 = CuFl::mul<SAF_WM, SAF_WE, SAF_L, SAF_B>(saf1, saf2);

//		float err_sum = abs(saf3.to_float() - fp3) / abs(fp3);
//		float err_mul = abs(saf4.to_float() - fp4) / abs(fp4);

		sum_ref_sum += pow(fp3, 2);
		sum_ref_mul += pow(fp4, 2);

		double err_sum = pow(saf3.to_float() - fp3, 2);
		double err_mul = pow(saf4.to_float() - fp4, 2);
		rel_err_sum += err_sum;
		rel_err_mul += err_mul;

//		if (err_sum > 0.001) {
//			print_log(fp1, fp2, fp3, saf3.to_float(), err_sum, 1);
//		}
//
//		if (err_mul > 0.001) {
//			print_log(fp1, fp2, fp4, saf4.to_float(), err_mul, 0);
//		}
	}

	printf("Er. SUM = %30.15f | Er. Mul =  %30.15f\n", sqrt(rel_err_sum / sum_ref_sum), sqrt(rel_err_mul / sum_ref_mul));

#elif defined(VSUM)

	float vfp[VSIZE];
	CuFPSAF_t vsaf[VSIZE];
	float sum_fp = 0;
	for (int i = 0; i < VSIZE; i++) {
		const float rnd = get_random();
		sum_fp += rnd;
		vfp[i] = rnd;
		vsaf[i] = CuFPSAF_t(rnd);
//		printf("FP[%d]:  %30.15f\n", i, rnd);
	}

	CuFPSAF_t sum_saf = process_vector(vsaf);

	printf("SUM FP:  %30.15f\n", sum_fp);
	printf("SUM SAF: %30.15f\n", sum_saf.to_float());

#elif defined(DP)

	float vfp1[VSIZE];
	float vfp2[VSIZE];
	CuFPSAF_t vsaf1[VSIZE];
	CuFPSAF_t vsaf2[VSIZE];
	float dp_fp = 0;
	for (int i = 0; i < VSIZE; i++) {
		const float rnd1 = get_random();
		const float rnd2 = get_random();
		dp_fp += rnd1 * rnd2;
		vfp1[i] = rnd1;
		vfp2[i] = rnd2;
		vsaf1[i] = CuFPSAF_t(rnd1);
		vsaf2[i] = CuFPSAF_t(rnd2);
//		printf("FP1[%d]:  %30.15f\n", i, rnd1);
//		printf("FP2[%d]:  %30.15f\n", i, rnd2);
	}

	CuFPSAF_t dp_saf = CuFl::dp<SAF_WM, SAF_WE, SAF_L, SAF_B, VSIZE>(vsaf1, vsaf2);

	printf("SUM FP:  %30.15f\n", dp_fp);
	printf("SUM SAF: %30.15f\n", dp_saf.to_float());

#else
	float fp1 = -5.991881847381592;
	float fp2 = 4.323617935180664;
	float fp3 = fp1 + fp2;
	float fp4 = fp1 * fp2;
	CuFPSAF_t saf1(fp1);
	CuFPSAF_t saf2(fp2);
	CuFPSAF_t saf3 = CuFl::sum<SAF_WM, SAF_WE, SAF_L, SAF_B>(saf1, saf2);
	CuFPSAF_t saf4 = CuFl::mul<SAF_WM, SAF_WE, SAF_L, SAF_B>(saf1, saf2);

	printf("fp1  : %8.16f +* fp2  : %8.16f = %8.16f  %8.16f\n", fp1, fp2, fp3, fp4);
	printf("saf1 : %8.16f +* saf2 : %8.16f = %8.16f  %8.16f\n", saf1.to_float(), saf2.to_float(), saf3.to_float(), saf4.to_float());
#endif

    printf("Simulation completed successfully!\n");

    return 0;
}
