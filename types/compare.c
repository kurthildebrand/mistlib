/************************************************************************************************//**
 * @file		compare.c
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
 * @brief		Typedef for a comparison callback function which helps make algorithms generic.
 * @desc		A comparison callback takes pointers to two elements and defines the order of the
 *				arguments by returning:
 *
 *				return value > zero  if a > b
 *				return value == zero if a == b
 *				return value < zero  if a < b
 *
 ***************************************************************************************************/
#include "compare.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern int compare_keys     (const void*, const void*);
extern int compare_ptrs     (const void*, const void*);
extern int compare_dptrs    (const void*, const void*);

extern int compare_int      (const void*, const void*);
extern int compare_i8       (const void*, const void*);
extern int compare_i16      (const void*, const void*);
extern int compare_i32      (const void*, const void*);
extern int compare_i64      (const void*, const void*);

extern int compare_uint     (const void*, const void*);
extern int compare_u8       (const void*, const void*);
extern int compare_u16      (const void*, const void*);
extern int compare_u32      (const void*, const void*);
extern int compare_u64      (const void*, const void*);

extern int compare_f        (const void*, const void*);
extern int compare_d        (const void*, const void*);

extern int compare_uint_wrap(const void*, const void*);
extern int compare_u8_wrap  (const void*, const void*);
extern int compare_u16_wrap (const void*, const void*);
extern int compare_u32_wrap (const void*, const void*);
extern int compare_u64_wrap (const void*, const void*);

extern int compare_normalize(int);


/******************************************* END OF FILE *******************************************/
