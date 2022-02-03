/************************************************************************************************//**
 * @file		calc.c
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
 * @brief
 * @desc
 *
 ***************************************************************************************************/
#include "calc.h"


/* Inline Function Instances --------------------------------------------------------------------- */
// ----------------------------------------------------------------------------------------------- //
// Sign                                                                                            //
// ----------------------------------------------------------------------------------------------- //
extern int      calc_sign_int (int      x);
extern int32_t  calc_sign_i32 (int32_t  x);
extern int64_t  calc_sign_i64 (int64_t  x);
extern unsigned calc_sign_uint(unsigned x);
extern uint32_t calc_sign_u32 (uint32_t x);
extern uint64_t calc_sign_u64 (uint64_t x);
extern float    calc_sign_f   (float    x);
extern double   calc_sign_d   (double   x);


// ----------------------------------------------------------------------------------------------- //
// Abs                                                                                             //
// ----------------------------------------------------------------------------------------------- //
extern int      calc_abs_int (int      x);
extern int32_t  calc_abs_i32 (int32_t  x);
extern int64_t  calc_abs_i64 (int64_t  x);
extern unsigned calc_abs_uint(unsigned x);
extern uint32_t calc_abs_u32 (uint32_t x);
extern uint64_t calc_abs_u64 (uint64_t x);
extern float    calc_abs_f   (float    x);
extern double   calc_abs_d   (double   x);


// ----------------------------------------------------------------------------------------------- //
// Max                                                                                             //
// ----------------------------------------------------------------------------------------------- //
extern int      calc_max_int (int      a, int      b);
extern int32_t  calc_max_i32 (int32_t  a, int32_t  b);
extern int64_t  calc_max_i64 (int64_t  a, int64_t  b);
extern unsigned calc_max_uint(unsigned a, unsigned b);
extern uint32_t calc_max_u32 (uint32_t a, uint32_t b);
extern uint64_t calc_max_u64 (uint64_t a, uint64_t b);
extern float    calc_max_f   (float    a, float    b);
extern double   calc_max_d   (double   a, double   b);


// ----------------------------------------------------------------------------------------------- //
// Min                                                                                             //
// ----------------------------------------------------------------------------------------------- //
extern int      calc_min_int (int      a, int      b);
extern int32_t  calc_min_i32 (int32_t  a, int32_t  b);
extern int64_t  calc_min_i64 (int64_t  a, int64_t  b);
extern unsigned calc_min_uint(unsigned a, unsigned b);
extern uint32_t calc_min_u32 (uint32_t a, uint32_t b);
extern uint64_t calc_min_u64 (uint64_t a, uint64_t b);
extern float    calc_min_f   (float    a, float    b);
extern double   calc_min_d   (double   a, double   b);


// ----------------------------------------------------------------------------------------------- //
// Clamp                                                                                           //
// ----------------------------------------------------------------------------------------------- //
extern int      calc_clamp_int (int      x, int      low, int      high);
extern int32_t  calc_clamp_i32 (int32_t  x, int32_t  low, int32_t  high);
extern int64_t  calc_clamp_i64 (int64_t  x, int64_t  low, int64_t  high);
extern unsigned calc_clamp_uint(unsigned x, unsigned low, unsigned high);
extern uint32_t calc_clamp_u32 (uint32_t x, uint32_t low, uint32_t high);
extern uint64_t calc_clamp_u64 (uint64_t x, uint64_t low, uint64_t high);
extern float    calc_clamp_f   (float    x, float    low, float    high);
extern double   calc_clamp_d   (double   x, double   low, double   high);


// ----------------------------------------------------------------------------------------------- //
// Avg                                                                                             //
// ----------------------------------------------------------------------------------------------- //
extern int      calc_avg_int (int      a, int      b);
extern int32_t  calc_avg_i32 (int32_t  a, int32_t  b);
extern int64_t  calc_avg_i64 (int64_t  a, int64_t  b);
extern unsigned calc_avg_uint(unsigned a, unsigned b);
extern uint32_t calc_avg_u32 (uint32_t a, uint32_t b);
extern uint64_t calc_avg_u64 (uint64_t a, uint64_t b);
extern float    calc_avg_f   (float    a, float    b);
extern double   calc_avg_d   (double   a, double   b);


// ----------------------------------------------------------------------------------------------- //
// Bit-twiddling                                                                                   //
// ----------------------------------------------------------------------------------------------- //
extern unsigned calc_log2        (unsigned);
extern unsigned calc_clp2        (unsigned);
extern unsigned calc_flp2        (unsigned);
extern unsigned calc_popcount_u16(uint16_t);
extern unsigned calc_popcount_u32(uint32_t);
extern unsigned calc_popcount_u64(uint64_t);


// ----------------------------------------------------------------------------------------------- //
// Round                                                                                           //
// ----------------------------------------------------------------------------------------------- //
extern int      calc_round_int (int      x, int      n);
extern int32_t  calc_round_i32 (int32_t  x, int32_t  n);
extern int64_t  calc_round_i64 (int64_t  x, int64_t  n);
extern unsigned calc_round_uint(unsigned x, unsigned n);
extern uint32_t calc_round_u32 (uint32_t x, uint32_t n);
extern uint64_t calc_round_u64 (uint64_t x, uint64_t n);
extern float    calc_round_f   (float    x, float    n);
extern double   calc_round_d   (double   x, double   n);


// ----------------------------------------------------------------------------------------------- //
// Mod                                                                                             //
// ----------------------------------------------------------------------------------------------- //
extern int      calc_mod_int (int      num, int      div);
extern int32_t  calc_mod_i32 (int32_t  num, int32_t  div);
extern int64_t  calc_mod_i64 (int64_t  num, int64_t  div);
extern unsigned calc_mod_uint(unsigned num, unsigned div);
extern uint32_t calc_mod_u32 (uint32_t num, uint32_t div);
extern uint64_t calc_mod_u64 (uint64_t num, uint64_t div);
extern float    calc_mod_f   (float    num, float    div);
extern double   calc_mod_d   (double   num, double   div);


// ----------------------------------------------------------------------------------------------- //
// Submod                                                                                          //
// ----------------------------------------------------------------------------------------------- //
extern unsigned calc_submod_uint(unsigned, unsigned, unsigned);
extern uint32_t calc_submod_u32 (uint32_t, uint32_t, uint32_t);
extern uint64_t calc_submod_u64 (uint64_t, uint64_t, uint64_t);


// ----------------------------------------------------------------------------------------------- //
// Addmod                                                                                          //
// ----------------------------------------------------------------------------------------------- //
extern unsigned calc_addmod_uint(unsigned, unsigned, unsigned);
extern uint32_t calc_addmod_u32 (uint32_t, uint32_t, uint32_t);
extern uint64_t calc_addmod_u64 (uint64_t, uint64_t, uint64_t);


// ----------------------------------------------------------------------------------------------- //
// Saturating Add                                                                                  //
// ----------------------------------------------------------------------------------------------- //
extern uint8_t  calc_addsat_u8 (uint8_t,  uint8_t);
extern uint16_t calc_addsat_u16(uint16_t, uint16_t);
extern uint32_t calc_addsat_u32(uint32_t, uint32_t);
extern uint64_t calc_addsat_u64(uint64_t, uint64_t);
extern int8_t   calc_addsat_i8 (int8_t,   int8_t);
extern int16_t  calc_addsat_i16(int16_t,  int16_t);
extern int32_t  calc_addsat_i32(int32_t,  int32_t);
extern int64_t  calc_addsat_i64(int64_t,  int64_t);


// ----------------------------------------------------------------------------------------------- //
// Wrapdiff                                                                                        //
// ----------------------------------------------------------------------------------------------- //
extern int     calc_wrapdiff_int (int,     int,     int);
extern int32_t calc_wrapdiff_i32 (int32_t, int32_t, int32_t);
extern int64_t calc_wrapdiff_i64 (int64_t, int64_t, int64_t);
extern int     calc_wrapdiff_uint(unsigned, unsigned, unsigned);
extern int32_t calc_wrapdiff_u32 (uint32_t, uint32_t, uint32_t);
extern int64_t calc_wrapdiff_u64 (uint64_t, uint64_t, uint64_t);
extern float   calc_wrapdiff_f   (float,   float,   float);
extern double  calc_wrapdiff_d   (float,   float,   float);


// ----------------------------------------------------------------------------------------------- //
// Difference of Products                                                                          //
// ----------------------------------------------------------------------------------------------- //
extern float calc_dop_f(float, float, float, float);


// ----------------------------------------------------------------------------------------------- //
// GCD                                                                                             //
// ----------------------------------------------------------------------------------------------- //
extern int      calc_gcd_int (int,      int);
extern int32_t  calc_gcd_i32 (int32_t,  int32_t);
extern int64_t  calc_gcd_i64 (int64_t,  int64_t);
extern unsigned calc_gcd_uint(unsigned, unsigned);
extern uint32_t calc_gcd_u32 (uint32_t, uint32_t);
extern uint64_t calc_gcd_u64 (uint64_t, uint64_t);


// ----------------------------------------------------------------------------------------------- //
// Extended Euclidean Algorithm                                                                    //
// ----------------------------------------------------------------------------------------------- //
extern int      calc_ext_gcd_int (int, int, int*, int*);
extern int32_t  calc_ext_gcd_i32 (int32_t, int32_t, int32_t*, int32_t*);
extern int64_t  calc_ext_gcd_i64 (int64_t, int64_t, int64_t*, int64_t*);
extern unsigned calc_ext_gcd_uint(unsigned, unsigned, unsigned*, unsigned*);
extern uint32_t calc_ext_gcd_u32 (uint32_t, uint32_t, uint32_t*, uint32_t*);
extern uint64_t calc_ext_gcd_u64 (uint64_t, uint64_t, uint64_t*, uint64_t*);


// ----------------------------------------------------------------------------------------------- //
// LCM                                                                                             //
// ----------------------------------------------------------------------------------------------- //
extern int      calc_lcm_int (int,      int);
extern int32_t  calc_lcm_i32 (int32_t,  int32_t);
extern int64_t  calc_lcm_i64 (int64_t,  int64_t);
extern unsigned calc_lcm_uint(unsigned, unsigned);
extern uint32_t calc_lcm_u32 (uint32_t, uint32_t);
extern uint64_t calc_lcm_u64 (uint64_t, uint64_t);


// ----------------------------------------------------------------------------------------------- //
// ax + by = c                                                                                     //
// ----------------------------------------------------------------------------------------------- //
extern bool calc_ax_by_c_int(int, int, int, int[2], int[2]);


// ----------------------------------------------------------------------------------------------- //
// ax^2 + bx + c = 0                                                                               //
// ----------------------------------------------------------------------------------------------- //
extern void calc_ax2_bx_c_f(float, float, float, float*, float*);


// ----------------------------------------------------------------------------------------------- //
// Deg / Rad                                                                                       //
// ----------------------------------------------------------------------------------------------- //
extern float  calc_rad_to_deg_f(float );
extern double calc_rad_to_deg_d(double);
extern float  calc_deg_to_rad_f(float );
extern double calc_deg_to_rad_d(double);


// ----------------------------------------------------------------------------------------------- //
// Map                                                                                             //
// ----------------------------------------------------------------------------------------------- //
extern float  calc_mapf_360(float);
extern double calc_mapd_360(double);
extern float  calc_mapf_2pi(float);
extern double calc_mapd_2pi(double);
extern float  calc_mapf_180(float);
extern double calc_mapd_180(double);
extern float  calc_mapf_pi (float);
extern double calc_mapd_pi (double);


// ----------------------------------------------------------------------------------------------- //
// Comparing Floats                                                                                //
// ----------------------------------------------------------------------------------------------- //
extern bool calc_within_f (float,  float,  float);
extern bool calc_within_d (double, double, double);
extern int  calc_compare_f(float,  float);
extern int  calc_compare_d(double, double);


// ----------------------------------------------------------------------------------------------- //
// Comparing Floats                                                                                //
// ----------------------------------------------------------------------------------------------- //
extern float calc_randf      (void);
extern float calc_randf_range(float, float);
extern int   calc_rand_range (int, int);


/******************************************* END OF FILE *******************************************/
