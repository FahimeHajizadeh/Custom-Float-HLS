#ifndef CUSTOM_FLOAT_H
#define CUSTOM_FLOAT_H

#include <math.h>
#include <iostream>
#include "array_util.h"
#include "arith_utils.h"

namespace CuFl {

template <int WM, int WE, int l, int b>
class CuFPSAF
{
public:
    ap_int<WM> m;
    ap_uint<WE> e;
    ap_uint<l> lsh;

    CuFPSAF() :
    	m(0),
		e(0),
		lsh(0)
    {}

    CuFPSAF(const float &val)
    {
    	if (val == 0.0) {
    		m = 0;
    		e = 0;
    		lsh = 0;
    	}
    	else {
    		TransferF t;
    		t.f = val;
    		const ap_uint<32> bin(t.i);

    		const ap_uint<8> et(bin.range(30, 23));
    		lsh = et.range(l - 1, 0);
    		const ap_int<WM> mt((ap_uint<WM>(bin.range(23 - 1, 0)) | (ap_uint<WM>(1) << 23)) << lsh);

    		e = et.range(7, l);
    		m = bin[31] ? ap_int<WM>(- mt) : mt;
    	}
    }

    CuFPSAF(const CuFPSAF<WM, WE, l, b>& copy)
    {
        this->m = copy.m;
        this->e = copy.e;
        this->lsh = copy.lsh;
    }

    float to_float() const {
    	bool sign = (m[WM - 1] == 1) ? 1 : 0;
    	const ap_int<WM> mt = sign ? ap_int<WM>(- m) : m;
    	const short zcnt = clz_ap_uint<WM>(mt);
    	const short ih = WM - zcnt - 2; // (WM - 1) - zcnt - 1
    	const short il = ih < 23 ? 0 : ih - 22;

    	ap_uint<32> fp(0);
    	fp.range(22, 22-(ih-il)) = mt.range(ih, il);
    	fp.range(30, 23) = (ap_uint<8>(e) << l) + WM - zcnt - 23 - 1;
    	fp[31] = sign ? 1 : 0;

        TransferF t;
        t.i = fp.to_uint();

//        printf("%d, %d, %d, %x %8.16f\n", zcnt, ih, il, fp.to_uint(), t.f);

        return t.f;
    }

    void print() const
    {
        printf("The value is %16.16lf\n", to_float());
    }
};

// Recursive template to compute max index within a range
template<int START, int END, int WM, int WE, int l, int b>
struct MaxIndex
{
    static int compute(const CuFPSAF<WM, WE, l, b> x[], ap_uint<WE>& max_e) {
        constexpr int MID = (START + END) / 2;

        ap_uint<WE> max_e_left = 0, max_e_right = 0;
        const int max_idx_left = MaxIndex<START, MID, WM, WE, l, b>::compute(x, max_e_left);
        const int max_idx_right = MaxIndex<MID + 1, END, WM, WE, l, b>::compute(x, max_e_right);

        if (max_e_left >= max_e_right) {
            max_e = max_e_left;
            return max_idx_left;
        } else {
            max_e = max_e_right;
            return max_idx_right;
        }
    }
};

// Base case for single element
template<int INDEX, int WM, int WE, int l, int b>
struct MaxIndex<INDEX, INDEX, WM, WE, l, b>
{
    static int compute(const CuFPSAF<WM, WE, l, b> x[], ap_uint<WE>& max_e) {
        max_e = x[INDEX].e;
        return INDEX;
    }
};

template<int WM, int WE, int l, int b>
CuFPSAF<WM, WE, l, b> sum(const CuFPSAF<WM, WE, l, b> x, const CuFPSAF<WM, WE, l, b> y)
{
//#pragma HLS INLINE
#pragma HLS PIPELINE
	CuFPSAF<WM, WE, l, b> z;

	z.lsh = (x.e > y.e) ? x.lsh : y.lsh;
	z.e = (x.e > y.e) ? x.e : y.e;

	const ap_uint<WE+l+1> qx = z.e - x.e;
	const ap_uint<WE+l+1> qy = z.e - y.e;
	z.m = (x.m >> (qx << l)) + (y.m >> (qy << l));

//	printf("%x + %x = %x\n", (x.m >> (qx * pow2l)).to_uint(), (y.m >> (qy * pow2l)).to_uint(), z.m);

	return z;
}

template<int WM, int WE, int l, int b>
CuFPSAF<WM, WE, l, b> mul(const CuFPSAF<WM, WE, l, b> x, const CuFPSAF<WM, WE, l, b> y)
{
#pragma HLS INLINE
	CuFPSAF<WM, WE, l, b> z;

	const ap_uint<WE+l> bias = (1 << (WE + l - 1)) - 1;

	const ap_uint<WE+l> et = ap_uint<WE+l>(x.e.concat(x.lsh)) + ap_uint<WE+l>(y.e.concat(y.lsh)) - bias;
//	printf("EXP: %d %d %d %d %d - %d %d %d\n", bias, x.e.to_uint(), x.lsh.to_uint(), y.e.to_uint(), y.lsh.to_uint(), et.to_uint(), ap_uint<WE+l>(x.e.concat(x.lsh)).to_uint(), ap_uint<WE+l>(y.e.concat(y.lsh)).to_uint());
	z.e = et.range(WE + l - 1, l);
	z.lsh = et.range(l - 1, 0);

//	const ap_uint<l+1> il(ap_uint<l+1>(x.lsh) + ap_uint<l+1>(y.lsh));
//	const ap_uint<l+1> ih(il + 25 - 1);

//	const ap_int<50> mt = x.m.range(ap_uint<l+1>(x.lsh) + 24, ap_uint<l+1>(x.lsh)) * y.m.range(ap_uint<l+1>(y.lsh) + 24, ap_uint<l+1>(y.lsh));
//	z.m = ap_int<64>(mt.range(47, 47-24)) << z.lsh;
//	z.lsh += mt.range(49, 48);


	const ap_uint<25> m1(x.m[WM-1] ? - x.m.range(ap_uint<l+1>(x.lsh) + 24, ap_uint<l+1>(x.lsh)) : x.m.range(ap_uint<l+1>(x.lsh) + 24, ap_uint<l+1>(x.lsh)));
	const ap_uint<25> m2(y.m[WM-1] ? - y.m.range(ap_uint<l+1>(y.lsh) + 24, ap_uint<l+1>(y.lsh)) : y.m.range(ap_uint<l+1>(y.lsh) + 24, ap_uint<l+1>(y.lsh)));
	const ap_uint<50> mt(m1 * m2);

//	z.lsh += mt.range(49, 48);
	z.m = (x.m[WM-1] ^ y.m[WM-1]) ? ap_int<64>(- (ap_int<64>(mt.range(47, 47-24)) << z.lsh)) : (ap_int<64>(mt.range(47, 47-24)) << z.lsh);

	return z;
}

template<int WM, int WE, int l, int b, int N>
CuFPSAF<WM, WE, l, b> vsum(const CuFPSAF<WM, WE, l, b> x[N])
{
#pragma HLS INLINE
//#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=x type=complete

	CuFPSAF<WM, WE, l, b> z;

	ap_uint<WE> max_e = x[0].e;
	int max_idx = MaxIndex<0, N - 1, WM, WE, l, b>::compute(x, max_e);

	z.lsh = x[max_idx].lsh;
	z.e = x[max_idx].e;

	ap_int<WM> m[N];
#pragma HLS ARRAY_PARTITION variable=m type=complete
	for (int i = 0; i < N; i ++) {
#pragma HLS UNROLL factor=N
		m[i] = x[i].m >> (ap_uint<WE+l+1>(z.e - x[i].e) << l);  // Aligning all mantissas (m) to the maximum exponent by rsh each mantissa.
	}

	z.m = array_sum<ap_int<WM>, N>(m);   // Performs a summation of all mantissas stored in the array

	return z;
}

template<int WM, int WE, int l, int b, int N>
CuFPSAF<WM, WE, l, b> dp(const CuFPSAF<WM, WE, l, b> x[N], const CuFPSAF<WM, WE, l, b> y[N])
{
#pragma HLS INLINE
	CuFl::CuFPSAF<WM, WE, l, b> m[N];
#pragma HLS ARRAY_PARTITION variable=m type=complete

	for (int i = 0; i < N; i ++) {
#pragma HLS UNROLL factor=N
		m[i] = CuFl::mul<WM, WE, l, b>(x[i], y[i]);
	}

	return CuFl::vsum<WM, WE, l, b, N>(m);
}

}

#endif // CUSTOM_FLOAT_H
