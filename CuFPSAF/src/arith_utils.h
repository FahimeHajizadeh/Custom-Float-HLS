#ifndef ARITH_UTILS_H
#define ARITH_UTILS_H

#include <ap_int.h>

union TransferD {
    double d;
    unsigned long long i;
};

union TransferF {
    float f;
    unsigned int i;
};

namespace CuFl {

template<int W>
void lshu(ap_uint<W> &x, const unsigned char &shift)
{
#pragma HLS inline
	const bool b0 = shift & static_cast<const unsigned char>(1);
	const bool b1 = shift & static_cast<const unsigned char>(2);
	const bool b2 = shift & static_cast<const unsigned char>(4);
	const bool b3 = shift & static_cast<const unsigned char>(8);
	const bool b4 = shift & static_cast<const unsigned char>(16);
	const bool b5 = shift & static_cast<const unsigned char>(32);
	const bool b6 = shift & static_cast<const unsigned char>(64);
	x <<= b0 ? 1 : 0;
	x <<= b1 ? 2 : 0;
	x <<= b2 ? 4 : 0;
	x <<= b3 ? 8 : 0;
	x <<= b4 ? 16 : 0;
	x <<= b5 ? 32 : 0;
	x <<= b6 ? 64 : 0;
}

template<int W>
void lsh(ap_int<W> &x, const unsigned char &shift)
{
//#pragma HLS inline
	const bool b0 = shift & static_cast<const unsigned char>(1);
	const bool b1 = shift & static_cast<const unsigned char>(2);
	const bool b2 = shift & static_cast<const unsigned char>(4);
	const bool b3 = shift & static_cast<const unsigned char>(8);
	const bool b4 = shift & static_cast<const unsigned char>(16);
	const bool b5 = shift & static_cast<const unsigned char>(32);
	const bool b6 = shift & static_cast<const unsigned char>(64);
	x <<= b0 ? 1 : 0;
	x <<= b1 ? 2 : 0;
	x <<= b2 ? 4 : 0;
	x <<= b3 ? 8 : 0;
	x <<= b4 ? 16 : 0;
	x <<= b5 ? 32 : 0;
	x <<= b6 ? 64 : 0;
}

template<int W>
void rshu(ap_uint<W> &x, const unsigned char &shift)
{
#pragma HLS inline
	const bool b0 = shift & static_cast<const unsigned char>(1);
	const bool b1 = shift & static_cast<const unsigned char>(2);
	const bool b2 = shift & static_cast<const unsigned char>(4);
	const bool b3 = shift & static_cast<const unsigned char>(8);
	const bool b4 = shift & static_cast<const unsigned char>(16);
	const bool b5 = shift & static_cast<const unsigned char>(32);
	const bool b6 = shift & static_cast<const unsigned char>(64);
	x >>= b0 ? 1 : 0;
	x >>= b1 ? 2 : 0;
	x >>= b2 ? 4 : 0;
	x >>= b3 ? 8 : 0;
	x >>= b4 ? 16 : 0;
	x >>= b5 ? 32 : 0;
	x >>= b6 ? 64 : 0;
}

template<int W>
void rsh(ap_int<W> &x, const unsigned char &shift)
{
//#pragma HLS inline
	const bool b0 = shift & static_cast<const unsigned char>(1);
	const bool b1 = shift & static_cast<const unsigned char>(2);
	const bool b2 = shift & static_cast<const unsigned char>(4);
	const bool b3 = shift & static_cast<const unsigned char>(8);
	const bool b4 = shift & static_cast<const unsigned char>(16);
	const bool b5 = shift & static_cast<const unsigned char>(32);
	const bool b6 = shift & static_cast<const unsigned char>(64);
	x >>= b0 ? 1 : 0;
	x >>= b1 ? 2 : 0;
	x >>= b2 ? 4 : 0;
	x >>= b3 ? 8 : 0;
	x >>= b4 ? 16 : 0;
	x >>= b5 ? 32 : 0;
	x >>= b6 ? 64 : 0;
}

template<int W>
short enc(const ap_uint<W> &x)
{
	if (W == 2)
		return x[1] ? 1 : x[0] ? 0 : 0;
	else if (W == 3)
		return x[2] ? 2 : x[1] ? 1 : x[0] ? 0 : 0;
	else if (W == 4)
		return x[3] ? 3 : x[2] ? 2 : x[1] ? 1 : x[0] ? 0 : 0;
	else if (W == 5)
		return x[4] ? 4 : x[3] ? 3 : x[2] ? 2 : x[1] ? 1 : x[0] ? 0 : 0;
	else if (W == 6)
		return x[5] ? 5 : x[4] ? 4 : x[3] ? 3 : x[2] ? 2 : x[1] ? 1 : x[0] ? 0 : 0;
	else if (W == 7)
		return x[6] ? 6 : x[5] ? 5 : x[4] ? 4 : x[3] ? 3 : x[2] ? 2 : x[1] ? 1 : x[0] ? 0 : 0;
	else if (W == 8)
		return x[7] ? 7 : x[6] ? 6 : x[5] ? 5 : x[4] ? 4 : x[3] ? 3 : x[2] ? 2 : x[1] ? 1 : x[0] ? 0 : 0;
	else if (W == 9)
		return x[8] ? 8 : x[7] ? 7 : x[6] ? 6 : x[5] ? 5 : x[4] ? 4 : x[3] ? 3 : x[2] ? 2 : x[1] ? 1 : x[0] ? 0 : 0;
	else if (W == 10)
		return x[9] ? 9 : x[8] ? 8 : x[7] ? 7 : x[6] ? 6 : x[5] ? 5 : x[4] ? 4 : x[3] ? 3 : x[2] ? 2 : x[1] ? 1 : x[0] ? 0 : 0;
	else {
		return 0;
	}
}

// *****************************************************************************************************************

// Recursive template specialization to compute leading zeros
template <int W>
struct clz_ap_uint_impl {
	static int calculate(const ap_uint<W>& num) {
		if (num == 0) { return W; }
		if (W == 0) { return 0; }
		else if (W == 1) { return (num == 0) ? 1 : 0; }
		else {
			constexpr int half_width = (W + 1) / 2;
			constexpr int remaining_width = W - half_width;

			ap_uint<half_width> upper = num.range(W - 1, remaining_width);
			ap_uint<remaining_width> lower = num.range(remaining_width - 1, 0);

			if (upper != 0) {
				return clz_ap_uint_impl<half_width>::calculate(upper);
			} else {
				return half_width + clz_ap_uint_impl<remaining_width>::calculate(lower);
			}
		}
	}
};

// Base case for 32-bit numbers
template <>
struct clz_ap_uint_impl<32> {
    static int calculate(const ap_uint<32>& num) {
        return num != 0 ? __builtin_clz(num.to_uint()) : 32;
    }
};

// Wrapper function to call the implementation
template <int W>
int clz_ap_uint(const ap_uint<W>& num) {
    return clz_ap_uint_impl<W>::calculate(num);
}

}


#endif // ARITH_UTILS_H
