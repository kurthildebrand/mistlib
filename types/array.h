/************************************************************************************************//**
 * @file		array.h
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
 ***************************************************************************************************/
#ifndef ARRAY_H
#define ARRAY_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "key.h"
#include "range.h"
#include "utils.h"


/* Public Macros --------------------------------------------------------------------------------- */
/* MAKE_ARRAY ***********************************************************************************//**
 * @brief		Initializes an Array at compile time. This macro may be called with a varying number
 *				of arguments.
 * @param[in]	k: the key of the Array.
 * @param[in]	d: pointer to the Array's data.
 * @param[in]	s: total number of entries in the Array.
 * @param[in]	e: the number of bytes per entry.
 * @desc		MAKE_ARRAY(d): Initializes an Array to point to a raw array. The count and element
 *					size are automatically set using sizeof. The macro performs the following
 *					operation:
 *
 *						const int data[] = {
 *							10, 20, 30, 40, 50
 *						};
 *
 *						const Array a1 = {
 *							.entries  = data,
 *							.count    = sizeof(data) / sizeof(data[0]),
 *							.elemsize = sizeof(data[0]),
 *						};
 *
 *						const Array a2 = MAKE_ARRAY(data);
 *
 *				MAKE_ARRAY(k,d): Performs the same initialization as MAKE_ARRAY(d) but also sets the
 *					array's key.
 *
 *				MAKE_ARRAY(d,c,e): Sets the Array's pointer, count, and element size.
 *
 *				MAKE_ARRAY(k,d,c,e): Sets the Array's key, pointer, count, and element size. */
#define MAKE_ARRAY(...)			APPEND_NARGS(MAKE_ARRAY, __VA_ARGS__)
#define MAKE_ARRAY1(d)			MAKE_ARRAY3(     (d), sizeof((d))/sizeof((d)[0]), sizeof((d)[0]))
#define MAKE_ARRAY2(k,d)		MAKE_ARRAY4((k), (d), sizeof((d))/sizeof((d)[0]), sizeof((d)[0]))
#define MAKE_ARRAY3(d,c,e)		(Array){           .range=MAKE_RANGE3(d,c,e) }
#define MAKE_ARRAY4(k,d,c,e)	(Array){ .key=(k), .range=MAKE_RANGE3(d,c,e) }


/* Public Types ---------------------------------------------------------------------------------- */
typedef struct {
	Key   key;
	Range range;
} Array;


/* Public Functions ------------------------------------------------------------------------------ */
inline void     array_init      (Array*, void*, unsigned, unsigned);
inline void     array_const_init(Array*, const void*, unsigned, unsigned);
inline void     array_init_range(Array*, Range);
inline Array    array_from_range(Range);
inline Array    make_array      (void*, unsigned, unsigned);
inline Array    make_const_array(const void*, unsigned, unsigned);

inline Range*   array_range     (const Array*);
inline Key      array_key       (const Array* a)  { return a->key;                    }
inline unsigned array_count     (const Array* a)  { return range_count(&a->range);    }
inline unsigned array_elemsize  (const Array* a)  { return range_elemsize(&a->range); }
inline void*    array_entry     (const Array*, unsigned);
inline bool     array_put       (Array*, const void*, unsigned);
inline bool     array_put_many  (Array*, const void*, unsigned, unsigned);
inline bool     array_get       (const Array*, void*, unsigned);
inline bool     array_get_many  (const Array*, void*, unsigned, unsigned);
inline bool     array_swap      (Array*, unsigned, unsigned);


/* array_init ***********************************************************************************//**
 * @brief		Initializes an array to point to the specified data with 'count' number of elements
 *				of 'elemsize' bytes each.
 * @param[in]	a: the array to initialize.
 * @param[in]	data: pointer to the array's data.
 * @param[in]	count: the number of entries in the array.
 * @param[in]	elemsize: the size in bytes of an entry. */
inline void array_init(Array* a, void* data, unsigned count, unsigned elemsize)
{
	range_init(&a->range, data, count, elemsize);
}


/* array_const_init *****************************************************************************//**
 * @brief		Initializes an array to point to the specified const data with 'count' number of
 * 				elements of 'elemsize' bytes each.
 * @param[in]	a: the array to initialize.
 * @param[in]	data: pointer to the array's const data.
 * @param[in]	count: the number of entries in the array.
 * @param[in]	elemsize: the size in bytes of an entry. */
inline void array_const_init(Array* a, const void* data, unsigned count, unsigned elemsize)
{
	range_const_init(&a->range, data, count, elemsize);
}


/* array_init_range *****************************************************************************//**
 * @brief		Initializes an array from a range of entries.
 * @param[in]	a: the array to initialize.
 * @param[in]	r: the range representing the array's entries. */
inline void array_init_range(Array* a, Range r)
{
	a->range = r;
}


/* array_from_range *****************************************************************************//**
 * @brief		Creates an array from a range of entries. */
inline Array array_from_range(Range r)
{
	return (Array){ .range = r };
}


/* make_array ***********************************************************************************//**
 * @brief		Creates an array pointing to the specified data with 'count' number of elements of
 * 				'elemsize' bytes each.
 * @param[in]	data: pointer to the array's data.
 * @param[in]	count: the number of entries in the array.
 * @param[in]	elemsize: the size in bytes of an entry. */
inline Array make_array(void* data, unsigned count, unsigned elemsize)
{
	return (Array){ .range = make_range(data, count, elemsize) };
}


/* make_const_array *****************************************************************************//**
 * @brief		Creates an array pointing to the specified const data with 'count' number of elements
 * 				of 'elemsize' bytes each.
 * @param[in]	data: pointer to the array's data.
 * @param[in]	count: the number of entries in the array.
 * @param[in]	elemsize: the size in bytes of an entry. */
inline Array make_const_array(const void* data, unsigned count, unsigned elemsize)
{
	return (Array){ .range = make_const_range(data, count, elemsize) };
}


/* array_range **********************************************************************************//**
 * @brief		Returns a range of valid entries for the entire array. */
inline Range* array_range(const Array* a)
{
	return (Range*)&a->range;
}


/* array_entry **********************************************************************************//**
 * @brief		Returns a pointer to the specified entry.
 * @warning		This function removes the const qualifier from the returned data pointer. Do not try
 * 				to modify const data. However, non-const data can be safely modified even if the
 * 				const qualifier has been cast away.
 * @param[in]	a: the array to retrieve the entry from.
 * @param[in]	idx: the index of the entry to retrieve.
 * @return		Pointer to the specified entry. Null if the index is out of range. */
inline void* array_entry(const Array* a, unsigned idx)
{
	Range* r = array_range(a);

	return range_entry(r, idx);
}


/* array_put ************************************************************************************//**
 * @brief		Copies a single element into the array at the specified index.
 * @param[in]	a: the array to copy into.
 * @param[in]	in: the data to copy.
 * @param[in]	idx: the index of the entry to copy into.
 * @retval		true if the element was copied successfully.
 * @retval		false if the index is out of bounds. */
inline bool array_put(Array* a, const void* in, unsigned idx)
{
	Range* r = array_range(a);

	return range_put(r, in, idx);
}


/* array_put_many *******************************************************************************//**
 * @brief		Copies 'count' elements into the array starting at the specified index.
 * @param[in]	a: the array to copy into.
 * @param[in]	in: the data to copy.
 * @param[in]	start: the index of the first entry in the array to start copying into.
 * @param[in]	count: the number of elements to copy.
 * @retval		true if the data was copied successfully.
 * @retval		false if the indices were out of bounds. */
inline bool array_put_many(Array* a, const void* in, unsigned start, unsigned count)
{
	Range* r = array_range(a);

	return range_put_many(r, in, start, count);
}


/* array_get ************************************************************************************//**
 * @brief		Copies a single element out of the array from the specified index.
 * @param[in]	a: the array to copy out of.
 * @param[out]	out: buffer to hold a copy of the requested element.
 * @param[in]	idx: the index to entry copy.
 * @retval		true if the data was copied successfully.
 * @retval		false if the index is out of bounds. */
inline bool array_get(const Array* a, void* out, unsigned idx)
{
	Range* r = array_range(a);

	return range_get(r, out, idx);
}


/* array_get_many *******************************************************************************//**
 * @brief		Copies 'count' elements out of the array starting at the specified index.
 * @param[in]	a: the array to copy out of.
 * @param[out]	out: buffer to hold a copy of the requested elements.
 * @param[in]	start: the index of the first entry in the array to start copying out of.
 * @param[in]	count: the number of elements to copy.
 * @retval		true if the data was copied successfully.
 * @retval		false if the indices were out of bounds. */
inline bool array_get_many(const Array* a, void* out, unsigned start, unsigned count)
{
	Range* r = array_range(a);

	return range_get_many(r, out, start, count);
}


/* array_swap ***********************************************************************************//**
 * @brief		Swaps two elements in an array.
 * @param[in]	a: the array to swap elements in.
 * @param[in]	first: the index of the first element.
 * @param[in]	second: the index of the second element.
 * @retval		true if the elements were swapped successfully.
 * @retval		false if one of the indices is out of bounds. */
inline bool array_swap(Array* a, unsigned first, unsigned second)
{
	Range* r = array_range(a);

	return range_swap(r, first, second);
}


#ifdef __cplusplus
}
#endif

#endif // ARRAY_H
/******************************************* END OF FILE *******************************************/
