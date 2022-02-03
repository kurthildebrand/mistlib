/************************************************************************************************//**
 * @file		calc.h
 *
 * @copyright	Copyright 2022 Kurt Hildebrand.
 * @license		Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
 *				file except in compliance with the License. You may obtain a copy of the License at
 *
 *				http://www.apache.org/licenses/LICENSE-2.0
 *
 *				Unless required by applicable law or agreed to in writing, software distributed under
 *				the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
 *				ANY KIND, either express or implied. See the License for the specific language
 *				governing permissions and limitations under the License.
 *
 * @ref			https://stackoverflow.com/questions/1878907/the-smallest-difference-between-2-angles
 * @TODO:		Add ceildiv: ceil(a/b) -> ((a-1) / b) + 1
 *
 * @desc		calc_sign(x)          Returns the sign [-1, 0, +1] of x.
 * 				calc_abs(x)           Returns the absolute value of x
 * 				calc_max(a,b)         Returns the maximum of a, b.
 * 				calc_min(a,b)         Returns the minimum of a, b.
 * 				calc_clamp(x, l, h)   Limits x to between l (low) and h (high).
 * 				calc_avg(a,b)         Returns the average of a, b while avoiding overflow.
 * 				calc_log2(x)          Returns the log base 2 of x.
 * 				calc_clp2(x)          Returns the ceiling power of 2 of x.
 * 				calc_flp2(x)          Returns the floor power of 2 of x.
 * 				calc_popcount(x)      Returns the number of bits set in x.
 * 				calc_round(x,n)       Rounds x to the nearest multiple of n.
 * 				calc_mod(a,b)         Calculates a mod b: calc_mod(-1, 360) = 359
 * 				calc_submod(a,b,m)    Calculates (a-b) mod m.
 * 				calc_addmod(a,b,m)    Calculates (a+b) mod m.
 * 				calc_saddmod(a,b,m)   Calculates (a+b) mod m where b is signed.
 * 				calc_addsat(a,b)      Performs the saturating addition a+b.
 * 				calc_wrapdiff(a,b,m)  Calculates a-b with wrap around m: wrapdiff(350, 10, 360) = -20
 * 				calc_dop(a,b,c,d)     Calculates a*b - c*d. Avoides catastrophic cancellation.
 * 				calc_gcd(a,b)         Calculates the greatest common divisor of a and b.
 * 				calc_ext_gcd(a,b,s,t) Calculates the extgcd(a,b) = r = a*s + b*t.
 * 				calc_lcm(a,b)         Calculates the least common multiple of a and b.
 * 				calc_ax_by_c          Calculates integer solutions to ax + by = c.
 * 				calc_ax2_bx_c         Solves ax^2 + bx + c = 0. Avoids catastrphic cancellation.
 * 				calc_lerp             Calculates a linear interpolation between two inputs.
 * 				calc_rad_to_deg(x)    Converts x in radians to degrees.
 * 				calc_deg_to_rad(x)    Converts x in degrees to radians.
 * 				calc_map_360(x)       Maps x to [0, 360].
 * 				calc_map_2pi(x)       Maps x to [0, 2pi].
 * 				calc_map_180(x)       Maps x to [-180, 180].
 * 				calc_map_pi(x)        Maps x to [-pi, pi].
 * 				calc_within           Floating point comparison with given epsilon.
 * 				calc_compare          Floating point comparison with FLT_EPSILON / DBL_EPSILON.
 * 				calc_randf            Returns a random float between [0.0, 1.0].
 * 				calc_randf_range      Returns a random float between [min, max].
 * 				calc_rand_range       Retruns a random int between [min, max].
 *
 ***************************************************************************************************/
#ifndef CALC_H
#define CALC_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


/* Public Macros --------------------------------------------------------------------------------- */
#if !defined(M_E)
	#define M_E				(2.71828182845904523536028747135266250)   /* e               */
#endif
#if !defined(M_LOG2E)
	#define M_LOG2E			(1.44269504088896340735992468100189214)   /* log2(e)         */
#endif
#if !defined(M_LOG10E)
	#define M_LOG10E		(0.434294481903251827651128918916605082)  /* log10(e)        */
#endif
#if !defined(M_LN2)
	#define M_LN2			(0.693147180559945309417232121458176568)  /* loge(2)         */
#endif
#if !defined(M_LN10)
	#define M_LN10			(2.30258509299404568401799145468436421)   /* loge(10)        */
#endif
#if !defined(M_PI)
	#define M_PI			(3.14159265358979323846264338327950288)   /* pi              */
#endif
#if !defined(M_2PI)
	#define M_2PI			(6.28318530717958647692528676655900577)   /* 2*pi            */
#endif
#if !defined(M_PI_2)
	#define M_PI_2			(1.57079632679489661923132169163975144)   /* pi/2            */
#endif
#if !defined(M_PI_4)
	#define M_PI_4			(0.785398163397448309615660845819875721)  /* pi/4            */
#endif
#if !defined(M_1_PI)
	#define M_1_PI			(0.318309886183790671537767526745028724)  /* 1/pi            */
#endif
#if !defined(M_2_PI)
	#define M_2_PI			(0.636619772367581343075535053490057448)  /* 2/pi            */
#endif
#if !defined(M_2_SQRTPI)
	#define M_2_SQRTPI		(1.12837916709551257389615890312154517)   /* 2/sqrt(pi)      */
#endif
#if !defined(M_SQRT2)
	#define M_SQRT2			(1.41421356237309504880168872420969808)   /* sqrt(2)         */
#endif
#if !defined(M_SQRT3)
	#define M_SQRT3			(1.73205080756887729352744634150587237)   /* sqrt(3)         */
#endif
#if !defined(M_SQRt6)
	#define M_SQRT6			(2.44948974278317809819728407470589139)   /* sqrt(6)         */
#endif
#if !defined(M_SQRT1_2)
	#define M_SQRT1_2		(0.707106781186547524400844362104849039)  /* 1/sqrt(2)       */
#endif
#if !defined(M_PHI)
	#define M_PHI			(1.61803398874989484820458683436563812)  /* golden ratio     */
#endif
#if !defined(M_PHI_FRAC_32)
	#define M_PHI_FRAC_32	(0x9e3779b9)                             /* frac(phi) * 2^32 */
#endif
#if !defined(M_PHI_FRAC_64)
	#define M_PHI_FRAC_64	(0x9e3779b97f4a7c15)                     /* frac(phi) * 2^64 */
#endif
#if !defined(M_C)
	#define M_C				(299792458.0) /* speed of light in m/s */
#endif


/* Public Functions ------------------------------------------------------------------------------ */
// ----------------------------------------------------------------------------------------------- //
// Sign                                                                                            //
// ----------------------------------------------------------------------------------------------- //
#define calc_sign(x) _Generic((x), \
	int8_t:   calc_sign_int (x), \
	int16_t:  calc_sign_int (x), \
	int32_t:  calc_sign_i32 (x), \
	int64_t:  calc_sign_i64 (x), \
	uint8_t:  calc_sign_uint(x), \
	uint16_t: calc_sign_uint(x), \
	uint32_t: calc_sign_u32 (x), \
	uint64_t: calc_sign_u64 (x), \
	float:    calc_sign_f   (x), \
	double:   calc_sign_d   (x))


inline int      calc_sign_int (int      x) { return x < 0 ? -1 : +1;    }
inline int32_t  calc_sign_i32 (int32_t  x) { return x < 0 ? -1 : +1;    }
inline int64_t  calc_sign_i64 (int64_t  x) { return x < 0 ? -1 : +1;    }
inline unsigned calc_sign_uint(unsigned x) { (void)(x); return +1;      }
inline uint32_t calc_sign_u32 (uint32_t x) { (void)(x); return +1;      }
inline uint64_t calc_sign_u64 (uint64_t x) { (void)(x); return +1;      }
inline float    calc_sign_f   (float    x) { return copysignf(1.0f, x); }
inline double   calc_sign_d   (double   x) { return copysign(1.0, x);   }





// ----------------------------------------------------------------------------------------------- //
// Abs                                                                                             //
// ----------------------------------------------------------------------------------------------- //
#define calc_abs(x) _Generic((x), \
	int8_t:   calc_abs_int (x), \
	int16_t:  calc_abs_int (x), \
	int32_t:  calc_abs_i32 (x), \
	int64_t:  calc_abs_i64 (x), \
	uint8_t:  calc_abs_uint(x), \
	uint16_t: calc_abs_uint(x), \
	uint32_t: calc_abs_u32 (x), \
	uint64_t: calc_abs_u64 (x), \
	float:    calc_abs_f   (x), \
	double:   calc_abs_d   (x))


inline int      calc_abs_int (int x)      { return x * calc_sign_int(x); }
inline int32_t  calc_abs_i32 (int32_t x)  { return x * calc_sign_i32(x); }
inline int64_t  calc_abs_i64 (int64_t x)  { return x * calc_sign_i64(x); }
inline unsigned calc_abs_uint(unsigned x) { return x;                    }
inline uint32_t calc_abs_u32 (uint32_t x) { return x;                    }
inline uint64_t calc_abs_u64 (uint64_t x) { return x;                    }
inline float    calc_abs_f   (float x)    { return fabsf(x);             }
inline double   calc_abs_d   (double x)   { return fabs(x);              }





// ----------------------------------------------------------------------------------------------- //
// Max                                                                                             //
// ----------------------------------------------------------------------------------------------- //
#define calc_max(a, b) _Generic((a) + (b), \
	int8_t:   calc_max_int (a, b), \
	int16_t:  calc_max_int (a, b), \
	int32_t:  calc_max_i32 (a, b), \
	int64_t:  calc_max_i64 (a, b), \
	uint8_t:  calc_max_uint(a, b), \
	uint16_t: calc_max_uint(a, b), \
	uint32_t: calc_max_u32 (a, b), \
	uint64_t: calc_max_u64 (a, b), \
	float:    calc_max_f   (a, b), \
	double:   calc_max_d   (a, b))

inline int      calc_max_int (int      a, int      b) { return a > b ? a : b; }
inline int32_t  calc_max_i32 (int32_t  a, int32_t  b) { return a > b ? a : b; }
inline int64_t  calc_max_i64 (int64_t  a, int64_t  b) { return a > b ? a : b; }
inline unsigned calc_max_uint(unsigned a, unsigned b) { return a > b ? a : b; }
inline uint32_t calc_max_u32 (uint32_t a, uint32_t b) { return a > b ? a : b; }
inline uint64_t calc_max_u64 (uint64_t a, uint64_t b) { return a > b ? a : b; }
inline float    calc_max_f   (float    a, float    b) { return a > b ? a : b; }
inline double   calc_max_d   (double   a, double   b) { return a > b ? a : b; }





// ----------------------------------------------------------------------------------------------- //
// Min                                                                                             //
// ----------------------------------------------------------------------------------------------- //
#define calc_min(a, b) _Generic((a) + (b), \
	int8_t:   calc_min_int (a, b), \
	int16_t:  calc_min_int (a, b), \
	int32_t:  calc_min_i32 (a, b), \
	int64_t:  calc_min_i64 (a, b), \
	uint8_t:  calc_min_uint(a, b), \
	uint16_t: calc_min_uint(a, b), \
	uint32_t: calc_min_u32 (a, b), \
	uint64_t: calc_min_u64 (a, b), \
	float:    calc_min_f   (a, b), \
	double:   calc_min_d   (a, b))

inline int      calc_min_int (int      a, int      b) { return a < b ? a : b; }
inline int32_t  calc_min_i32 (int32_t  a, int32_t  b) { return a < b ? a : b; }
inline int64_t  calc_min_i64 (int64_t  a, int64_t  b) { return a < b ? a : b; }
inline unsigned calc_min_uint(unsigned a, unsigned b) { return a < b ? a : b; }
inline uint32_t calc_min_u32 (uint32_t a, uint32_t b) { return a < b ? a : b; }
inline uint64_t calc_min_u64 (uint64_t a, uint64_t b) { return a < b ? a : b; }
inline float    calc_min_f   (float    a, float    b) { return a < b ? a : b; }
inline double   calc_min_d   (double   a, double   b) { return a < b ? a : b; }





// ----------------------------------------------------------------------------------------------- //
// Clamp                                                                                           //
// ----------------------------------------------------------------------------------------------- //
/* calc_clmap ***********************************************************************************//**
 * @brief		Limits a number to between [low, high]. */
#define calc_clamp(x, low, high) _Generic((x) + (low) + (high), \
	int8_t:   calc_clamp_int (x, low, high), \
	int16_t:  calc_clamp_int (x, low, high), \
	int32_t:  calc_clamp_i32 (x, low, high), \
	int64_t:  calc_clamp_i64 (x, low, high), \
	uint8_t:  calc_clamp_uint(x, low, high), \
	uint16_t: calc_clamp_uint(x, low, high), \
	uint32_t: calc_clamp_u32 (x, low, high), \
	uint64_t: calc_clamp_u64 (x, low, high), \
	float:    calc_clamp_f   (x, low, high), \
	double:   calc_clamp_d   (x, low, high))


inline int calc_clamp_int(int x, int low, int high)
{
	return calc_min_int(calc_max_int(x, low), high);
}


inline int32_t calc_clamp_i32(int32_t x, int32_t low, int32_t high)
{
	return calc_min_i32(calc_max_i32(x, low), high);
}


inline int64_t calc_clamp_i64(int64_t x, int64_t low, int64_t high)
{
	return calc_min_i64(calc_max_i64(x, low), high);
}


inline unsigned calc_clamp_uint(unsigned x, unsigned low, unsigned high)
{
	return calc_min_uint(calc_max_uint(x, low), high);
}


inline uint32_t calc_clamp_u32(uint32_t x, uint32_t low, uint32_t high)
{
	return calc_min_u32(calc_max_u32(x, low), high);
}


inline uint64_t calc_clamp_u64(uint64_t x, uint64_t low, uint64_t high)
{
	return calc_min_u64(calc_max_u64(x, low), high);
}


inline float calc_clamp_f(float x, float low, float high)
{
	return calc_min_f(calc_max_f(x, low), high);
}


inline double calc_clamp_d(double x, double low, double high)
{
	return calc_min_d(calc_max_d(x, low), high);
}





// ----------------------------------------------------------------------------------------------- //
// Avg                                                                                             //
// ----------------------------------------------------------------------------------------------- //
/* calc_avg *************************************************************************************//**
 * @brief		Returns the average of two numbers. Rounds towards zero. Avoids overflow. */
#define calc_avg(a, b) _Generic((a) + (b), \
	int8_t:   calc_avg_int (a, b), \
	int16_t:  calc_avg_int (a, b), \
	int32_t:  calc_avg_i32 (a, b), \
	int64_t:  calc_avg_i64 (a, b), \
	uint8_t:  calc_avg_uint(a, b), \
	uint16_t: calc_avg_uint(a, b), \
	uint32_t: calc_avg_u32 (a, b), \
	uint64_t: calc_avg_u64 (a, b), \
	float:    calc_avg_f   (a, b), \
	double:   calc_avg_d   (a, b))

inline unsigned calc_avg_uint(unsigned a, unsigned b) { return a/2 + b/2 + (a%2) * (b%2); }
inline uint32_t calc_avg_u32 (uint32_t a, uint32_t b) { return a/2 + b/2 + (a%2) * (b%2); }
inline uint64_t calc_avg_u64 (uint64_t a, uint64_t b) { return a/2 + b/2 + (a%2) * (b%2); }
inline float    calc_avg_f   (float    a, float    b) { return (a+b) / 2; }
inline double   calc_avg_d   (double   a, double   b) { return (a+b) / 2; }


inline int calc_avg_int(int a, int b)
{
	/* Check if a and b have different signs */
	if((a < 0) == (b > 0))
	{
		return (a + b) / 2;
	}
	else
	{
		return a/2 + b/2 + (a%2 + b%2)/2;
	}
}


inline int32_t calc_avg_i32(int32_t a, int32_t b)
{
	/* Check if a and b have different signs */
	if((a < 0) == (b > 0))
	{
		return (a + b) / 2;
	}
	else
	{
		return a/2 + b/2 + (a%2 + b%2)/2;
	}
}


inline int64_t calc_avg_i64(int64_t a, int64_t b)
{
	/* Check if a and b have different signs */
	if((a < 0) == (b > 0))
	{
		return (a + b) / 2;
	}
	else
	{
		return a/2 + b/2 + (a%2 + b%2)/2;
	}
}





// ----------------------------------------------------------------------------------------------- //
// Bit-twiddling                                                                                   //
// ----------------------------------------------------------------------------------------------- //
/* calc_log2 ************************************************************************************//**
 * @brief		Calculates and returns log2 of the input x. */
inline unsigned calc_log2(unsigned x)
{
	unsigned i = 0;

	while(x /= 2)
	{
		i++;
	}

	return i;
}


/* calc_clp2 ************************************************************************************//**
 * @brief		Ceiling power of 2. Rounds a number up to a power of 2. */
inline unsigned calc_clp2(unsigned x)
{
	unsigned i;

	x--;

	for(i = 1; i < (sizeof(x) * CHAR_BIT) / 2; i *= 2)
	{
		x |= x >> i;
	}

	x++;

	return x;
}


/* calc_flp2 ************************************************************************************//**
 * @brief		Floor power of 2. Rounds a number down to a power of 2. */
inline unsigned calc_flp2(unsigned x)
{
	unsigned i;

	for(i = 1; i < sizeof(x) * CHAR_BIT; i *= 2)
	{
		x |= x >> 1;
	}

	return x - (x/2);
}


/* calc_popcount ********************************************************************************//**
 * @brief		Returns the number of bits set to 1 in the variable. */
#define calc_popcount(x) _Generic((x), \
	int8_t:   calc_popcount_u16(x), \
	int16_t:  calc_popcount_u16(x), \
	int32_t:  calc_popcount_u32(x), \
	int64_t:  calc_popcount_u64(x), \
	uint8_t:  calc_popcount_u16(x), \
	uint16_t: calc_popcount_u16(x), \
	uint32_t: calc_popcount_u32(x), \
	uint64_t: calc_popcount_u64(x))

inline unsigned calc_popcount_u16(uint16_t x)
{
	/* Count in parallel */
	// x = ((x >> 1)  & 0x5555) + (x & 0x5555);
	// x = ((x >> 2)  & 0x3333) + (x & 0x3333);
	// x = ((x >> 4)  & 0x0F0F) + (x & 0x0F0F);
	// x = ((x >> 8)  & 0x00FF) + (x & 0x00FF);
	// return x;

	/* Count each set bit */
	unsigned count = 0;
	while(x)
	{
		x &= x-1;
		count++;
	}

	return count;
}

inline unsigned calc_popcount_u32(uint32_t x)
{
	/* Count in parallel */
	// x = ((x >> 1)  & 0x55555555) + (x & 0x55555555);
	// x = ((x >> 2)  & 0x33333333) + (x & 0x33333333);
	// x = ((x >> 4)  & 0x0F0F0F0F) + (x & 0x0F0F0F0F);
	// x = ((x >> 8)  & 0x00FF00FF) + (x & 0x00FF00FF);
	// x = ((x >> 16) & 0x0000FFFF) + (x & 0x0000FFFF);
	// return x;

	/* Count each set bit */
	unsigned count = 0;
	while(x)
	{
		x &= x-1;
		count++;
	}

	return count;
}

inline unsigned calc_popcount_u64(uint64_t x)
{
	/* Count in parallel */
	x = ((x >> 1)  & 0x5555555555555555) + (x & 0x5555555555555555);
	x = ((x >> 2)  & 0x3333333333333333) + (x & 0x3333333333333333);
	x = ((x >> 4)  & 0x0F0F0F0F0F0F0F0F) + (x & 0x0F0F0F0F0F0F0F0F);
	x = ((x >> 8)  & 0x00FF00FF00FF00FF) + (x & 0x00FF00FF00FF00FF);
	x = ((x >> 16) & 0x0000FFFF0000FFFF) + (x & 0x0000FFFF0000FFFF);
	x = ((x >> 32) & 0x00000000FFFFFFFF) + (x & 0x00000000FFFFFFFF);
	return (unsigned)x;

	/* Count each set bit */
	// unsigned count = 0;
	// while(x)
	// {
	// 	x &= x-1;
	// 	count++;
	// }

	// return count;
}





// ----------------------------------------------------------------------------------------------- //
// Round                                                                                           //
// ----------------------------------------------------------------------------------------------- //
/* calc_round ***********************************************************************************//**
 * @brief		Rounds a number x to the nearest multiple of n. */
#define calc_round(x, n) _Generic((x) / (n), \
	int8_t:   calc_round_int (x, n), \
	int16_t:  calc_round_int (x, n), \
	int32_t:  calc_round_i32 (x, n), \
	int64_t:  calc_round_i64 (x, n), \
	uint8_t:  calc_round_uint(x, n), \
	uint16_t: calc_round_uint(x, n), \
	uint32_t: calc_round_u32 (x, n), \
	uint64_t: calc_round_u64 (x, n), \
	float:    calc_round_f   (x, n), \
	double:   calc_round_d   (x, n))


inline int calc_round_int(int x, int n)
{
	return n * (x/n + (calc_sign_int(x) * (calc_abs_int(x%n) >= n/2)));
}


inline int32_t calc_round_i32(int32_t x, int32_t n)
{
	return n * (x/n + (calc_sign_i32(x) * (calc_abs_i32(x%n) >= n/2)));
}


inline int64_t calc_round_i64(int64_t x, int64_t n)
{
	return n * (x/n + (calc_sign_i64(x) * (calc_abs_i64(x%n) >= n/2)));
}


inline unsigned calc_round_uint(unsigned x, unsigned n)
{
	return n * (x/n + (calc_sign_uint(x) * (calc_abs_uint(x%n) >= n/2)));
}


inline uint32_t calc_round_u32(uint32_t x, uint32_t n)
{
	return n * (x/n + (calc_sign_u32(x) * (calc_abs_u32(x%n) >= n/2)));
}


inline uint64_t calc_round_u64(uint64_t x, uint64_t n)
{
	return n * (x/n + (calc_sign_u64(x) * (calc_abs_u64(x%n) >= n/2)));
}


inline float calc_round_f(float x, float n)
{
	return n * (truncf(x/n) + (calc_sign_f(x) * (calc_abs_f(fmodf(x,n)) >= n/2)));
}


inline double calc_round_d(double x, double n)
{
	return n * (trunc(x/n) + (calc_sign_d(x) * (calc_abs_d(fmod(x,n)) >= n/2)));
}





// ----------------------------------------------------------------------------------------------- //
// Mod                                                                                             //
// ----------------------------------------------------------------------------------------------- //
/* calc_mod *************************************************************************************//**
 * @brief		Calculates a modulus b assuming b is positive.
 * @desc		The built in '%' operator calculates the remainder, not the modulus. This can produce
 * 				unexpected results when computing the modulus of negative numbers. For example:
 *
 * 					 1 % 360 = 1		calc_mod(1, 360) = 1
 * 					-1 % 360 = -1		calc_mod(-1, 360) = 359
 */
#define calc_mod(a, b) _Generic((a) % (b), \
	int8_t:   calc_mod_int (a, b), \
	int16_t:  calc_mod_int (a, b), \
	int32_t:  calc_mod_i32 (a, b), \
	int64_t:  calc_mod_i64 (a, b), \
	uint8_t:  calc_mod_uint(a, b), \
	uint16_t: calc_mod_uint(a, b), \
	uint32_t: calc_mod_u32 (a, b), \
	uint64_t: calc_mod_u64 (a, b), \
	float:    calc_mod_f   (a, b), \
	double:   calc_mod_d   (a, b))

inline int      calc_mod_int (int      a, int      b) { return ((a % b) + b) % b; }
inline int32_t  calc_mod_i32 (int32_t  a, int32_t  b) { return ((a % b) + b) % b; }
inline int64_t  calc_mod_i64 (int64_t  a, int64_t  b) { return ((a % b) + b) % b; }
inline unsigned calc_mod_uint(unsigned a, unsigned b) { return a % b; }
inline uint32_t calc_mod_u32 (uint32_t a, uint32_t b) { return a % b; }
inline uint64_t calc_mod_u64 (uint64_t a, uint64_t b) { return a % b; }
inline float    calc_mod_f   (float    a, float    b) { return a - b * floorf(a / b); }
inline double   calc_mod_d   (double   a, double   b) { return a - b * floor (a / b); }





// ----------------------------------------------------------------------------------------------- //
// Submod                                                                                          //
// ----------------------------------------------------------------------------------------------- //
/* submod ***************************************************************************************//**
 * @brief		Calculates (a-b) mod m. This is useful when (a-b) underflows the underlying variable.
 * @warning		Assumes a and b are less than m.
 * @desc		Examples:
 * 					submod(20, 40, 100) = 20-40 mod 100 = -20 mod 100 = 80
 * 					submod(587520000, 4112620000, 4112640000) = 587540000
 *
 * 					Note: with normal uint32_t behavior: 587520000 - 4112620000 = 4303120
 */
#define calc_submod(a, b, m) _Generic((a) + (b) + (m), \
	uint8_t:  calc_submod_uint(a, b, m), \
	uint16_t: calc_submod_uint(a, b, m), \
	uint32_t: calc_submod_u32 (a, b, m), \
	uint64_t: calc_submod_u64 (a, b, m))

inline unsigned calc_submod_uint(unsigned a, unsigned b, unsigned m)
{
	return a >= b ? (a - b) : (m - b + a);
}

inline uint32_t calc_submod_u32(uint32_t a, uint32_t b, uint32_t m)
{
	return a >= b ? (a - b) : (m - b + a);
}

inline uint64_t calc_submod_u64(uint64_t a, uint64_t b, uint64_t m)
{
	return a >= b ? (a - b) : (m - b + a);
}





// ----------------------------------------------------------------------------------------------- //
// Addmod                                                                                          //
// ----------------------------------------------------------------------------------------------- //
/* addmod ***************************************************************************************//**
 * @brief		Calculates (a+b) mod m. This is useful when (a+b) overflows the underlying variable.
 * @warning		Assumes a and b are less than m.
 * @desc		Examples:
 * 					addmod(60, 40, 100) = 20
 * 					addmod(4112620000, 587520000, 4112640000) = 587500000
 *
 * 					Note: with normal uint32_t behavior: 4112620000 + 587520000 = 40517270480.
 */
#define calc_addmod(a, b, m) _Generic((a) + (b) + (m), \
	uint8_t:  calc_addmod_uint(a, b, m), \
	uint16_t: calc_addmod_uint(a, b, m), \
	uint32_t: calc_addmod_u32 (a, b, m), \
	uint64_t: calc_addmod_u64 (a, b, m))


/* saddmod **************************************************************************************//**
 * @brief		Calculates (a+b) mod m where b is signed. This is useful when (a+b) overflows the
 * 				underlying variable.
 * @warning		Assumes a and b are less than m.
 * @desc		Examples:
 * 					addmod(60, 40, 100) = 20
 * 					addmod(4112620000, 587520000, 4112640000) = 587500000
 *
 * 					Note: with normal uint32_t behavior: 4112620000 + 587520000 = 40517270480.
 */
#define calc_saddmod(a, b, m) _Generic((a) + (b) + (m), \
	uint8_t:  calc_saddmod_uint(a, b, m) \
	uint16_t: calc_saddmod_uint(a, b, m) \
	uint32_t: calc_saddmod_u32 (a, b, m) \
	uint64_t: calc_saddmod_u64 (a, b, m))

inline unsigned calc_addmod_uint(unsigned a, unsigned b, unsigned m)
{
	return calc_submod_uint(a, m-b, m);
}

inline uint32_t calc_addmod_u32(uint32_t a, uint32_t b, uint32_t m)
{
	return calc_submod_u32(a, m-b, m);
}

inline uint64_t calc_addmod_u64(uint64_t a, uint64_t b, uint64_t m)
{
	return calc_submod_u64(a, m-b, m);
}

inline unsigned calc_saddmod_uint(unsigned a, int b, unsigned m)
{
	return b > 0 ? calc_addmod_uint(a,  b, m) :
	               calc_submod_uint(a, -b, m);
}

inline uint32_t calc_saddmod_u32(uint32_t a, int32_t b, uint32_t m)
{
	return b > 0 ? calc_addmod_u32(a,  b, m) :
	               calc_submod_u32(a, -b, m);
}

inline uint64_t calc_saddmod_u64(uint64_t a, int64_t b, uint64_t m)
{
	return b > 0 ? calc_addmod_u64(a,  b, m) :
	               calc_submod_u64(a, -b, m);
}





// ----------------------------------------------------------------------------------------------- //
// Saturating Add                                                                                  //
// ----------------------------------------------------------------------------------------------- //
/* addsat ***************************************************************************************//**
 * @brief		Performs a saturating addition. */
#define calc_addsat(a, b) _Generic((a) + (b), \
	int8_t:   calc_addsat_i8 (a, b), \
	int16_t:  calc_addsat_i16(a, b), \
	int32_t:  calc_addsat_i32(a, b), \
	int64_t:  calc_addsat_i64(a, b), \
	uint8_t:  calc_addsat_u8 (a, b), \
	uint16_t: calc_addsat_u16(a, b), \
	uint32_t: calc_addsat_u32(a, b), \
	uint64_t: calc_addsat_u64(a, b))


inline int8_t calc_addsat_i8(int8_t a, int8_t b)
{
	return (a > 0 && b > (INT8_MAX-a)) ? INT8_MAX :
	       (a < 0 && b < (INT8_MIN-a)) ? INT8_MIN : a+b;
}


inline int16_t calc_addsat_i16(int16_t a, int16_t b)
{
	return (a > 0 && b > (INT16_MAX-a)) ? INT16_MAX :
	       (a < 0 && b < (INT16_MIN-a)) ? INT16_MIN : a+b;
}


inline int32_t calc_addsat_i32(int32_t a, int32_t b)
{
	return (a > 0 && b > (INT32_MAX-a)) ? INT32_MAX :
	       (a < 0 && b < (INT32_MIN-a)) ? INT32_MIN : a+b;
}


inline int64_t calc_addsat_i64(int64_t a, int64_t b)
{
	return (a > 0 && b > (INT64_MAX-a)) ? INT64_MAX :
	       (a < 0 && b < (INT64_MIN-a)) ? INT64_MIN : a+b;
}


inline uint8_t  calc_addsat_u8 (uint8_t  a, uint8_t  b) { return ((a+b) < a) ? UINT8_MAX  : a+b; }
inline uint16_t calc_addsat_u16(uint16_t a, uint16_t b) { return ((a+b) < a) ? UINT16_MAX : a+b; }
inline uint32_t calc_addsat_u32(uint32_t a, uint32_t b) { return ((a+b) < a) ? UINT32_MAX : a+b; }
inline uint64_t calc_addsat_u64(uint64_t a, uint64_t b) { return ((a+b) < a) ? UINT64_MAX : a+b; }





// ----------------------------------------------------------------------------------------------- //
// Wrapdiff                                                                                        //
// ----------------------------------------------------------------------------------------------- //
/* wrapdiff *************************************************************************************//**
 * @brief		Calculates the difference between two numbers with wrap around. The sign of the
 * 				returned value indicates the relative order 'a' and 'b'. A positive (>0) value
 * 				indicates that a > b. A negative (<0) value indicates that a < b.
 * @desc		For example, if given two angles
 *
 * 						a = 350 degrees
 * 						b = 10  degrees
 *
 * 					wrapdiff(10, 350, 360) =  20 (10 > 350)
 * 					wrapdiff(350, 10, 360) = -20 (350 < 10)
 */
#define calc_wrapdiff(a, b, m) _Generic((a) + (b) + (m), \
	int8_t:   calc_wrapdiff_int (a, b, m), \
	int16_t:  calc_wrapdiff_int (a, b, m), \
	int32_t:  calc_wrapdiff_i32 (a, b, m), \
	int64_t:  calc_wrapdiff_i64 (a, b, m), \
	uint8_t:  calc_wrapdiff_uint(a, b, m), \
	uint16_t: calc_wrapdiff_uint(a, b, m), \
	uint32_t: calc_wrapdiff_u32 (a, b, m), \
	uint64_t: calc_wrapdiff_u64 (a, b, m), \
	float:    calc_wrapdiff_f   (a, b, m), \
	double:   calc_wrapdiff_d   (a, b, m))


inline int calc_wrapdiff_int(int a, int b, int m)
{
	return calc_mod_int(a - b + m/2, m) - m/2;
}


inline int32_t calc_wrapdiff_i32(int32_t a, int32_t b, int32_t m)
{
	return calc_mod_i32(a - b + m/2, m) - m/2;
}


inline int64_t calc_wrapdiff_i64(int64_t a, int64_t b, int64_t m)
{
	return calc_mod_i64(a - b + m/2, m) - m/2;
}


inline int calc_wrapdiff_uint(unsigned a, unsigned b, unsigned m)
{
	unsigned d = calc_submod_uint(a, b, m);
	return   d > m/2 ? (d-m) : d;
}


inline int32_t calc_wrapdiff_u32(uint32_t a, uint32_t b, uint32_t m)
{
	uint32_t d = calc_submod_u32(a, b, m);
	return   d > m/2 ? (d-m) : d;
}


inline int64_t calc_wrapdiff_u64(uint64_t a, uint64_t b, uint64_t m)
{
	uint64_t d = calc_submod_u64(a, b, m);
	return   d > m/2 ? (d-m) : d;
}


inline float calc_wrapdiff_f(float a, float b, float m)
{
	return calc_mod_f(a - b + m/2, m) - m/2;
}


inline double calc_wrapdiff_d(float a, float b, float m)
{
	return calc_mod_d(a - b + m/2, m) - m/2;
}





// ----------------------------------------------------------------------------------------------- //
// Difference of Products (ab - cd)                                                                //
// ----------------------------------------------------------------------------------------------- //
/* calc_dop *************************************************************************************//**
 * @brief		Calculates a*b - c*d while avoiding catastrophic cancellation. */
#define calc_dop(a,c,b,d) _Generic((a)+(b)+(c)+(d), \
	float: calc_dop_f((a),(b),(c),(d)))

inline float calc_dop_f(float a, float b, float c, float d)
{
	float  cd  = c * d;
	float  err = fmaf(-c,  d,  cd);
	float  dop = fmaf( a,  b, -cd);
	return dop + err;
}





// ----------------------------------------------------------------------------------------------- //
// GCD                                                                                             //
// ----------------------------------------------------------------------------------------------- //
/* calc_gcd *************************************************************************************//**
 * @brief		Calculates the Greatest Common Divisor (GCD) of two numbers. */
#define calc_gcd(a, b) _Generic((a) + (b), \
	int8_t:   calc_gcd_int (a, b), \
	int16_t:  calc_gcd_int (a, b), \
	int32_t:  calc_gcd_i32 (a, b), \
	int64_t:  calc_gcd_i64 (a, b), \
	uint8_t:  calc_gcd_uint(a, b), \
	uint16_t: calc_gcd_uint(a, b), \
	uint32_t: calc_gcd_u32 (a, b), \
	uint64_t: calc_gcd_u64 (a, b))


inline int calc_gcd_int(int a, int b)
{
	int x = calc_max_int(a, b);
	int y = calc_min_int(a, b);

	while(y)
	{
		int z = x % y;
		x = y;
		y = z;
	}

	return x;
}


inline int32_t calc_gcd_i32(int32_t a, int32_t b)
{
	int32_t x = calc_max_i32(a, b);
	int32_t y = calc_min_i32(a, b);

	while(y)
	{
		int32_t z = x % y;
		x = y;
		y = z;
	}

	return x;
}


inline int64_t calc_gcd_i64(int64_t a, int64_t b)
{
	int64_t x = calc_max_i64(a, b);
	int64_t y = calc_min_i64(a, b);

	while(y)
	{
		int64_t z = x % y;
		x = y;
		y = z;
	}

	return x;
}


inline unsigned calc_gcd_uint(unsigned a, unsigned b)
{
	unsigned x = calc_max_uint(a, b);
	unsigned y = calc_min_uint(a, b);

	while(y)
	{
		unsigned z = x % y;
		x = y;
		y = z;
	}

	return x;
}


inline uint32_t calc_gcd_u32(uint32_t a, uint32_t b)
{
	uint32_t x = calc_max_u32(a, b);
	uint32_t y = calc_min_u32(a, b);

	while(y)
	{
		uint32_t z = x % y;
		x = y;
		y = z;
	}

	return x;
}


inline uint64_t calc_gcd_u64(uint64_t a, uint64_t b)
{
	uint64_t x = calc_max_u64(a, b);
	uint64_t y = calc_min_u64(a, b);

	while(y)
	{
		uint64_t z = x % y;
		x = y;
		y = z;
	}

	return x;
}




// ----------------------------------------------------------------------------------------------- //
// Extended Euclidean Algorithm                                                                    //
// ----------------------------------------------------------------------------------------------- //
/* calc_ext_gcd *********************************************************************************//**
 * @brief		Calculates the Greatest Common Divisor (GCD) of two numbers and outputs Bezout's
 * 				coefficients s and t defined as gcd(a,b) = r = a*s + b*t. */
#define calc_ext_gcd(a, b, s, t) _Generic((a) + (b) + (*(s)) + (*(t)), \
	int8_t:   calc_ext_gcd_int (a, b, s, t), \
	int16_t:  calc_ext_gcd_int (a, b, s, t), \
	int32_t:  calc_ext_gcd_i32 (a, b, s, t), \
	int64_t:  calc_ext_gcd_i64 (a, b, s, t), \
	uint8_t:  calc_ext_gcd_uint(a, b, s, t), \
	uint16_t: calc_ext_gcd_uint(a, b, s, t), \
	uint32_t: calc_ext_gcd_u32 (a, b, s, t), \
	uint64_t: calc_ext_gcd_u64 (a, b, s, t))


inline int calc_ext_gcd_int(int a, int b, int* s, int* t)
{
	int s1 = 0, s0 = 1;
	int t1 = 1, t0 = 0;
	int r1 = b, r0 = a;

	while(r1)
	{
		int q  = r0 / r1;
		int ri = r0 - q*r1;
		int si = s0 - q*s1;
		int ti = t0 - q*t1;

		r0 = r1; r1 = ri;
		s0 = s1; s1 = si;
		t0 = t1; t1 = ti;
	}

	*s = s0;
	*t = t0;
	return r0;
}


inline int32_t calc_ext_gcd_i32(int32_t a, int32_t b, int32_t* s, int32_t* t)
{
	int32_t s1 = 0, s0 = 1;
	int32_t t1 = 1, t0 = 0;
	int32_t r1 = b, r0 = a;

	while(r1)
	{
		int32_t q  = r0 / r1;
		int32_t ri = r0 - q*r1;
		int32_t si = s0 - q*s1;
		int32_t ti = t0 - q*t1;

		r0 = r1; r1 = ri;
		s0 = s1; s1 = si;
		t0 = t1; t1 = ti;
	}

	*s = s0;
	*t = t0;
	return r0;
}


inline int64_t calc_ext_gcd_i64(int64_t a, int64_t b, int64_t* s, int64_t* t)
{
	int64_t s1 = 0, s0 = 1;
	int64_t t1 = 1, t0 = 0;
	int64_t r1 = b, r0 = a;

	while(r1)
	{
		int64_t q  = r0 / r1;
		int64_t ri = r0 - q*r1;
		int64_t si = s0 - q*s1;
		int64_t ti = t0 - q*t1;

		r0 = r1; r1 = ri;
		s0 = s1; s1 = si;
		t0 = t1; t1 = ti;
	}

	*s = s0;
	*t = t0;
	return r0;
}


inline unsigned calc_ext_gcd_uint(unsigned a, unsigned b, unsigned* s, unsigned* t)
{
	unsigned s1 = 0, s0 = 1;
	unsigned t1 = 1, t0 = 0;
	unsigned r1 = b, r0 = a;

	while(r1)
	{
		unsigned q  = r0 / r1;
		unsigned ri = r0 - q*r1;
		unsigned si = s0 - q*s1;
		unsigned ti = t0 - q*t1;

		r0 = r1; r1 = ri;
		s0 = s1; s1 = si;
		t0 = t1; t1 = ti;
	}

	*s = s0;
	*t = t0;
	return r0;
}


inline uint32_t calc_ext_gcd_u32(uint32_t a, uint32_t b, uint32_t* s, uint32_t* t)
{
	uint32_t s1 = 0, s0 = 1;
	uint32_t t1 = 1, t0 = 0;
	uint32_t r1 = b, r0 = a;

	while(r1)
	{
		uint32_t q  = r0 / r1;
		uint32_t ri = r0 - q*r1;
		uint32_t si = s0 - q*s1;
		uint32_t ti = t0 - q*t1;

		r0 = r1; r1 = ri;
		s0 = s1; s1 = si;
		t0 = t1; t1 = ti;
	}

	*s = s0;
	*t = t0;
	return r0;
}


inline uint64_t calc_ext_gcd_u64(uint64_t a, uint64_t b, uint64_t* s, uint64_t* t)
{
	uint64_t s1 = 0, s0 = 1;
	uint64_t t1 = 1, t0 = 0;
	uint64_t r1 = b, r0 = a;

	while(r1)
	{
		uint64_t q  = r0 / r1;
		uint64_t ri = r0 - q*r1;
		uint64_t si = s0 - q*s1;
		uint64_t ti = t0 - q*t1;

		r0 = r1; r1 = ri;
		s0 = s1; s1 = si;
		t0 = t1; t1 = ti;
	}

	*s = s0;
	*t = t0;
	return r0;
}





// ----------------------------------------------------------------------------------------------- //
// LCM                                                                                             //
// ----------------------------------------------------------------------------------------------- //
/* calc_lcm ************************************************************************************//**
 * @brief		Calculates the least common multiple of a and b. */
#define calc_lcm(a, b) _Generic((a) + (b), \
	int8_t:   calc_lcm_int (a, b), \
	int16_t:  calc_lcm_int (a, b), \
	int32_t:  calc_lcm_i32 (a, b), \
	int64_t:  calc_lcm_i64 (a, b), \
	uint8_t:  calc_lcm_uint(a, b), \
	uint16_t: calc_lcm_uint(a, b), \
	uint32_t: calc_lcm_u32 (a, b), \
	uint64_t: calc_lcm_u64 (a, b))


inline int calc_lcm_int(int a, int b)
{
	return calc_abs_int(a) / calc_gcd_int(a, b) * calc_abs_int(b);
}


inline int32_t calc_lcm_i32(int32_t a, int32_t b)
{
	return calc_abs_i32(a) / calc_gcd_i32(a, b) * calc_abs_i32(b);
}


inline int64_t calc_lcm_i64(int64_t a, int64_t  b)
{
	return calc_abs_i64(a) / calc_gcd_i64(a, b) * calc_abs_i64(b);
}


inline unsigned calc_lcm_uint(unsigned a, unsigned b)
{
	return calc_abs_uint(a) / calc_gcd_uint(a, b) * calc_abs_uint(b);
}


inline uint32_t calc_lcm_u32(uint32_t a, uint32_t b)
{
	return calc_abs_u32(a) / calc_gcd_u32(a, b) * calc_abs_u32(b);
}


inline uint64_t calc_lcm_u64(uint64_t a, uint64_t b)
{
	return calc_abs_u64(a) / calc_gcd_u64(a, b) * calc_abs_u64(b);
}





// ----------------------------------------------------------------------------------------------- //
// ax + by = c                                                                                     //
// ----------------------------------------------------------------------------------------------- //
/* calc_ax_by_c *********************************************************************************//**
 * @brief		Solves the linear Diophantine equation ax + by = c. Outputs solutions where x0 is
 * 				closest to 0 and positive, and x increases as n increases. Returns false if no
 * 				solution. The solutions are returned into x and y arrays and are interpreted like:
 *
 * 					x = x0 - b/gcd(a,b)*n = x[0] + x[1]*n
 * 					y = y0 + a/gcd(a,b)*n = y[0] + y[1]*n
 *
 * @desc		Diophantine equations can be solved with the Extended Euclidean Algorithm. The
 * 				Extended Euclidean Algorithm finds integers s and t such that
 *
 * 					as + bt = gcd(a,b)
 *
 * 				Find an integer k:
 *
 * 					a(sk) + b(tk) = gcd(a,b)*k = c => k = c / gcd(a,b)
 *
 * 				Which also gives a solution to the Diophantine equation:
 *
 * 					x0 = sk
 * 					y0 = tk
 *
 * 				Now the general solution to the Diophatine equation is:
 *
 * 					x = x0 - b/gcd(a,b)*n = s*k - b/gcd(a,b)*n
 * 					y = y0 + a/gcd(a,b)*n = t*k + a/gcd(a,b)*n
 */
#define calc_ax_by_c(a, b, c, x, y) calc_ax_by_c_int(a, b, c, x, y);


inline bool calc_ax_by_c_int(int a, int b, int c, int x[2], int y[2])
{
	int s, t;
	int gcd = calc_ext_gcd_int(a, b, &s, &t);
	int k   = c / gcd;

	/* No solution if gcd(a,b) does not evenly divide c. */
	if(c % gcd != 0)
	{
		return false;
	}

	int n = s*k / (b / gcd);
	x[0]  = s*k - (b / gcd) * n; x[1] = -b / gcd;
	y[0]  = t*k + (a / gcd) * n; y[1] =  a / gcd;

	/* Always ensure +x with +n */
	if(x[1] < 0)
	{
		x[1] *= -1;
		y[1] *= -1;
	}

	/* Set x0 closest to +0 */
	if(x[0] < 0)
	{
		x[0] += x[1];
		y[0] += y[1];
	}

	return true;
}





// ----------------------------------------------------------------------------------------------- //
// ax^2 + bx + c = 0                                                                               //
// ----------------------------------------------------------------------------------------------- //
/* calc_ax2_bx_c ********************************************************************************//**
 * @brief		Solves the quadratic equation ax^2 + bx + c = 0 while avoiding catastrophic
 * 				cancellation. The two solutions are stored in x0 and x1. The solutions x0 and x1
 * 				are sorted such that x0 < x1. */
#define calc_ax2_bx_c(a, b, c, x0, x1) calc_ax2_bx_c_f((a), (b), (c), (x0), (x1))

inline void calc_ax2_bx_c_f(float a, float b, float c, float* x0, float* x1)
{
	float q = -0.5f * (b + copysignf(sqrtf(calc_dop_f(b, b, 4.0f*a, c)), b));
	float r = q / a;
	float s = c / q;

	if(x0) { *x0 = calc_min_f(r, s); }
	if(x1) { *x1 = calc_max_f(r, s); }
}





// ----------------------------------------------------------------------------------------------- //
// LERP                                                                                            //
// ----------------------------------------------------------------------------------------------- //
/* calc_lerp ************************************************************************************//**
 * @brief		Calculates a linear interpolation between two points (v0, v1) for a parameter (t) in
 * 				the closed interval [0, 1]. */
#define calc_lerp(x) _Generic((x), \
	float:  calc_lerp_f(x), \
	double: calc_lerp_d(x))


inline float  calc_lerp_f(float  v0, float  v1, float  t) { return (1.0f - t) * v0 + t * v1; }
inline double calc_lerp_d(double v0, double v1, double t) { return (1.0f - t) * v0 + t * v1; }





// ----------------------------------------------------------------------------------------------- //
// Deg / Rad                                                                                       //
// ----------------------------------------------------------------------------------------------- //
/* calc_rad_to_deg ******************************************************************************//**
 * @brief		Converts x from radians to degrees. */
#define calc_rad_to_deg(x) _Generic((x), \
	float:  calc_rad_to_deg_f(x), \
	double: calc_rad_to_deg_d(x))


/* calc_deg_to_rad ******************************************************************************//**
 * @brief		Converts x from degrees to radians. */
#define calc_deg_to_rad(x) _Generic((x), \
	float:  calc_deg_to_rad_f(x), \
	double: calc_deg_to_rad_d(x))

inline float  calc_rad_to_deg_f(float  x) { return x * (float)(180.0 / M_PI); }
inline double calc_rad_to_deg_d(double x) { return x *         180.0 / M_PI;  }
inline float  calc_deg_to_rad_f(float  x) { return x * (float)(M_PI / 180.0); }
inline double calc_deg_to_rad_d(double x) { return x *         M_PI / 180.0;  }





// ----------------------------------------------------------------------------------------------- //
// Map                                                                                             //
// ----------------------------------------------------------------------------------------------- //
/* calc_map_360 *********************************************************************************//**
 * @brief		Maps a number to the closed interval [0, 360]. Positive multiples of 360 are mapped
 * 				to 360. Negative multiples of 360 are mapped to 0.
 * @desc		Example output:
 *
 * 					 0   -> 0
 * 					 360 -> 360
 * 					-360 -> 0
 * 					 361 -> 1
 * 					-361 -> 359 */
#define calc_map_360(x) _Generic((x), \
	float:  calc_mapf_360(x), \
	double: calc_mapd_360(x))


/* calc_map_2pi *********************************************************************************//**
 * @brief		Maps a number to the closed interval [0, 2*pi]. Positive multiples of 2*pi are mapped
 * 				to 2*pi. Negative multiples of 2*pi are mapped to 0.
 * @desc		Example output:
 *
 * 					 0     -> 0
 * 					 2pi   -> 2pi
 * 					-2pi   -> 0
 * 					 2pi+1 -> 1
 * 					-2pi-1 -> 2pi-1 */
#define calc_map_2pi(x) _Generic((x), \
	float:  calc_mapf_2pi(x), \
	double: calc_mapd_2pi(x))


/* calc_map_180 *********************************************************************************//**
 * @brief		Maps a number to the closed interval [-180, 180].
 * @desc		Example output:
 *
 * 					 0     ->  0
 * 					 180   ->  180
 * 					-180   ->  180
 * 					 181   -> -179
 * 					-181   ->  179 */
#define calc_map_180(x) _Generic((x), \
	float:  calc_mapf_180(x), \
	double: calc_mapd_180(x))


/* calc_map_pi **********************************************************************************//**
 * @brief		Maps a number to the closed interval [-pi, pi].
 * @desc		Example output:
 *
 * 					 0     ->  0
 * 					 pi    ->  pi
 * 					-pi    -> -pi
 * 					 pi+1  -> -pi+1
 * 					-pi-1  ->  pi-1 */
#define calc_map_pi(x) _Generic((x), \
	float:  calc_mapf_pi(x), \
	double: calc_mapd_pi(x))


inline float calc_mapf_360(float x)
{
	float wrap = calc_mod_f(x, 360);

	if(wrap == 0 && x > 0)
	{
		wrap = 360;
	}

	return wrap;
}


inline double calc_mapd_360(double x)
{
	double wrap = calc_mod_d(x, 360);

	if(wrap == 0 && x > 0)
	{
		wrap = 360;
	}

	return wrap;
}


inline float calc_mapf_2pi(float x)
{
	float wrap = calc_mod_f(x, M_2PI);

	if(wrap == 0 && x > 0)
	{
		wrap = M_2PI;
	}

	return wrap;
}


inline double calc_mapd_2pi(double x)
{
	double wrap = calc_mod_d(x, M_2PI);

	if(wrap == 0 && x > 0)
	{
		wrap = M_2PI;
	}

	return wrap;
}


inline float calc_mapf_180(float x)
{
	return fabsf(x) <= 180.0f ? x : calc_mapf_360(x + 180.0f) - 180.0f;
}


inline double calc_mapd_180(double x)
{
	return fabs(x) <= 180.0 ? x : calc_mapd_360(x + 180.0) - 180.0;
}


inline float calc_mapf_pi(float x)
{
	return fabsf(x) <= (float)M_PI ? x : calc_mapf_2pi(x + (float)M_PI) - (float)M_PI;
}


inline double calc_mapd_pi(double x)
{
	return fabs(x) <= M_PI ? x : calc_mapd_2pi(x + M_PI) - M_PI;
}





// ----------------------------------------------------------------------------------------------- //
// Comparing Floats                                                                                //
// ----------------------------------------------------------------------------------------------- //
/* calc_within **********************************************************************************//**
 * @brief		Returns true if 'a' is within a user specified epsilon of 'b'. */
#define calc_within(a,b,eps)	_Generic((a)+(b)+(eps), \
	float: calc_within_f((a),(b),(eps)), \
	double: calc_within_d((a),(b),(eps)))

/* calc_compare *********************************************************************************//**
 * @brief		Returns true if 'a' is within FLT_EPSILON / DBL_EPSILON of 'b'. */
#define calc_compare(a,b)	_Generic((a)+(b), \
	float:  calc_compare_f((a),(b)), \
	double: calc_compare_d((a),(b)))

inline bool calc_within_f(float a, float b, float eps)
{
	return fabsf(a - b) <= (calc_max_f(1.0, calc_max_f(a, b)) * eps);
}

inline bool calc_within_d(double a, double b, double eps)
{
	return fabs(a - b) <= (calc_max_d(1.0, calc_max_d(a, b)) * eps);
}

inline int calc_compare_f(float a, float b)
{
	float  diff = a-b;
	float  eps  = calc_max_f(1.0f, calc_max_f(a, b)) * FLT_EPSILON;
	return diff < -eps ? -1 :
	       diff >  eps ? +1 : 0;
}

inline int calc_compare_d(double a, double b)
{
	double  diff = a-b;
	double  eps  = calc_max_d(1.0, calc_max_d(a, b)) * DBL_EPSILON;
	return  diff < -eps ? -1 :
	        diff >  eps ? +1 : 0;
}





// ----------------------------------------------------------------------------------------------- //
// Random                                                                                          //
// ----------------------------------------------------------------------------------------------- //
/* calc_randf ***********************************************************************************//**
 * @brief		Returns a random single precision float between 0.0 and 1.0. */
inline float calc_randf(void)
{
	return (float)(rand()) / (float)(RAND_MAX);
}


/* calc_randf_range *****************************************************************************//**
 * @brief		Returns a random single precision float between [min, max]. */
inline float calc_randf_range(float min, float max)
{
	float hi = calc_max_f(min, max);
	float lo = calc_min_f(min, max);
	return calc_randf() * (hi - lo) + lo;
}


/* calc_rand_range ******************************************************************************//**
 * @brief		Returns a random number between [min, max]. */
inline int calc_rand_range(int min, int max)
{
	/* Divide RAND_MAX into n equally sized slots */
	int r;
	int n = max - min + 1;
	// int blocksize = (RAND_MAX + 1u) / n;
	int blocksize = (RAND_MAX) / n;

	/* Keep rolling r until it is within range */
	do {
		r = rand();
	} while(r >= blocksize * n);

	return min + r / blocksize;
}


#ifdef __cplusplus
}
#endif

#endif // CALC_H
/******************************************* END OF FILE *******************************************/
