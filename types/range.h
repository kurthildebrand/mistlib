/************************************************************************************************//**
 * @file		range.h
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
 * @brief		A range is a set of contiguous elements from [start, end). A range is used as a
 *				intermediate structure to enable generic algorithms.
 *
 ***************************************************************************************************/
#ifndef RANGE_H
#define RANGE_H

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


/* Public Macros --------------------------------------------------------------------------------- */
/* MAKE_RANGE ***********************************************************************************//**
 * @brief		Initializes a Range at compile time. This macro may be called with a varying number
 * 				of arguments.
 * @param[in]	d: pointer to the Range's data.
 * @param[in]	s: total number of entries in the Range.
 * @param[in]	e: the number of bytes per entry.
 * @desc		MAKE_RANGE(d): Initializes a Range to point to a raw C array. The size and element
 * 					size are automatically set using sizeof. The macro performs the following
 *					operation:
 *
 *					const int data[] = {
 *						10, 20, 30, 40, 50
 *					};
 *
 *					const Range r1 = {
 *						.entries  = data,
 *						.elemsize = sizeof(data[0]),
 *						.end      = sizeof(data) / sizeof(data[0]),
 *						.start    = 0,
 *					};
 *
 *					const Range r2 = MAKE_RANGE(data);
 *
 *				MAKE_RANGE(d,s,e): Sets the Range's pointer, size, and element size. */
#define MAKE_RANGE(...) 		APPEND_NARGS(MAKE_RANGE, __VA_ARGS__)
#define MAKE_RANGE1(d)			MAKE_RANGE3((d), sizeof((d))/sizeof((d)[0]), sizeof((d)[0]))
#define MAKE_RANGE3(d,c,e)		(Range){ .entries=(d), .elemsize=(e), .start=(0), .end=(c) }


/* Public Types ---------------------------------------------------------------------------------- */
typedef struct {
	const void* entries;
	unsigned elemsize;
	unsigned end;
	unsigned start;
} Range;


/* Public Functions ------------------------------------------------------------------------------ */
inline void     range_init      (Range*, void*, unsigned, unsigned);
inline void     range_const_init(Range*, const void*, unsigned, unsigned);
inline bool     range_slice     (Range*, const Range*, unsigned, unsigned);
inline Range    make_range      (void*, unsigned, unsigned);
inline Range    make_const_range(const void*, unsigned, unsigned);
inline Range    make_range_slice(const Range*, unsigned, unsigned);

inline unsigned range_start     (const Range* r) { return r->start;           }
inline unsigned range_end       (const Range* r) { return r->end;             }
inline unsigned range_count     (const Range* r) { return r->end - r->start;  }
inline bool     range_empty     (const Range* r) { return r->end == r->start; }
inline unsigned range_elemsize  (const Range* r) { return r->elemsize;        }
inline void*    range_entry     (const Range*, unsigned);
inline void*    range_offset    (const Range*, unsigned);
inline void*    range_at        (const Range*, unsigned);
inline unsigned range_indexof   (const Range*, const void*);
inline unsigned range_offsetof  (const Range*, const void*);
inline bool     range_put       (Range*, const void*, unsigned);
inline bool     range_put_many  (Range*, const void*, unsigned, unsigned);
inline bool     range_get       (const Range*, void*, unsigned);
inline bool     range_get_many  (const Range*, void*, unsigned, unsigned);
       bool     range_swap      (Range*, unsigned, unsigned);


/* range_init ***********************************************************************************//**
 * @brief		Initializes a range object to point to the mutable data.
 * @param[in]	r: the range to initialize.
 * @param[in]	ptr: the range's data.
 * @param[in]	count: the number of elements in the range.
 * @param[in]	elemsize: the number of bytes per element. */
inline void range_init(Range* r, void* ptr, unsigned count, unsigned elemsize)
{
	range_const_init(r, ptr, count, elemsize);
}


/* range_const_init *****************************************************************************//**
 * @brief		Initializes a range object to point to the immutable data.
 * @param[in]	r: the immutable range to initialize.
 * @param[in]	ptr: the range's data.
 * @param[in]	count: the number of elements in the range.
 * @param[in]	elemsize: the number of bytes per element. */
inline void range_const_init(Range* r, const void* ptr, unsigned count, unsigned elemsize)
{
	r->entries  = ptr;
	r->elemsize = elemsize;
	r->end      = elemsize && ptr ? count : 0;
	r->start    = 0;
}


/* range_slice **********************************************************************************//**
 * @brief		Creates a range that is equal to or smaller than the source range. */
inline bool range_slice(Range* dest, const Range* src, unsigned start, unsigned end)
{
	if(!dest)
	{
		return false;
	}
	else if(!src)
	{
		dest->entries  = 0;
		dest->elemsize = 0;
		dest->start    = 0;
		dest->end      = 0;
		return false;
	}
	else if(range_start(src) > start || start > end || end > range_end(src))
	{
		dest->entries  = src->entries;
		dest->elemsize = src->elemsize;
		dest->start    = -1u;
		dest->end      = -1u;
		return false;
	}
	else
	{
		dest->entries  = src->entries;
		dest->elemsize = src->elemsize;
		dest->start    = start;
		dest->end      = end;
		return true;
	}
}


/* make_range_slice *****************************************************************************//**
 * @brief		Returns a range that is equal to or smaller than the source range. */
inline Range make_range_slice(const Range* r, unsigned start, unsigned end)
{
	if(!r)
	{
		return (Range){
			.entries  = 0,
			.elemsize = 0,
			.start    = 0,
			.end      = 0,
		};
	}
	else if(range_start(r) > start || start > end || end > range_end(r))
	{
		return (Range){
			.entries  = r->entries,
			.elemsize = r->elemsize,
			.start    = -1u,
			.end      = -1u,
		};
	}
	else
	{
		return (Range){
			.entries  = r->entries,
			.elemsize = r->elemsize,
			.start    = start,
			.end      = end,
		};
	}
}


/* make_range ***********************************************************************************//**
 * @brief		Creates a range pointing to the specified data of length size and element width
 *				elemsize.
 * @param[in]	ptr: the range's data.
 * @param[in]	count: the number of elements in the range.
 * @param[in]	elemsize: the number of bytes per element. */
inline Range make_range(void* ptr, unsigned count, unsigned elemsize)
{
	return make_const_range(ptr, count, elemsize);
}


/* make_const_range *****************************************************************************//**
 * @brief		Creates a range pointing to the const data of length count and element width
 *				elemsize. Invalid inputs (null pointer, elemsize of 0) result in an empty range.
 * @param[in]	ptr: the range's data.
 * @param[in]	count: the number of elements in the range.
 * @param[in]	elemsize: the number of bytes per element. */
inline Range make_const_range(const void* ptr, unsigned count, unsigned elemsize)
{
	return (Range){
		.entries  = ptr,
		.elemsize = elemsize,
		.end      = elemsize && ptr ? count : 0,
		.start    = 0,
	};
}


/* range_entry **********************************************************************************//**
 * @brief		Returns a pointer to the entry at the specified index. Returns null if the index is
 *				out of bounds.
 * @warning		This function removes the const qualifier from the returned data pointer. Do not try
 * 				to modify const data. However, non-const data can be safely modified even if the
 * 				const qualifier has been cast away.
 * @param[in]	r: the range from which to retrieve a pointer to an entry.
 * @param[in]	idx: the index of the entry to return a pointer to.
 * @return		Pointer to the specified entry. Null if the offset is out of bounds. */
inline void* range_entry(const Range* r, unsigned idx)
{
	if(range_start(r) <= idx && idx < range_end(r))
	{
		return range_at(r, idx);
	}
	else
	{
		return 0;
	}
}


/* range_offset *********************************************************************************//**
 * @brief		Returns a pointer to the entry at the specified offset from the start of the range.
 * 				Returns null if the offset is out of bounds.
 * @warning		This function removes the const qualifier from the returned data pointer. Do not try
 * 				to modify const data. However, non-const data can be safely modified even if the
 * 				const qualifier has been cast away.
 * @param[in]	r: the range from which to retrieve a pointer to an entry.
 * @param[in]	offset: positive offset from the start of the range.
 * @return		Pointer to the specified entry. Null if the offset is out of bounds. */
inline void* range_offset(const Range* r, unsigned offset)
{
	if(offset < range_count(r))
	{
		return range_at(r, offset + range_start(r));
	}
	else
	{
		return 0;
	}
}


/* range_at *************************************************************************************//**
 * @brief		Returns a pointer to the entry at the specified index. Returns null if the range is
 * 				invalid (null pointer or range elemsize is 0).
 * @warning		This function does not check if the index is out of bounds. This function may be used
 *				in cases where the index is guaranteed to always be in bounds to avoid unnecessary
 *				checks.
 * @warning		This function removes the const qualifier from the returned data pointer. Do not try
 * 				to modify const data. However, non-const data can be safely modified even if the
 * 				const qualifier has been cast away.
 * @param[in]	r: the range from which to retrieve a pointer to an entry.
 * @param[in]	idx: the index of the entry to retrieve a pointer to. */
inline void* range_at(const Range* r, unsigned idx)
{
	/* Beware. This removes the const qualifier which is necessary for this function to operate on
	 * both const and non-const Ranges. */
	char* ptr = (char*)(r->entries);

	return ptr + (idx * range_elemsize(r));
}


/* range_indexof ********************************************************************************//**
 * @brief		Returns the index of a pointer to an element in the range.
 * 				For example, if the range is given by:
 *
 * 										Start			End
 * 										|				|
 * 										V				V
 * 					Index	0	1	2	3	4	5	6	7	8	9
 * 									^			^
 * 									|			|
 * 									ptr1		ptr2
 *
 * 				The index of ptr1 is 2.
 * 				The index of ptr2 is 5.
 * @param[in]	r: the range containing the element.
 * @param[in]	ptr: pointer to an element in the range.
 * @return		The index of the element pointed to by ptr. If ptr does not exist in the range, an
 *				out of bounds index is returned. */
inline unsigned range_indexof(const Range* r, const void* ptr)
{
	if(range_elemsize(r))
	{
		const char* first = range_at(r, 0);
		const char* last  = range_at(r, range_end(r));
		const char* p     = ptr;

		if(first <= p && p < last)
		{
			return (p - first) / range_elemsize(r);
		}
	}

	return -1u;
}


/* range_offsetof *******************************************************************************//**
 * @brief		Returns the offset of a pointer to an element in the range. The offset is the number
 *				of elements between the start index of the range to the element pointed to by ptr.
 *				For example, if the range is given by:
 *
 * 										Start			End
 * 										|				|
 * 										V				V
 * 					Index	0	1	2	3	4	5	6	7	8	9
 * 					Offset				0	1	2	3	4
 * 									^			^
 * 									|			|
 * 									ptr1		ptr2
 *
 * 				The offset of ptr1 is out of bounds.
 * 				The offset of ptr2 is 2.
 * @param[in]	r: the range containing the element.
 * @param[in]	ptr: pointer to an element in the range.
 * @return		The offset of the element pointed to by ptr. If ptr does not exist in the range, an
 *				out of bounds offset is returned. */
inline unsigned range_offsetof(const Range* r, const void* ptr)
{
	unsigned index = range_indexof(r, ptr);

	if(range_start(r) <= index && index < range_end(r))
	{
		return index - range_start(r);
	}
	else
	{
		return -1u;
	}
}


/* range_put ************************************************************************************//**
 * @brief		Copies a single element into the range at the specified index.
 * @param[in]	r: the range to copy into.
 * @param[in]	in: the data to copy.
 * @param[in]	idx: the index of the entry in the range to copy into.
 * @retval		true if the element was copied successfully.
 * @retval		false if the index is out of bounds. */
inline bool range_put(Range* r, const void* in, unsigned idx)
{
	return range_put_many(r, in, idx, 1);
}


/* range_put_many *******************************************************************************//**
 * @brief		Copies 'count' elements into the range starting at the specified index.
 * @param[in]	r: the range to copy into.
 * @param[in]	in: the data to copy.
 * @param[in]	start: the index of the first entry in the range to start copying into.
 * @param[in]	count: the number of elements to copy.
 * @retval		true if the data was copied successfully.
 * @retval		false if the indices were out of bounds. */
inline bool range_put_many(Range* r, const void* in, unsigned start, unsigned count)
{
	void* entry = range_entry(r, start);

	/* The entry pointer will be null if the start index is out of bounds. Also, ensure that the
	 * number of new elements does not overfill the range. */
	if(entry && in && count <= range_end(r) - start)
	{
		memmove(entry, in, count * range_elemsize(r));
		return true;
	}
	else
	{
		return false;
	}
}


/* range_get ************************************************************************************//**
 * @brief		Copies a single element out of the range from the specified index.
 * @param[in]	r: the range to copy out of.
 * @param[out]	out: buffer to hold a copy of the requested element.
 * @param[in]	idx: the index to entry copy.
 * @retval		true if the data was copied successfully.
 * @retval		false if the index is out of bounds. */
inline bool range_get(const Range* r, void* out, unsigned idx)
{
	return range_get_many(r, out, idx, 1);
}


/* range_get_many *******************************************************************************//**
 * @brief		Copies 'count' elements out of the range starting at the specified index.
 * @param[in]	r: the range to copy out of.
 * @param[out]	out: buffer to hold a copy of the requested elements.
 * @param[in]	start: the index of the first entry in the range to start copying out of.
 * @param[in]	count: the number of elements to copy.
 * @retval		true if the data was copied successfully.
 * @retval		false if the indices were out of bounds. */
inline bool range_get_many(const Range* r, void* out, unsigned start, unsigned count)
{
	const void* entry = range_entry(r, start);

	/* The entry pointer will be null if the start index is out of bounds. Also ensure that the
	 * number of requested elements is not more than the total number of elements in the range. */
	if(out && entry && count <= range_end(r) - start)
	{
		memmove(out, entry, count * range_elemsize(r));
		return true;
	}
	else
	{
		return false;
	}
}


#ifdef __cplusplus
}
#endif

#endif // RANGE_H
/******************************************* END OF FILE *******************************************/
