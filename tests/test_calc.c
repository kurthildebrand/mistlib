/************************************************************************************************//**
 * @file		test_calc.c
 *
 * @copyright	Copyright 2022 Kurt Hildebrand.
 * @license		Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
 * 				file except in compliance with the License. You may obtain a copy of the License at
 *
 * 				http://www.apache.org/licenses/LICENSE-2.0
 *
 * 				Unless required by applicable law or agreed to in writing, software distributed under
 * 				the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
 * 				ANY KIND, either express or implied. See the License for the specific language
 * 				governing permissions and limitations under the License.
 *
 ***************************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <limits.h>

#include "calc.h"
#include "tharness.h"



// ----------------------------------------------------------------------------------------------- //
// Sign                                                                                            //
// ----------------------------------------------------------------------------------------------- //
TEST(test_sign)
{
	EXPECT(+1 == calc_sign(50));
	EXPECT(-1 == calc_sign(-50));
	EXPECT(+1 == calc_sign(0));
}


TEST(test_sign_i32)
{
	EXPECT(+1 == calc_sign_i32(50));
	EXPECT(-1 == calc_sign_i32(-50));
	EXPECT(+1 == calc_sign_i32(0));
}


TEST(test_sign_i64)
{
	EXPECT(+1 == calc_sign_i64(50));
	EXPECT(-1 == calc_sign_i64(-50));
	EXPECT(+1 == calc_sign_i64(0));
}


TEST(test_sign_uint)
{
	EXPECT(+1 == calc_sign_uint(50));
	EXPECT(+1 == calc_sign_uint(0));
}


TEST(test_sign_u32)
{
	EXPECT(+1 == calc_sign_u32(50));
	EXPECT(+1 == calc_sign_u32(0));
}


TEST(test_sign_u64)
{
	EXPECT(+1 == calc_sign_u64(50));
	EXPECT(+1 == calc_sign_u64(0));
}


TEST(test_sign_f)
{
	EXPECT(+1.0f == calc_sign_f(50.0f));
	EXPECT(-1.0f == calc_sign_f(-50.0f));
	EXPECT(+1.0f == calc_sign_i32(0.0f));
}


TEST(test_sign_d)
{
	EXPECT(+1.0 == calc_sign_f(50.0));
	EXPECT(-1.0 == calc_sign_f(-50.0));
	EXPECT(+1.0 == calc_sign_i32(0.0));
}





// ----------------------------------------------------------------------------------------------- //
// Abs                                                                                             //
// ----------------------------------------------------------------------------------------------- //
TEST(test_calc_abs_int)
{
	EXPECT(5 == calc_abs_int(-5));
	EXPECT(5 == calc_abs_int(5));
	EXPECT(1234 == calc_abs_int(-1234));
	EXPECT(1234 == calc_abs_int(1234));
	EXPECT(INT_MIN == calc_abs_int(INT_MIN));
}


TEST(test_calc_abs_i32)
{
	EXPECT(5 == calc_abs_i32(-5));
	EXPECT(5 == calc_abs_i32(5));
	EXPECT(1234 == calc_abs_i32(-1234));
	EXPECT(1234 == calc_abs_i32(1234));
}


TEST(test_calc_abs_i64)
{
	EXPECT(5 == calc_abs_i64(-5));
	EXPECT(5 == calc_abs_i64(5));
	EXPECT(1234 == calc_abs_i64(-1234));
	EXPECT(1234 == calc_abs_i64(1234));
}


TEST(test_calc_abs_f)
{
	EXPECT(5.0f == calc_abs_f(-5.0f));
	EXPECT(5.0f == calc_abs_f(5.0f));
	EXPECT(1234.0f == calc_abs_f(-1234.0f));
	EXPECT(1234.0f == calc_abs_f(1234.0f));
}


TEST(test_calc_abs_d)
{
	EXPECT(5.0 == calc_abs_d(-5.0));
	EXPECT(5.0 == calc_abs_d(5.0));
	EXPECT(1234.0 == calc_abs_d(-1234.0));
	EXPECT(1234.0 == calc_abs_d(1234.0));
}





// ----------------------------------------------------------------------------------------------- //
// Max                                                                                             //
// ----------------------------------------------------------------------------------------------- //
TEST(test_calc_max)
{
	EXPECT(20 == calc_max(10, 20));
	EXPECT(20 == calc_max(20, 10));
}


TEST(test_calc_max_i32)
{
	EXPECT(INT32_MAX == calc_max_i32(INT32_MIN, INT32_MAX));
	EXPECT(INT32_MAX == calc_max_i32(INT32_MAX, INT32_MIN));
}


TEST(test_calc_max_i64)
{
	EXPECT(INT64_MAX == calc_max_i64(INT64_MIN, INT64_MAX));
	EXPECT(INT64_MAX == calc_max_i64(INT64_MAX, INT64_MIN));
}


TEST(test_calc_max_uint)
{
	EXPECT(20 == calc_max_uint(10, 20));
	EXPECT(20 == calc_max_uint(20, 10));
}


TEST(test_calc_max_u32)
{
	EXPECT(UINT32_MAX == calc_max_u32(0, UINT32_MAX));
	EXPECT(UINT32_MAX == calc_max_u32(UINT32_MAX, 0));
}


TEST(test_calc_max_u64)
{
	EXPECT(UINT64_MAX == calc_max_u64(0, UINT64_MAX));
	EXPECT(UINT64_MAX == calc_max_u64(UINT64_MAX, 0));
}


TEST(test_calc_max_f)
{
	EXPECT(12357831.0f == calc_max_f(-3.1415f, 12357831.0f));
	EXPECT(12357831.0f == calc_max_f(12357831.0f, -3.1415));
}


TEST(test_calc_max_d)
{
	EXPECT(20 == calc_max_d(10, 20));
	EXPECT(20 == calc_max_d(20, 10));
}





// ----------------------------------------------------------------------------------------------- //
// Min                                                                                             //
// ----------------------------------------------------------------------------------------------- //
TEST(test_calc_min)
{
	EXPECT(10 == calc_min(10, 20));
	EXPECT(10 == calc_min(20, 10));
}


TEST(test_calc_min_i32)
{
	EXPECT(INT32_MIN == calc_min_i32(INT32_MIN, INT32_MAX));
	EXPECT(INT32_MIN == calc_min_i32(INT32_MAX, INT32_MIN));
}


TEST(test_calc_min_i64)
{
	EXPECT(INT64_MIN == calc_min_i64(INT64_MIN, INT64_MAX));
	EXPECT(INT64_MIN == calc_min_i64(INT64_MAX, INT64_MIN));
}


TEST(test_calc_min_uint)
{
	EXPECT(10 == calc_min_uint(10, 20));
	EXPECT(10 == calc_min_uint(20, 10));
}


TEST(test_calc_min_u32)
{
	EXPECT(0 == calc_min_u32(0, UINT32_MAX));
	EXPECT(0 == calc_min_u32(UINT32_MAX, 0));
}


TEST(test_calc_min_u64)
{
	EXPECT(0 == calc_min_u64(0, UINT64_MAX));
	EXPECT(0 == calc_min_u64(UINT64_MAX, 0));
}


TEST(test_calc_min_f)
{
	EXPECT((float)-3.1415f == calc_min_f(-3.1415f, 123578310.0f));
	EXPECT((float)-3.1415f == calc_min_f(123578310.0f, -3.1415f));
}


TEST(test_calc_min_d)
{
	EXPECT(10.0 == calc_min_d(10, 20));
	EXPECT(10.0 == calc_min_d(20, 10));
}





// ----------------------------------------------------------------------------------------------- //
// Avg                                                                                             //
// ----------------------------------------------------------------------------------------------- //
TEST(test_calc_avg)
{
	EXPECT(-1 == calc_avg(-6, 3));
	EXPECT(-1 == calc_avg(3, -6));

	EXPECT(0 == calc_avg(1, -2));
	EXPECT(0 == calc_avg(-2, 1));

	EXPECT(INT_MAX == calc_avg(INT_MAX, INT_MAX));
	EXPECT(INT_MIN == calc_avg(INT_MIN, INT_MIN));
	EXPECT(0 == calc_avg(INT_MAX, INT_MIN));
}


TEST(test_calc_avg_i32)
{
	EXPECT(-1 == calc_avg_i32(-6, 3));
	EXPECT(-1 == calc_avg_i32(3, -6));

	EXPECT(0 == calc_avg_i32(1, -2));
	EXPECT(0 == calc_avg_i32(-2, 1));

	EXPECT(INT32_MAX == calc_avg_i32(INT32_MAX, INT32_MAX));

	EXPECT(INT32_MIN == calc_avg_i32(INT32_MIN, INT32_MIN));
}


TEST(test_calc_avg_i64)
{
	EXPECT(-1 == calc_avg_i64(-6, 3));
	EXPECT(-1 == calc_avg_i64(3, -6));

	EXPECT(0 == calc_avg_i64(1, -2));
	EXPECT(0 == calc_avg_i64(-2, 1));

	EXPECT(INT64_MAX == calc_avg_i64(INT64_MAX, INT64_MAX));

	EXPECT(INT64_MIN == calc_avg_i64(INT64_MIN, INT64_MIN));
}


TEST(test_calc_avg_uint)
{
	EXPECT(2 == calc_avg_uint(1, 4));
	EXPECT(2 == calc_avg_uint(4, 1));

	EXPECT(UINT_MAX == calc_avg_uint(UINT_MAX, UINT_MAX));
}


TEST(test_calc_avg_u32)
{
	EXPECT(2 == calc_avg_u32(1, 4));
	EXPECT(2 == calc_avg_u32(4, 1));

	EXPECT(UINT32_MAX == calc_avg_u32(UINT32_MAX, UINT32_MAX));
}


TEST(test_calc_avg_u64)
{
	EXPECT(2 == calc_avg_u64(1, 4));
	EXPECT(2 == calc_avg_u64(4, 1));

	EXPECT(UINT64_MAX == calc_avg_u64(UINT64_MAX, UINT64_MAX));
}


TEST(test_calc_avg_f)
{
	EXPECT(2.5f == calc_avg_f(2, 3));
	EXPECT(2.5f == calc_avg_f(3, 2));
}


TEST(test_calc_avg_d)
{
	EXPECT(2.5 == calc_avg_d(2, 3));
	EXPECT(2.5 == calc_avg_d(3, 2));
}





// ----------------------------------------------------------------------------------------------- //
// Bit-twiddling                                                                                   //
// ----------------------------------------------------------------------------------------------- //
TEST(test_calc_log2)
{
	EXPECT(3  == calc_log2(12));
	EXPECT(8  == calc_log2(500));
	EXPECT(14 == calc_log2(16857));
}


TEST(test_calc_clp2)
{
	EXPECT(16    == calc_clp2(12));
	EXPECT(512   == calc_clp2(500));
	EXPECT(32768 == calc_clp2(16857));
}


TEST(test_calc_flp2)
{
	EXPECT(8     == calc_flp2(12));
	EXPECT(256   == calc_flp2(500));
	EXPECT(16384 == calc_flp2(16857));
}


TEST(test_calc_popcount_u16)
{
	EXPECT(0  == calc_popcount_u16(0x0000));
	EXPECT(16 == calc_popcount_u16(0xFFFF));
	EXPECT(4  == calc_popcount_u16(0x8421));
	EXPECT(7  == calc_popcount_u16(0x1C3C));
	EXPECT(8  == calc_popcount_u16(0xFF00));
}


TEST(test_calc_popcount_u32)
{
	EXPECT(0  == calc_popcount_u32(0x00000000));
	EXPECT(32 == calc_popcount_u32(0xFFFFFFFF));
	EXPECT(6  == calc_popcount_u32(0x43002021));
	EXPECT(16 == calc_popcount_u32(0xFF3C8421));
}


TEST(test_calc_popcount_u64)
{
	EXPECT(0  == calc_popcount_u64(0x0000000000000000));
	EXPECT(64 == calc_popcount_u64(0xFFFFFFFFFFFFFFFF));
	EXPECT(28 == calc_popcount_u64(0x3CF00FF03C3C8421));
	EXPECT(24 == calc_popcount_u64(0x532050848CE394A6));
}





// ----------------------------------------------------------------------------------------------- //
// Round                                                                                           //
// ----------------------------------------------------------------------------------------------- //
TEST(test_calc_round)
{
	EXPECT(-20 == calc_round(-22, 10));
	EXPECT(-20 == calc_round(-18, 10));
	EXPECT(-10 == calc_round(-11, 10));
	EXPECT(-10 == calc_round(-9,  10));
	EXPECT(  0 == calc_round(-4,  10));
	EXPECT( 10 == calc_round( 7,  10));
	EXPECT( 10 == calc_round( 14, 10));
	EXPECT( 20 == calc_round( 19, 10));
	EXPECT( 20 == calc_round( 21, 10));
	EXPECT( 20 == calc_round( 24, 10));
	EXPECT( 30 == calc_round( 25, 10));
}


TEST(test_round_i32)
{
	EXPECT(-20 == calc_round_i32(-22, 10));
	EXPECT(-20 == calc_round_i32(-18, 10));
	EXPECT(-10 == calc_round_i32(-11, 10));
	EXPECT(-10 == calc_round_i32(-9,  10));
	EXPECT(  0 == calc_round_i32(-4,  10));
	EXPECT( 10 == calc_round_i32( 7,  10));
	EXPECT( 10 == calc_round_i32( 14, 10));
	EXPECT( 20 == calc_round_i32( 19, 10));
	EXPECT( 20 == calc_round_i32( 21, 10));
	EXPECT( 20 == calc_round_i32( 24, 10));
	EXPECT( 30 == calc_round_i32( 25, 10));
}


TEST(test_round_i64)
{
	EXPECT(-20 == calc_round_i64(-22, 10));
	EXPECT(-20 == calc_round_i64(-18, 10));
	EXPECT(-10 == calc_round_i64(-11, 10));
	EXPECT(-10 == calc_round_i64(-9,  10));
	EXPECT(  0 == calc_round_i64(-4,  10));
	EXPECT( 10 == calc_round_i64( 7,  10));
	EXPECT( 10 == calc_round_i64( 14, 10));
	EXPECT( 20 == calc_round_i64( 19, 10));
	EXPECT( 20 == calc_round_i64( 21, 10));
	EXPECT( 20 == calc_round_i64( 24, 10));
	EXPECT( 30 == calc_round_i64( 25, 10));
}


TEST(test_round_uint)
{
	EXPECT( 10 == calc_round_uint( 7,  10));
	EXPECT( 10 == calc_round_uint( 14, 10));
	EXPECT( 20 == calc_round_uint( 19, 10));
	EXPECT( 20 == calc_round_uint( 21, 10));
	EXPECT( 20 == calc_round_uint( 24, 10));
	EXPECT( 30 == calc_round_uint( 25, 10));
}


TEST(test_round_u32)
{
	EXPECT( 10 == calc_round_u32( 7,  10));
	EXPECT( 10 == calc_round_u32( 14, 10));
	EXPECT( 20 == calc_round_u32( 19, 10));
	EXPECT( 20 == calc_round_u32( 21, 10));
	EXPECT( 20 == calc_round_u32( 24, 10));
	EXPECT( 30 == calc_round_u32( 25, 10));
}


TEST(test_round_u64)
{
	EXPECT( 10 == calc_round_u64( 7,  10));
	EXPECT( 10 == calc_round_u64( 14, 10));
	EXPECT( 20 == calc_round_u64( 19, 10));
	EXPECT( 20 == calc_round_u64( 21, 10));
	EXPECT( 20 == calc_round_u64( 24, 10));
	EXPECT( 30 == calc_round_u64( 25, 10));
}


TEST(test_round_f)
{
	EXPECT(-20.0f == calc_round_f(-22.0f, 10.0f));
	EXPECT(-20.0f == calc_round_f(-18.0f, 10.0f));
	EXPECT(-10.0f == calc_round_f(-11.0f, 10.0f));
	EXPECT(-10.0f == calc_round_f(-9.0f,  10.0f));
	EXPECT(  0.0f == calc_round_f(-4.0f,  10.0f));
	EXPECT( 10.0f == calc_round_f( 7.0f,  10.0f));
	EXPECT( 10.0f == calc_round_f( 14.0f, 10.0f));
	EXPECT( 20.0f == calc_round_f( 19.0f, 10.0f));
	EXPECT( 20.0f == calc_round_f( 21.0f, 10.0f));
	EXPECT( 20.0f == calc_round_f( 24.0f, 10.0f));
	EXPECT( 20.0f == calc_round_f( 24.4f, 10.0f));
	EXPECT( 30.0f == calc_round_f( 25.0f, 10.0f));
	EXPECT( 30.0f == calc_round_f( 26.7f, 10.0f));
}


TEST(test_round_d)
{
	EXPECT(-20.0 == calc_round_d(-22.0, 10.0));
	EXPECT(-20.0 == calc_round_d(-18.0, 10.0));
	EXPECT(-10.0 == calc_round_d(-11.0, 10.0));
	EXPECT(-10.0 == calc_round_d(-9.0,  10.0));
	EXPECT(  0.0 == calc_round_d(-4.0,  10.0));
	EXPECT( 10.0 == calc_round_d( 7.0,  10.0));
	EXPECT( 10.0 == calc_round_d( 14.0, 10.0));
	EXPECT( 20.0 == calc_round_d( 19.0, 10.0));
	EXPECT( 20.0 == calc_round_d( 21.0, 10.0));
	EXPECT( 20.0 == calc_round_d( 24.4, 10.0));
	EXPECT( 30.0 == calc_round_d( 25.0, 10.0));
	EXPECT( 30.0 == calc_round_d( 26.7, 10.0));
}





// ----------------------------------------------------------------------------------------------- //
// Mod                                                                                             //
// ----------------------------------------------------------------------------------------------- //
TEST(test_calc_mod)
{
	EXPECT(340 == calc_mod(-20, 360));
}


TEST(test_calc_mod_i32)
{
	EXPECT(340 == calc_mod_i32(-20, 360));
}


TEST(test_calc_mod_i64)
{
	EXPECT(340 == calc_mod_i64(-20, 360));
}


TEST(test_calc_mod_f)
{
	EXPECT(340.0f == calc_mod_f(-20, 360));
}


TEST(test_calc_mod_d)
{
	EXPECT(340.0 == calc_mod_d(-20, 360));
}





// ----------------------------------------------------------------------------------------------- //
// Addmod                                                                                          //
// ----------------------------------------------------------------------------------------------- //
TEST(test_calc_addmod_u32)
{
	EXPECT(587500000 == calc_addmod_u32(4112620000, 587520000, 4112640000));
}





// ----------------------------------------------------------------------------------------------- //
// Wrapdiff                                                                                        //
// ----------------------------------------------------------------------------------------------- //
TEST(test_calc_wrapdiff)
{
	EXPECT(-20 == calc_wrapdiff(350, 10, 360));
	EXPECT( 20 == calc_wrapdiff(10, 350, 360));
}


TEST(test_calc_wrapdiff_i32)
{
	EXPECT(-20 == calc_wrapdiff_i32(350, 10, 360));
	EXPECT( 20 == calc_wrapdiff_i32(10, 350, 360));
}


TEST(test_calc_wrapdiff_i64)
{
	EXPECT(-20 == calc_wrapdiff_i64(350, 10, 360));
	EXPECT( 20 == calc_wrapdiff_i64(10, 350, 360));
}


TEST(test_calc_wrapdiff_uint)
{
	EXPECT(-20 == calc_wrapdiff_uint(350, 10, 360));
	EXPECT( 20 == calc_wrapdiff_uint(10, 350, 360));
	EXPECT(-10000 == calc_wrapdiff_uint(50000, 60000, 64000));
	EXPECT( 10000 == calc_wrapdiff_uint(60000, 50000, 64000));
}


TEST(test_calc_wrapdiff_u32)
{
	EXPECT(-20 == calc_wrapdiff_u32(350, 10, 360));
	EXPECT( 20 == calc_wrapdiff_u32(10, 350, 360));
	EXPECT(-1 == calc_wrapdiff_uint(3999999999, 4000000000, 4200000000));
	EXPECT( 1 == calc_wrapdiff_uint(4000000000, 3999999999, 4200000000));
}


TEST(test_calc_wrapdiff_u64)
{
	EXPECT(-20 == calc_wrapdiff_u64(350, 10, 360));
	EXPECT( 20 == calc_wrapdiff_u64(10, 350, 360));
	EXPECT(-100000000000000000 == calc_wrapdiff_u64(18300000000000000000ull, 18400000000000000000ull, 18440000000000000000ull));
	EXPECT( 100000000000000000 == calc_wrapdiff_u64(18400000000000000000ull, 18300000000000000000ull, 18440000000000000000ull));
}


TEST(test_calc_wrapdiff_f)
{
	EXPECT(-20.0f == calc_wrapdiff_f(350.0f, 10.0f, 360.0f));
	EXPECT( 20.0f == calc_wrapdiff_f(10.0f, 350.0f, 360.0f));
}


TEST(test_calc_wrapdiff_d)
{
	EXPECT(-20.0 == calc_wrapdiff_d(350.0, 10.0, 360.0));
	EXPECT( 20.0 == calc_wrapdiff_d(10.0, 350.0, 360.0));
}





// ----------------------------------------------------------------------------------------------- //
// GCD                                                                                             //
// ----------------------------------------------------------------------------------------------- //
TEST(test_calc_gcd)
{
	EXPECT(6   == calc_gcd(12, 18));
	EXPECT(6   == calc_gcd(18, 12));
	EXPECT(139 == calc_gcd(1529, 14039));
	EXPECT(139 == calc_gcd(14039, 1529));
	EXPECT(1   == calc_gcd(123, 277));
	EXPECT(1   == calc_gcd(277, 123));
	EXPECT(123 == calc_gcd(123, 0));
	EXPECT(123 == calc_gcd(0, 123));
}


TEST(test_calc_gcd_i32)
{
	EXPECT(6   == calc_gcd_i32(12, 18));
	EXPECT(6   == calc_gcd_i32(18, 12));
	EXPECT(139 == calc_gcd_i32(1529, 14039));
	EXPECT(139 == calc_gcd_i32(14039, 1529));
	EXPECT(1   == calc_gcd_i32(123, 277));
	EXPECT(1   == calc_gcd_i32(277, 123));
	EXPECT(123 == calc_gcd_i32(123, 0));
	EXPECT(123 == calc_gcd_i32(0, 123));
}


TEST(test_calc_gcd_i64)
{
	EXPECT(6   == calc_gcd_i64(12, 18));
	EXPECT(6   == calc_gcd_i64(18, 12));
	EXPECT(139 == calc_gcd_i64(1529, 14039));
	EXPECT(139 == calc_gcd_i64(14039, 1529));
	EXPECT(1   == calc_gcd_i64(123, 277));
	EXPECT(1   == calc_gcd_i64(277, 123));
	EXPECT(123 == calc_gcd_i64(123, 0));
	EXPECT(123 == calc_gcd_i64(0, 123));
}


TEST(test_calc_gcd_uint)
{
	EXPECT(6   == calc_gcd_uint(12, 18));
	EXPECT(6   == calc_gcd_uint(18, 12));
	EXPECT(139 == calc_gcd_uint(1529, 14039));
	EXPECT(139 == calc_gcd_uint(14039, 1529));
	EXPECT(1   == calc_gcd_uint(123, 277));
	EXPECT(1   == calc_gcd_uint(277, 123));
	EXPECT(123 == calc_gcd_uint(123, 0));
	EXPECT(123 == calc_gcd_uint(0, 123));
}


TEST(test_calc_gcd_u32)
{
	EXPECT(6   == calc_gcd_u32(12, 18));
	EXPECT(6   == calc_gcd_u32(18, 12));
	EXPECT(139 == calc_gcd_u32(1529, 14039));
	EXPECT(139 == calc_gcd_u32(14039, 1529));
	EXPECT(1   == calc_gcd_u32(123, 277));
	EXPECT(1   == calc_gcd_u32(277, 123));
	EXPECT(123 == calc_gcd_u32(123, 0));
	EXPECT(123 == calc_gcd_u32(0, 123));
}


TEST(test_calc_gcd_u64)
{
	EXPECT(6   == calc_gcd_u64(12, 18));
	EXPECT(6   == calc_gcd_u64(18, 12));
	EXPECT(139 == calc_gcd_u64(1529, 14039));
	EXPECT(139 == calc_gcd_u64(14039, 1529));
	EXPECT(1   == calc_gcd_u64(123, 277));
	EXPECT(1   == calc_gcd_u64(277, 123));
	EXPECT(123 == calc_gcd_u64(123, 0));
	EXPECT(123 == calc_gcd_u64(0, 123));
}





// ----------------------------------------------------------------------------------------------- //
// LCM                                                                                             //
// ----------------------------------------------------------------------------------------------- //
TEST(test_calc_lcm)
{
	EXPECT(24   == calc_lcm(8, 12));
	EXPECT(24   == calc_lcm(12, 8));
	EXPECT(5000 == calc_lcm(1000, 625));
	EXPECT(5000 == calc_lcm(625, 1000));
	EXPECT(6    == calc_lcm(2, 3));
	EXPECT(6    == calc_lcm(3, 2));
	EXPECT(220  == calc_lcm(22, 20));
	EXPECT(220  == calc_lcm(20, 22));
	EXPECT(20   == calc_lcm(20, 20));
	EXPECT(0    == calc_lcm(0, 20));
	EXPECT(0    == calc_lcm(20, 0));
	EXPECT(504  == calc_lcm(63, 56));
	EXPECT(504  == calc_lcm(56, 63));
	EXPECT(693  == calc_lcm(63, 77));
	EXPECT(693  == calc_lcm(77, 63));
}


TEST(test_calc_lcm_i32)
{
	EXPECT(24   == calc_lcm_i32(8, 12));
	EXPECT(24   == calc_lcm_i32(12, 8));
	EXPECT(5000 == calc_lcm_i32(1000, 625));
	EXPECT(5000 == calc_lcm_i32(625, 1000));
	EXPECT(6    == calc_lcm_i32(2, 3));
	EXPECT(6    == calc_lcm_i32(3, 2));
	EXPECT(220  == calc_lcm_i32(22, 20));
	EXPECT(220  == calc_lcm_i32(20, 22));
	EXPECT(20   == calc_lcm_i32(20, 20));
	EXPECT(0    == calc_lcm_i32(0, 20));
	EXPECT(0    == calc_lcm_i32(20, 0));
	EXPECT(504  == calc_lcm_i32(63, 56));
	EXPECT(504  == calc_lcm_i32(56, 63));
	EXPECT(693  == calc_lcm_i32(63, 77));
	EXPECT(693  == calc_lcm_i32(77, 63));
}


TEST(test_calc_lcm_i64)
{
	EXPECT(24   == calc_lcm_i64(8, 12));
	EXPECT(24   == calc_lcm_i64(12, 8));
	EXPECT(5000 == calc_lcm_i64(1000, 625));
	EXPECT(5000 == calc_lcm_i64(625, 1000));
	EXPECT(6    == calc_lcm_i64(2, 3));
	EXPECT(6    == calc_lcm_i64(3, 2));
	EXPECT(220  == calc_lcm_i64(22, 20));
	EXPECT(220  == calc_lcm_i64(20, 22));
	EXPECT(20   == calc_lcm_i64(20, 20));
	EXPECT(0    == calc_lcm_i64(0, 20));
	EXPECT(0    == calc_lcm_i64(20, 0));
	EXPECT(504  == calc_lcm_i64(63, 56));
	EXPECT(504  == calc_lcm_i64(56, 63));
	EXPECT(693  == calc_lcm_i64(63, 77));
	EXPECT(693  == calc_lcm_i64(77, 63));
}


TEST(test_calc_lcm_uint)
{
	EXPECT(24   == calc_lcm_uint(8, 12));
	EXPECT(24   == calc_lcm_uint(12, 8));
	EXPECT(5000 == calc_lcm_uint(1000, 625));
	EXPECT(5000 == calc_lcm_uint(625, 1000));
	EXPECT(6    == calc_lcm_uint(2, 3));
	EXPECT(6    == calc_lcm_uint(3, 2));
	EXPECT(220  == calc_lcm_uint(22, 20));
	EXPECT(220  == calc_lcm_uint(20, 22));
	EXPECT(20   == calc_lcm_uint(20, 20));
	EXPECT(0    == calc_lcm_uint(0, 20));
	EXPECT(0    == calc_lcm_uint(20, 0));
	EXPECT(504  == calc_lcm_uint(63, 56));
	EXPECT(504  == calc_lcm_uint(56, 63));
	EXPECT(693  == calc_lcm_uint(63, 77));
	EXPECT(693  == calc_lcm_uint(77, 63));
}


TEST(test_calc_lcm_u32)
{
	EXPECT(24   == calc_lcm_u32(8, 12));
	EXPECT(24   == calc_lcm_u32(12, 8));
	EXPECT(5000 == calc_lcm_u32(1000, 625));
	EXPECT(5000 == calc_lcm_u32(625, 1000));
	EXPECT(6    == calc_lcm_u32(2, 3));
	EXPECT(6    == calc_lcm_u32(3, 2));
	EXPECT(220  == calc_lcm_u32(22, 20));
	EXPECT(220  == calc_lcm_u32(20, 22));
	EXPECT(20   == calc_lcm_u32(20, 20));
	EXPECT(0    == calc_lcm_u32(0, 20));
	EXPECT(0    == calc_lcm_u32(20, 0));
	EXPECT(504  == calc_lcm_u32(63, 56));
	EXPECT(504  == calc_lcm_u32(56, 63));
	EXPECT(693  == calc_lcm_u32(63, 77));
	EXPECT(693  == calc_lcm_u32(77, 63));
}


TEST(test_calc_lcm_u64)
{
	EXPECT(24   == calc_lcm_u64(8, 12));
	EXPECT(24   == calc_lcm_u64(12, 8));
	EXPECT(5000 == calc_lcm_u64(1000, 625));
	EXPECT(5000 == calc_lcm_u64(625, 1000));
	EXPECT(6    == calc_lcm_u64(2, 3));
	EXPECT(6    == calc_lcm_u64(3, 2));
	EXPECT(220  == calc_lcm_u64(22, 20));
	EXPECT(220  == calc_lcm_u64(20, 22));
	EXPECT(20   == calc_lcm_u64(20, 20));
	EXPECT(0    == calc_lcm_u64(0, 20));
	EXPECT(0    == calc_lcm_u64(20, 0));
	EXPECT(504  == calc_lcm_u64(63, 56));
	EXPECT(504  == calc_lcm_u64(56, 63));
	EXPECT(693  == calc_lcm_u64(63, 77));
	EXPECT(693  == calc_lcm_u64(77, 63));
}





// ----------------------------------------------------------------------------------------------- //
// Diophantine                                                                                     //
// ----------------------------------------------------------------------------------------------- //
static void show_diophantine(int a, int b, int c, int x[2], int y[2])
{
	PRINT("%dx + %dy = %d\n", a, b, c);
	PRINT("x = %d + %dn\n", x[0], x[1]);
	PRINT("y = %d + %dn\n", y[0], y[1]);
}


TEST(test_calc_diophantine_int)
{
	int x[2] = { 0 };
	int y[2] = { 0 };
	int a, b, c;

	/* 21x - 4y = -2
	 *
	 * Initial solution is:
	 * 		x = -2 + 4n
	 * 		y = -10 + 21n
	 *
	 * Minimal solution is:
	 * 		x = 2 + 4n
	 * 		y = 11 + 21n */
	a = 21; b = -4; c = -2;
	calc_ax_by_c_int(a, b, c, x, y);
	EXPECT(
		x[0] == 2  &&
		x[1] == 4  &&
		y[0] == 11 &&
		y[1] == 21);
	show_diophantine(a, b, c, x, y);

	/* 141x - 34y = 30
	 *
	 * Initial solution is:
	 * 		x = 210 + 34n
	 * 		y = 870 + 141n
	 *
	 * Minimal solution is:
	 * 		x = 6 + 34n
	 * 		y = 24 + 141n */
	a = 141; b = -34; c = 30;
	calc_ax_by_c_int(a, b, c, x, y);
	EXPECT(
		x[0] == 6  &&
		x[1] == 34  &&
		y[0] == 24 &&
		y[1] == 141);
	calc_ax_by_c_int(a, b, c, x, y);

	/* 20243x - 9153y = 69084
	 *
	 * Initial solution is:
	 * 		x =  46355364 - 9153n
	 * 		y = -102520656 + 20243n
	 *
	 * Minimal solution is:
	 * 		x = 4572 + 9153n
	 * 		y = -10104 - 20243n */
	a = 20243; b = 9153; c = 69084;
	calc_ax_by_c_int(a, b, c, x, y);
	EXPECT(
		x[0] ==  4572  &&
		x[1] ==  9153  &&
		y[0] == -10104 &&
		y[1] == -20243);
	calc_ax_by_c_int(a, b, c, x, y);
}





// ----------------------------------------------------------------------------------------------- //
// Clamp                                                                                           //
// ----------------------------------------------------------------------------------------------- //
TEST(test_calc_clamp_f)
{
	EXPECT(30.0f == calc_clamp(30.0f, 20.0f, 40.0f));
	EXPECT(20.0f == calc_clamp(10.0f, 20.0f, 40.0f));
	EXPECT(40.0f == calc_clamp(50.0f, 20.0f, 40.0f));
}


TEST(test_calc_clamp_d)
{
	EXPECT(30.0 == calc_clamp(30.0, 20.0, 40.0));
	EXPECT(20.0 == calc_clamp(10.0, 20.0, 40.0));
	EXPECT(40.0 == calc_clamp(50.0, 20.0, 40.0));
}





// ----------------------------------------------------------------------------------------------- //
// Deg / Rad                                                                                       //
// ----------------------------------------------------------------------------------------------- //
TEST(test_calc_rad_to_deg_f)
{
	EXPECT(0.0f   == calc_rad_to_deg_f(0));
	EXPECT(90.0f  == calc_rad_to_deg_f(M_PI / 2.0));
	EXPECT(180.0f == calc_rad_to_deg_f(M_PI));
	EXPECT(270.0f == calc_rad_to_deg_f(3.0 * M_PI / 2.0));
	EXPECT(360.0f == calc_rad_to_deg_f(2.0 * M_PI));
}


TEST(test_calc_rad_to_deg_d)
{
	EXPECT(0.0f   == calc_rad_to_deg_d(0));
	EXPECT(90.0f  == calc_rad_to_deg_d(M_PI / 2.0));
	EXPECT(180.0f == calc_rad_to_deg_d(M_PI));
	EXPECT(270.0f == calc_rad_to_deg_d(3.0 * M_PI / 2.0));
	EXPECT(360.0f == calc_rad_to_deg_d(2.0 * M_PI));
}


TEST(test_calc_deg_to_rad_f)
{
	EXPECT(0 == calc_deg_to_rad_f(0));
	EXPECT((float)(M_PI / 2.0)       == calc_deg_to_rad_f(90));
	EXPECT((float)(M_PI)             == calc_deg_to_rad_f(180));
	EXPECT((float)(3.0 * M_PI / 2.0) == calc_deg_to_rad_f(270));
	EXPECT((float)(2.0 * M_PI)       == calc_deg_to_rad_f(360));
}


TEST(test_calc_deg_to_rad_d)
{
	EXPECT(0 == calc_deg_to_rad_d(0));
	EXPECT((double)(M_PI / 2.0)       == calc_deg_to_rad_d(90));
	EXPECT((double)(M_PI)             == calc_deg_to_rad_d(180));
	EXPECT((double)(3.0 * M_PI / 2.0) == calc_deg_to_rad_d(270));
	EXPECT((double)(2.0 * M_PI)       == calc_deg_to_rad_d(360));
}





// ----------------------------------------------------------------------------------------------- //
// Map                                                                                             //
// ----------------------------------------------------------------------------------------------- //
TEST(test_calc_mapf_360)
{
	EXPECT(0.0f   == calc_mapf_360(0),       "Got %g instead.", calc_mapf_360( 0));
	EXPECT(360.0f == calc_mapf_360(360),     "Got %g instead.", calc_mapf_360( 360));
	EXPECT(0.0f   == calc_mapf_360(-360),    "Got %g instead.", calc_mapf_360(-360));
	EXPECT(1.0f   == calc_mapf_360(361),     "Got %g instead.", calc_mapf_360( 361));
	EXPECT(359.0f == calc_mapf_360(-361),    "Got %g instead.", calc_mapf_360(-361));
	EXPECT(360.0f == calc_mapf_360(720.0f),  "Got %g instead.", calc_mapf_360( 720.0f));
	EXPECT(0.0f   == calc_mapf_360(-720.0f), "Got %g instead.", calc_mapf_360(-720.0f));
}


TEST(test_calc_mapd_360)
{
	EXPECT(0.0   == calc_mapd_360(0));
	EXPECT(360.0 == calc_mapd_360(360));
	EXPECT(0.0   == calc_mapd_360(-360));
	EXPECT(1.0   == calc_mapd_360(361));
	EXPECT(359.0 == calc_mapd_360(-361));
	EXPECT(360.0 == calc_mapd_360(720.0));
	EXPECT(0.0   == calc_mapd_360(-720.0));
}


TEST(test_calc_mapf_2pi)
{
	EXPECT(0.0f                == calc_mapf_2pi(0));
	EXPECT((float)M_2PI        == calc_mapf_2pi((float)M_2PI));
	EXPECT(0.0f                == calc_mapf_2pi((float)-M_2PI));
	EXPECT(1.0f                == calc_mapf_2pi((float)M_2PI + 1.0f));
	EXPECT((float)M_2PI - 1.0f == calc_mapf_2pi((float)-M_2PI - 1.0f));
}


TEST(test_calc_mapd_2pi)
{
	EXPECT(0.0 == calc_mapd_2pi(0));
	EXPECT((double)(M_2PI) == calc_mapd_2pi(M_2PI));
	EXPECT(0.0 == calc_mapd_2pi(-M_2PI));
	EXPECT(1.0 == calc_mapd_2pi(M_2PI + 1));
	EXPECT((double)(M_2PI - 1) == calc_mapd_2pi(-M_2PI - 1));
}


TEST(test_calc_mapf_180)
{
	EXPECT( 0.0f   == calc_mapf_180(0));
	EXPECT( 180.0f == calc_mapf_180(180));
	EXPECT(-180.0f == calc_mapf_180(-180));
	EXPECT( 179.0f == calc_mapf_180(-181));
	EXPECT(-179.0f == calc_mapf_180(181));
}


TEST(test_calc_mapd_180)
{
	EXPECT( 0.0   == calc_mapd_180(0));
	EXPECT( 180.0 == calc_mapd_180(180));
	EXPECT(-180.0 == calc_mapd_180(-180));
	EXPECT( 179.0 == calc_mapd_180(-181));
	EXPECT(-179.0 == calc_mapd_180(181));
}


TEST(test_calc_mapf_pi)
{
	EXPECT(0 == calc_mapf_pi(0));
	EXPECT((float)( M_PI  ) == calc_mapf_pi((float)(M_PI)));
	EXPECT((float)(-M_PI  ) == calc_mapf_pi((float)(-M_PI)));
	EXPECT((float)(-M_PI+1) == calc_mapf_pi((float)(-M_PI+1)));
	EXPECT((float)( M_PI-1) == calc_mapf_pi((float)(M_PI-1)));
}


TEST(test_calc_mapd_pi)
{
	EXPECT(0 == calc_mapd_pi(0));
	EXPECT((double)( M_PI  ) == calc_mapd_pi(M_PI));
	EXPECT((double)(-M_PI  ) == calc_mapd_pi(-M_PI));
	EXPECT((double)(-M_PI+1) == calc_mapd_pi(M_PI+1));
	EXPECT((double)( M_PI-1) == calc_mapd_pi(-M_PI-1));
}




void test_calc(void)
{
	/* Sign */
	tharness_run(test_sign);
	tharness_run(test_sign_i32);
	tharness_run(test_sign_i64);
	tharness_run(test_sign_uint);
	tharness_run(test_sign_u32);
	tharness_run(test_sign_u64);
	tharness_run(test_sign_f);
	tharness_run(test_sign_d);

	/* Abs */
	tharness_run(test_calc_abs_int);
	tharness_run(test_calc_abs_i32);
	tharness_run(test_calc_abs_i64);
	tharness_run(test_calc_abs_f);
	tharness_run(test_calc_abs_d);

	/* Max */
	tharness_run(test_calc_max);
	tharness_run(test_calc_max_i32);
	tharness_run(test_calc_max_i64);
	tharness_run(test_calc_max_uint);
	tharness_run(test_calc_max_u32);
	tharness_run(test_calc_max_u64);
	tharness_run(test_calc_max_f);
	tharness_run(test_calc_max_d);

	/* Min */
	tharness_run(test_calc_min);
	tharness_run(test_calc_min_i32);
	tharness_run(test_calc_min_i64);
	tharness_run(test_calc_min_uint);
	tharness_run(test_calc_min_u32);
	tharness_run(test_calc_min_u64);
	tharness_run(test_calc_min_f);
	tharness_run(test_calc_min_d);

	/* Avg */
	tharness_run(test_calc_avg);
	tharness_run(test_calc_avg_i32);
	tharness_run(test_calc_avg_i64);
	tharness_run(test_calc_avg_uint);
	tharness_run(test_calc_avg_u32);
	tharness_run(test_calc_avg_u64);
	tharness_run(test_calc_avg_f);
	tharness_run(test_calc_avg_d);

	/* Bit-twiddling */
	tharness_run(test_calc_log2);
	tharness_run(test_calc_clp2);
	tharness_run(test_calc_flp2);
	tharness_run(test_calc_popcount_u16);
	tharness_run(test_calc_popcount_u32);
	tharness_run(test_calc_popcount_u64);

	/* Round */
	tharness_run(test_calc_round);
	tharness_run(test_round_i32);
	tharness_run(test_round_i64);
	tharness_run(test_round_uint);
	tharness_run(test_round_u32);
	tharness_run(test_round_u64);
	tharness_run(test_round_f);
	tharness_run(test_round_d);

	/* Mod */
	tharness_run(test_calc_mod);
	tharness_run(test_calc_mod_i32);
	tharness_run(test_calc_mod_i64);
	tharness_run(test_calc_mod_f);
	tharness_run(test_calc_mod_d);

	/* Addmod */
	tharness_run(test_calc_addmod_u32);

	/* Wrapdiff */
	tharness_run(test_calc_wrapdiff);
	tharness_run(test_calc_wrapdiff_i32);
	tharness_run(test_calc_wrapdiff_i64);
	tharness_run(test_calc_wrapdiff_uint);
	tharness_run(test_calc_wrapdiff_u32);
	tharness_run(test_calc_wrapdiff_u64);
	tharness_run(test_calc_wrapdiff_f);
	tharness_run(test_calc_wrapdiff_d);

	/* GCD */
	tharness_run(test_calc_gcd);
	tharness_run(test_calc_gcd_i32);
	tharness_run(test_calc_gcd_i64);
	tharness_run(test_calc_gcd_uint);
	tharness_run(test_calc_gcd_u32);
	tharness_run(test_calc_gcd_u64);

	/* LCM */
	tharness_run(test_calc_lcm);
	tharness_run(test_calc_lcm_i32);
	tharness_run(test_calc_lcm_i64);
	tharness_run(test_calc_lcm_uint);
	tharness_run(test_calc_lcm_u32);
	tharness_run(test_calc_lcm_u64);

	/* Diophantine */
	tharness_run(test_calc_diophantine_int);

	/* Clamp */
	tharness_run(test_calc_clamp_f);
	tharness_run(test_calc_clamp_d);

	/* Rad to Deg / Deg to Rad */
	tharness_run(test_calc_rad_to_deg_f);
	tharness_run(test_calc_rad_to_deg_d);
	tharness_run(test_calc_deg_to_rad_f);
	tharness_run(test_calc_deg_to_rad_d);

	/* Map */
	tharness_run(test_calc_mapf_360);
	tharness_run(test_calc_mapd_360);
	tharness_run(test_calc_mapf_2pi);
	tharness_run(test_calc_mapd_2pi);
	tharness_run(test_calc_mapf_180);
	tharness_run(test_calc_mapd_180);
	tharness_run(test_calc_mapf_pi);
	tharness_run(test_calc_mapd_pi);
}


/******************************************* END OF FILE *******************************************/
