#include "run_operations.h"
#include <iostream>
#include <random>

#if defined(MATMUL)

void matmul_float(const float x[NN][NN], const float y[NN][NN], float z[NN][NN])
{
    for (int i = 0; i < NN; i++) {
    	for (int j = 0; j < NN; j++) {
    		float s = 0;
    		for (int k = 0; k < NN; k++) {
    			s += x[i][k] * y[k][j];
    		}
    		z[i][j] = s;
    	}
    }
}

void matmul_double(const double x[NN][NN], const double y[NN][NN], double z[NN][NN])
{
    for (int i = 0; i < NN; i++) {
    	for (int j = 0; j < NN; j++) {
    		double s = 0;
    		for (int k = 0; k < NN; k++) {
    			s += x[i][k] * y[k][j];
    		}
    		z[i][j] = s;
    	}
    }
}

#elif defined(MVM)

void mvm_float(const float x[NN][NN], const float y[NN], float z[NN])
{
	for (int i = 0; i < NN; i++) {
		z[i] = 0;
		for (int j = 0; j < NN; j ++) {
			z[i] += x[i][j] * y[j];
		}
	}
}

void mvm_double(const double x[NN][NN], const double y[NN], double z[NN])
{
	for (int i = 0; i < NN; i++) {
		z[i] = 0;
		for (int j = 0; j < NN; j ++) {
			z[i] += x[i][j] * y[j];
		}
	}
}

#elif defined(DOT_PRODUCT)

float dp_float(const float a[NN], const float x[NN])
{
	float dp = 0;
	for (int i = 0; i < NN; i++) {
		dp += a[i] * x[i];
	}
	return dp;
}

double dp_double(const double a[NN], const double x[NN])
{
	double dp = 0;
	for (int i = 0; i < NN; i++) {
		dp += a[i] * x[i];
	}
	return dp;
}

#elif defined(VECTOR_OP_VSUM)

float vsum_float(const float x[NN])
{
	float sum = 0;
	for (int i = 0; i < NN; i++) {
		sum += x[i];
	}
	return sum;
}

double vsum_double(const double x[NN])
{
	double sum = 0;
	for (int i = 0; i < NN; i++) {
		sum += x[i];
	}
	return sum;
}

#endif

int main()
{
    // -----------------------------------------
	// --------- Start simulation loop ---------
	// -----------------------------------------

	// -------------- Random Engine Initialization
	std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(-10000000, 10000000);

#if defined(SINGLE_OP_SUM) or defined(SINGLE_OP_MUL)
    // ---------------------------- Single Operation Test ----------------------------
    double err_sum_cf2 = 0;
    double err_sum_f2 = 0;
    double ref_sum_d2 = 0;

    test_loop: for (int i = 0; i < 1; i++) {
    	double d1 = (1.0 * (long long)dist(rng)) / (long long)dist(rng);
    	double d2 = (1.0 * (long long)dist(rng)) / (long long)dist(rng);
    	float f1 = d1;
    	float f2 = d2;
    	CuFl::CustomFloat<W1, M1> cf1(d1);
    	CuFl::CustomFloat<W2, M2> cf2(d2);
    	CuFl::CustomFloat<WR, MR> cf3 = run_experiment(cf1, cf2);

#if defined(SINGLE_OP_SUM)
    	double res_d = d1 + d2;
    	float res_f = f1 + f2;
#elif defined(SINGLE_OP_MUL)
        double res_d = d1 * d2;
        float res_f = f1 * f2;
#endif
        float res_cf = cf3.getDouble();

        double err_abs_cf = fabs(res_d - res_cf);
        double err_rel_cf = err_abs_cf / fabs(res_d);

        double err_abs_f = fabs(res_d - res_f);
        double err_rel_f = err_abs_f / fabs(res_d);

#if defined(__ENABLE_PRINT__)
        printf("X: %+5.16f (0x%08X) | Y: %+5.16f (0x%08X)\n"
               "Double   : %+5.10f\n"
               "Float    : %+5.10f (0x%08X) | ErrAbs: %5.10f | ErrRel: %5.10f\n"
               "Cus.Float: %+5.10f (0x%08X) | ErrAbs: %5.10f | ErrRel: %5.10f\n\n",
               d1, *(int *)&f1, d2, *(int *)&f2,
			   res_d,
			   res_f,  *(int *)&res_f,  err_abs_f,  err_rel_f,
			   res_cf, *(int *)&res_cf, err_abs_cf, err_rel_cf);
#endif
        err_sum_cf2 += err_abs_cf * err_abs_cf;
        err_sum_f2 += err_abs_f * err_abs_f;
        ref_sum_d2 += res_d * res_d;
    }

    double err_norm2_f = sqrt(err_sum_f2 / ref_sum_d2);
    printf("\nNorm2 Error Float    : %5.16f\n", err_norm2_f);
    double err_norm2_cf = sqrt(err_sum_cf2 / ref_sum_d2);
    printf("Norm2 Error Cus.Float: %5.16f\n", err_norm2_cf);

#elif defined(VECTOR_OP_VSUM)
    // ---------------------------- VSUM Operation Test ----------------------------
    double err_sum_cf2 = 0;
    double err_sum_f2 = 0;
    double ref_sum_d2 = 0;

    test_lopp: for (int i = 0; i < 1; i++) {
    	double vd[NN];
    	float vf[NN];
    	CuFl::CustomFloat<WR, MR> vcf[NN];
    	for (int j = 0; j < NN; j++) {
    		vd[j] = (1.0 * (long long)dist(rng)) / (long long)dist(rng);
    		vf[j] = vd[j];
    		vcf[j] = vd[j];
    	}

    	double sum_d = vsum_double(vd);
    	float sum_f  = vsum_float(vf);
    	float sum_cf = run_experiment(vcf).getDouble();

        double err_abs_cf = fabs(sum_d - sum_cf);
        double err_rel_cf = err_abs_cf / fabs(sum_d);

        double err_abs_f = fabs(sum_d - sum_f);
        double err_rel_f = err_abs_f / fabs(sum_d);

#if defined(__ENABLE_PRINT__)
        printf("X1: %+5.16f (0x%08X) | X2: %+5.16f (0x%08X) | X3: %+5.16f (0x%08X) | X4: %+5.16f (0x%08X)\n"
               "Double   : %+5.10f\n"
               "Float    : %+5.10f (0x%08X) | ErrAbs: %5.10f | ErrRel: %5.10f\n"
               "Cus.Float: %+5.10f (0x%08X) | ErrAbs: %5.10f | ErrRel: %5.10f\n\n",
               d1, *(int *)&f1, d2, *(int *)&f2, d3, *(int *)&f3, d4, *(int *)&f4,
			   sum_d,
			   sum_f,  *(int *)&sum_f,  err_abs_f,  err_rel_f,
			   sum_cf, *(int *)&sum_cf, err_abs_cf, err_rel_cf);
#endif
        err_sum_cf2 += err_abs_cf * err_abs_cf;
        err_sum_f2 += err_abs_f * err_abs_f;
        ref_sum_d2 += sum_d * sum_d;
    }

    double err_norm2_f = sqrt(err_sum_f2 / ref_sum_d2);
    printf("\nNorm2 Error Float    : %5.16f\n", err_norm2_f);
    double err_norm2_cf = sqrt(err_sum_cf2 / ref_sum_d2);
    printf("Norm2 Error Cus.Float: %5.16f\n", err_norm2_cf);

#elif defined(DOT_PRODUCT)
    // ---------------------------- Dot Product Operation Test ----------------------------
    double err_sum_cf2 = 0;
    double err_sum_f2 = 0;
    double ref_sum_d2 = 0;

    test_lopp: for (int i = 0; i < 1; i++) {
    	double vd0[NN];
    	float vf0[NN];
    	CuFl::CustomFloat<WR, MR> vcf0[NN];
    	double vd1[NN];
    	float vf1[NN];
    	CuFl::CustomFloat<WR, MR> vcf1[NN];
    	for (int j = 0; j < NN; j++) {
    		vd0[j] = (1.0 * (long long)dist(rng)) / (long long)dist(rng);
    		vf0[j] = vd0[j];
    		vcf0[j] = vd0[j];

    		vd1[j] = (1.0 * (long long)dist(rng)) / (long long)dist(rng);
    		vf1[j] = vd1[j];
    		vcf1[j] = vd1[j];
    	}

    	double sum_d = dp_double(vd0, vd1);
    	float sum_f  = dp_float(vf0, vf1);
    	float sum_cf = run_experiment(vcf0, vcf1).getDouble();

        double err_abs_cf = fabs(sum_d - sum_cf);
        double err_rel_cf = err_abs_cf / fabs(sum_d);

        double err_abs_f = fabs(sum_d - sum_f);
        double err_rel_f = err_abs_f / fabs(sum_d);

#if defined(__ENABLE_PRINT__)
        printf("X1: %+5.16f (0x%08X) | X2: %+5.16f (0x%08X) | X3: %+5.16f (0x%08X) | X4: %+5.16f (0x%08X)\n"
               "Double   : %+5.10f\n"
               "Float    : %+5.10f (0x%08X) | ErrAbs: %5.10f | ErrRel: %5.10f\n"
               "Cus.Float: %+5.10f (0x%08X) | ErrAbs: %5.10f | ErrRel: %5.10f\n\n",
               d1, *(int *)&f1, d2, *(int *)&f2, d3, *(int *)&f3, d4, *(int *)&f4,
			   sum_d,
			   sum_f,  *(int *)&sum_f,  err_abs_f,  err_rel_f,
			   sum_cf, *(int *)&sum_cf, err_abs_cf, err_rel_cf);
#endif
        err_sum_cf2 += err_abs_cf * err_abs_cf;
        err_sum_f2 += err_abs_f * err_abs_f;
        ref_sum_d2 += sum_d * sum_d;
    }

    double err_norm2_f = sqrt(err_sum_f2 / ref_sum_d2);
    printf("\nNorm2 Error Float    : %5.16f\n", err_norm2_f);
    double err_norm2_cf = sqrt(err_sum_cf2 / ref_sum_d2);
    printf("Norm2 Error Cus.Float: %5.16f\n", err_norm2_cf);

#elif defined(MVM)
    // ---------------------------- MVM Operation Test ----------------------------
    double err_sum_cf2 = 0;
    double err_sum_f2 = 0;
    double ref_sum_d2 = 0;

    test_lopp: for (int i = 0; i < 1; i++) {
    	double vd_m[NN][NN];
    	double vd_v[NN];
    	double vd_z[NN];

    	float vf_m[NN][NN];
    	float vf_v[NN];
    	float vf_z[NN];

    	CuFl::CustomFloat<WR, MR> vcf_m[NN][NN];
    	CuFl::CustomFloat<WR, MR> vcf_v[NN];
    	CuFl::CustomFloat<WR, MR> vcf_z[NN];

    	for (int j = 0; j < NN; j++) {
    		for (int k = 0; k < NN; k++) {
    			vd_m[j][k] = (1.0 * (long long)dist(rng)) / (long long)dist(rng);
    			vf_m[j][k] = vd_m[j][k];
    			vcf_m[j][k] = vd_m[j][k];
    		}
    	}
    	for (int j = 0; j < NN; j++) {
    		vd_v[j] = (1.0 * (long long)dist(rng)) / (long long)dist(rng);
    		vf_v[j] = vd_v[j];
    		vcf_v[j] = vd_v[j];
    	}

    	mvm_double(vd_m, vd_v, vd_z);
    	mvm_float(vf_m, vf_v, vf_z);
    	run_experiment(vcf_m, vcf_v, vcf_z);

    	for (int j = 0; j < NN; j++) {
    		double sum_d = vd_z[j];
    		float sum_f = vf_z[j];
    		float sum_cf = vcf_z[j].getDouble();

    		double err_abs_cf = fabs(sum_d - sum_cf);
    		double err_rel_cf = err_abs_cf / fabs(sum_d);

    		double err_abs_f = fabs(sum_d - sum_f);
    		double err_rel_f = err_abs_f / fabs(sum_d);

#if defined(__ENABLE_PRINT__)
    		printf("X1: %+5.16f (0x%08X) | X2: %+5.16f (0x%08X) | X3: %+5.16f (0x%08X) | X4: %+5.16f (0x%08X)\n"
    				"Double   : %+5.10f\n"
    				"Float    : %+5.10f (0x%08X) | ErrAbs: %5.10f | ErrRel: %5.10f\n"
    				"Cus.Float: %+5.10f (0x%08X) | ErrAbs: %5.10f | ErrRel: %5.10f\n\n",
					d1, *(int *)&f1, d2, *(int *)&f2, d3, *(int *)&f3, d4, *(int *)&f4,
					sum_d,
					sum_f,  *(int *)&sum_f,  err_abs_f,  err_rel_f,
					sum_cf, *(int *)&sum_cf, err_abs_cf, err_rel_cf);
#endif
    		err_sum_cf2 += err_abs_cf * err_abs_cf;
    		err_sum_f2 += err_abs_f * err_abs_f;
    		ref_sum_d2 += sum_d * sum_d;
    	}
    }

    double err_norm2_f = sqrt(err_sum_f2 / ref_sum_d2);
    printf("\nNorm2 Error Float    : %5.16f\n", err_norm2_f);
    double err_norm2_cf = sqrt(err_sum_cf2 / ref_sum_d2);
    printf("Norm2 Error Cus.Float: %5.16f\n", err_norm2_cf);

#elif defined(MATMUL)
    // ---------------------------- Matrix Multiplication Test ----------------------------
    double md1[NN][NN] = {0};
    double md2[NN][NN] = {0};
    double md3[NN][NN] = {0};
    float mf1[NN][NN] = {0};
    float mf2[NN][NN] = {0};
    float mf3[NN][NN] = {0};
    CuFl::CustomFloat<WR, MR> mcf1[NN][NN];
    CuFl::CustomFloat<WR, MR> mcf2[NN][NN];
    CuFl::CustomFloat<WR, MR> mcf3[NN][NN];
    for (int i = 0; i < NN; i++) {
    	for (int j = 0; j < NN; j++) {
    		md1[i][j] = (1.0 * (long long)dist(rng)) / (long long)dist(rng);
    		mf1[i][j] = md1[i][j];
    		mcf1[i][j] = md1[i][j];

    		md2[i][j] = (1.0 * (long long)dist(rng)) / (long long)dist(rng);
    		mf2[i][j] = md2[i][j];
    		mcf2[i][j] = md2[i][j];

    		md3[i][j] = 0.0;
    		mf3[i][j] = 0.0;
    		mcf3[i][j] = 0.0;
    	}
    }

    matmul_double(md1, md2, md3);
    matmul_float(mf1, mf2, mf3);
    run_experiment(mcf1, mcf2, mcf3);

    double err_sum_cf2 = 0;
    double err_sum_f2 = 0;
    double ref_sum_d2 = 0;

    for (int j = 0; j < NN; j++) {
    	for (int k = 0; k < NN; k++) {
    		double sum_d = md3[j][k];
    		float sum_f = mf3[j][k];
    		float sum_cf = mcf3[j][k].getDouble();

    		double err_abs_cf = fabs(sum_d - sum_cf);
    		double err_rel_cf = err_abs_cf / fabs(sum_d);

    		double err_abs_f = fabs(sum_d - sum_f);
    		double err_rel_f = err_abs_f / fabs(sum_d);

    		err_sum_cf2 += err_abs_cf * err_abs_cf;
    		err_sum_f2 += err_abs_f * err_abs_f;
    		ref_sum_d2 += sum_d * sum_d;
    	}
    }
#if defined(__ENABLE_PRINT__)
    printf("\n\n");
    for (int i = 0; i < NN; i++) {
    	for (int j = 0; j < NN; j++) {
    	    printf("%5.16f, %5.16f, %5.16f\n", md3[i][j], mf3[i][j], mcf3[i][j].getDouble());
    	}
    	printf("\n");
    }
#endif

    double err_norm2_f = sqrt(err_sum_f2 / ref_sum_d2);
    printf("\nNorm2 Error Float    : %5.16f\n", err_norm2_f);
    double err_norm2_cf = sqrt(err_sum_cf2 / ref_sum_d2);
    printf("Norm2 Error Cus.Float: %5.16f\n", err_norm2_cf);

#endif

	return 0;
}
