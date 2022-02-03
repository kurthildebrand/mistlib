/************************************************************************************************//**
 * @file		compare.h
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
 *					return value > zero  if a > b
 *					return value == zero if a == b
 *					return value < zero  if a < b
 *
 *				This file also provides common comparison functions for the various primitive types:
 *
 *					compare_keys	compares two keys
 *					compare			compares two signed integers
 *					compare_i8		compares two int8_t integers
 *					compare_i16		compares two int16_t integers
 *					compare_i32		compares two int32_t integers
 *					compare_i64		compares two int64_t integers
 *					ucompare		compares two unsigned integers
 *					compare_u8		compares two uint8_t integers
 *					compare_u16		compares two uint16_t integers
 *					compare_u32		compares two uint32_t integers
 *					compare_u64		comapres two uint64_t integers
 *
 ***************************************************************************************************/
#ifndef COMPARE_H
#define COMPARE_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher for inline support.
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <limits.h>

#include "calc.h"
#include "key.h"


/* Public Types ---------------------------------------------------------------------------------- */
/** ICompare is a callback which takes two pointers and returns
 *  <0 if a < b (sorting order)
 *   0 if a == b
 *  >0 if a > b */
typedef int (*ICompare)(const void* a, const void* b);


/* Public Functions ------------------------------------------------------------------------------ */
inline int compare_keys     (const void*, const void*);
inline int compare_ptrs     (const void*, const void*);
inline int compare_dptrs    (const void*, const void*);

inline int compare_int      (const void*, const void*);
inline int compare_i8       (const void*, const void*);
inline int compare_i16      (const void*, const void*);
inline int compare_i32      (const void*, const void*);
inline int compare_i64      (const void*, const void*);

inline int compare_uint     (const void*, const void*);
inline int compare_u8       (const void*, const void*);
inline int compare_u16      (const void*, const void*);
inline int compare_u32      (const void*, const void*);
inline int compare_u64      (const void*, const void*);

inline int compare_f        (const void*, const void*);
inline int compare_d        (const void*, const void*);

inline int compare_uint_wrap(const void*, const void*);
inline int compare_u8_wrap  (const void*, const void*);
inline int compare_u16_wrap (const void*, const void*);
inline int compare_u32_wrap (const void*, const void*);
inline int compare_u64_wrap (const void*, const void*);

inline int compare_normalize(int);



/* compare_keys *********************************************************************************//**
 * @brief		Compares two Keys.
 * @param[in]	a: pointer to the first key.
 * @param[in]	b: pointer to the second key.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_keys(const void* a, const void* b)
{
	Key first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (first > second) - (first < second);
}


/* compare_ptrs *********************************************************************************//**
 * @brief		Compares two pointers.
 * @param[in]	a: the first pointer.
 * @param[in]	b: the second pointer.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_ptrs(const void* a, const void* b)
{
	return (a > b) - (b < a);
}


/* compare_dptrs ********************************************************************************//**
 * @brief		Compares two pointers to pointers.
 * @desc		Example:
 *
 * 					int data[10];
 * 					int* p1 = &data[2];
 * 					int* p2 = &data[4];
 *
 * 					// Note:      v    v address of pointer
 * 					compare_dptrs(&p1, &p2): <0
 * 					compare_dptrs(&p2, &p1): >0
 *
 * @param[in]	a: pointer to the first pointer.
 * @param[in]	b: pointer to the second pointer.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_dptrs(const void* a, const void* b)
{
	void* first;
	void* second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (first > second) - (first < second);
}


/* compare_int **********************************************************************************//**
 * @brief		Compares two signed integers.
 * @param[in]	a: pointer to the first int.
 * @param[in]	b: pointer to the second int.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_int(const void* a, const void* b)
{
	int first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (first > second) - (first < second);
}


/* compare_i8 ***********************************************************************************//**
 * @brief		Compares two int8_t.
 * @param[in]	a: pointer to the first int8_t.
 * @param[in]	b: pointer to the second int8_t.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_i8(const void* a, const void* b)
{
	int8_t first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (first > second) - (first < second);
}


/* compare_i16 **********************************************************************************//**
 * @brief		Compares two int16_t.
 * @param[in]	a: pointer to the first int16_t.
 * @param[in]	b: pointer to the second int16_t.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_i16(const void* a, const void* b)
{
	int16_t first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (first > second) - (first < second);
}


/* compare_i32 **********************************************************************************//**
 * @brief		Compares two int32_t.
 * @param[in]	a: pointer to the first int32_t.
 * @param[in]	b: pointer to the second int32_t.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_i32(const void* a, const void* b)
{
	int32_t first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (first > second) - (first < second);
}


/* compare_i64 **********************************************************************************//**
 * @brief		Compares two int64_t.
 * @param[in]	a: pointer to the first int64_t.
 * @param[in]	b: pointer to the second int64_t.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_i64(const void* a, const void* b)
{
	int64_t first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (first > second) - (first < second);
}


/* compare_uint *********************************************************************************//**
 * @brief		Compares two unsigned integers.
 * @param[in]	a: pointer to the first unsigned int.
 * @param[in]	b: pointer to the second unsigned int.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_uint(const void* a, const void* b)
{
	unsigned first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (first > second) - (first < second);
}


/* compare_u8 ***********************************************************************************//**
 * @brief		Compares two uint8_t.
 * @param[in]	a: pointer to the first uint8_t.
 * @param[in]	b: pointer to the second unsigned int.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_u8(const void* a, const void* b)
{
	uint8_t first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (first > second) - (first < second);
}


/* compare_u16 **********************************************************************************//**
 * @brief		Compares two uint16_t.
 * @param[in]	a: pointer to the first uint16_t.
 * @param[in]	b: pointer to the second uint16_t.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_u16(const void* a, const void* b)
{
	uint16_t first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (first > second) - (first < second);
}


/* compare_u32 **********************************************************************************//**
 * @brief		Compares two uint32_t.
 * @param[in]	a: pointer to the first uint32_t.
 * @param[in]	b: pointer to the second uint32_t.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_u32(const void* a, const void* b)
{
	uint32_t first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (first > second) - (first < second);
}


/* compare_u64 **********************************************************************************//**
 * @brief		Compares two uint64_t.
 * @param[in]	a: pointer to the first uint64_t.
 * @param[in]	b: pointer to the second uint64_t.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_u64(const void* a, const void* b)
{
	uint64_t first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (first > second) - (first < second);
}


/* compare_f ************************************************************************************//**
 * @brief		Compares two floats
 * @param[in]	a: pointer to the first float.
 * @param[in]	b: pointer to the second float.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_f(const void* a, const void*b)
{
	float first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return calc_compare_f(first, second);
}


/* compare_d ************************************************************************************//**
 * @brief		Compares two doubles.
 * @param[in]	a: pointer to the first double.
 * @param[in]	b: pointer to the second double.
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_d(const void* a, const void*b)
{
	double first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return calc_compare_d(first, second);
}


/* compare_uint_wrap ****************************************************************************//**
 * @brief		Compares two unsigned integers with overflow wrap around. The magnitude of the return
 * 				value is the difference between b and a. Example:
 *
 * 					unsigned a = 4,294,967,286
 * 					unsigned b = 10
 *
 * 					compare_uint_wrap(&a, &b) == -10
 * 					compare_uint_wrap(&b, &a) == +10
 *
 * @warning		The maximum difference between a and b must be less than UINT_MAX / 2.
 * @param[in]	a: pointer to the first
 * @param[in]	b: pointer to the second
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_uint_wrap(const void* a, const void* b)
{
	unsigned first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (int)(second - first);
}


/* compare_u8_wrap ******************************************************************************//**
 * @brief		Compares two uint8_t integers with overflow wrap around. The magnitude of the return
 * 				value is the difference between b and a.
 * @warning		The maximum difference between a and b must be less than UINT8_MAX / 2.
 * @param[in]	a: pointer to the first
 * @param[in]	b: pointer to the second
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_u8_wrap(const void* a, const void* b)
{
	uint8_t first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (int8_t)(second - first);
}


/* compare_u16_wrap *****************************************************************************//**
 * @brief		Compares two uint16_t integers with overflow wrap around. The magnitude of the return
 * 				value is the difference between b and a.
 * @warning		The maximum difference between a and b must be less than UINT16_MAX / 2.
 * @param[in]	a: pointer to the first
 * @param[in]	b: pointer to the second
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_u16_wrap(const void* a, const void* b)
{
	uint16_t first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (int16_t)(second - first);
}


/* compare_u32_wrap *****************************************************************************//**
 * @brief		Compares two uint16_t integers with overflow wrap around. The magnitude of the return
 * 				value is the difference between b and a.
 * @warning		The maximum difference between a and b must be less than UINT32_MAX / 2.
 * @param[in]	a: pointer to the first
 * @param[in]	b: pointer to the second
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_u32_wrap(const void* a, const void* b)
{
	uint32_t first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (int)calc_clamp_i32((int32_t)(second - first), INT_MIN, INT_MAX);
}


/* compare_u64_wrap *****************************************************************************//**
 * @brief		Compares two uint16_t integers with overflow wrap around. The magnitude of the return
 * 				value is the difference between b and a.
 * @warning		The maximum difference between a and b must be less than UINT64_MAX / 2.
 * @param[in]	a: pointer to the first
 * @param[in]	b: pointer to the second
 * @retval		<0 if a < b
 * @retval		 0 if a == b
 * @retval		>0 if a > b */
inline int compare_u64_wrap(const void* a, const void* b)
{
	uint64_t first, second;
	memmove(&first, a, sizeof(first));
	memmove(&second, b, sizeof(second));
	return (int)calc_clamp_i64((int64_t)(second - first), INT_MIN, INT_MAX);
}


/* compare_normalize ****************************************************************************//**
 * @brief		Normalizes a return value from a comparison function.
 * @param[in]	r: the result of a compare function to normalize.
 * @retval		-1 if c < 0.
 * @retval		 0 if c == 0.
 * @retval		+1 if c > 0. */
inline int compare_normalize(int r)
{
	return r > 0 ? +1 :
	       r < 0 ? -1 : 0;
}


#ifdef __cplusplus
}
#endif

#endif // COMPARE_H
/******************************************* END OF FILE *******************************************/
