/************************************************************************************************//**
 * @file		range.h
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
#ifndef SEARCH_H
#define SEARCH_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher for inline support.
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include "compare.h"
#include "entry.h"
#include "range.h"


/* Public Functions ------------------------------------------------------------------------------ */
inline unsigned upper_range(const Range*, const void*, ICompare);
inline unsigned lower_range(const Range*, const void*, ICompare);
inline Range    equal_range(const Range*, const void*, ICompare);
inline bool     binfind    (const Range*, const void*, ICompare, Entry*);
inline bool     linfind    (const Range*, const void*, ICompare, Entry*);
inline Entry    binsearch  (const Range*, const void*, ICompare);
inline Entry    linsearch  (const Range*, const void*, ICompare);
       Entry    range_max  (const Range*, ICompare);
       Entry    range_min  (const Range*, ICompare);


/* upper_range **********************************************************************************//**
 * @brief		Returns an index to the first element in the sorted range that is greater than the
 *				specified value.
 * @param[in]	r: the sorted ascending range to search.
 * @param[in]	value: the value to search for.
 * @param[in]	comp: callback to perform comparisons between value and entries in the range.
 * 				The first parameter passed the comparison callback is the search value.
 * 				The second value passed to the comparison callback are entries in the range.
 * @return		Index of the first element greater than or equal to value. If every value is smaller
 *				than the search value, the index will be one past the end of the range. */
inline unsigned upper_range(const Range* r, const void* value, ICompare comp)
{
	unsigned start = range_start(r);
	unsigned count = range_count(r);

	while(count)
	{
		if(comp(value, range_at(r, start + count/2)) >= 0)
		{
			/* Move the start of the search range one element to the right. The element at
			 * start + count/2 has already been compared. Therefore, the new search range begins at
			 * start + count/2 + 1. Remove this extra element from count. */
			start += count/2 + 1;
			count -= 1;
		}

		count /= 2;
	}

	return start;
}


/* lower_range **********************************************************************************//**
 * @brief		Returns an index to the first element in the sorted range that is greater than or
 *				equal to the specified value.
 * @param[in]	r: the sorted ascending range to search.
 * @param[in]	value: the value to search for.
 * @param[in]	comp: callback to perform comparisons between value and entries in the range.
 * 				The first parameter passed the comparison callback is the search value.
 * 				The second value passed to the comparison callback are entries in the range.
 * @return		Index of the first element greater than or equal to value. If every value is smaller
 *				than the search value, the index will be one past the end of the range. */
inline unsigned lower_range(const Range* r, const void* value, ICompare comp)
{
	unsigned start = range_start(r);
	unsigned count = range_count(r);

	while(count)
	{
		if(comp(value, range_at(r, start + count/2)) > 0)
		{
			/* Move the start of the search range one element to the right. The element at
			 * start + count/2 has already been compared. Therefore, the new search range begins at
			 * start + count/2 + 1. Remove this extra element from count. */
			start += count/2 + 1;
			count -= 1;
		}

		count /= 2;
	}

	return start;
}


/* equal_range **********************************************************************************//**
 * @brief		Returns a subrange of equal elements. The original range is expected to be sorted.
 * @param[in]	r: the sorted ascending range to retrieve the equal subrange from.
 * @param[in]	value: the value to search for.
 * @param[in]	comp: callback to perform comparisons between value and entries in the range.
 * 				The first parameter passed the comparison callback is the search value.
 * 				The second value passed to the comparison callback are entries in the range.
 * @return		A range of equal elements from the specified range. */
inline Range equal_range(const Range* r, const void* value, ICompare comp)
{
	return make_range_slice(r, lower_range(r, value, comp), upper_range(r, value, comp));
}


/* binfind **************************************************************************************//**
 * @brief		Searches an ascending range for the specified value. If the value was found in the
 * 				range, the output entry's pointer and index will be set to the value. If the value
 * 				was not found, the output entry's pointer will be null but the index will be set to
 * 				the location where value can be inserted while maintaining a sorted range.
 * @param[in]	r: the range to search.
 * @param[in]	value: the search value.
 * @param[in]	comp: callback to perform comparisons between value and entries in the range.
 * 				The first parameter passed the comparison callback is the search value.
 * 				The second value passed to the comparison callback are entries in the range.
 * @param[out]	entry: buffer to hold the entry to the value if found, or the location to insert
 * 				value to maintain a sorted range.
 * @retval		true if the value was found.
 * @retval		false if the value was not found. */
inline bool binfind(const Range* r, const void* value, ICompare c, Entry* entry)
{
	*entry = binsearch(r, value, c);

	return eptr(entry) != 0;
}


/* linfind **************************************************************************************//**
 * @brief		Searches an unsorted range for the specified value. Returns true if the value was
 * 				found. Returns false if the value was not found. If the value was found, the output
 * 				entry will have it's pointer and index set to the value's location in the range. If
 * 				not found, the output entry will have it's pointer set to null.
 * @param[in]	r: the range to search.
 * @param[in]	value: the search value.
 * @param[in]	comp: callback to perform comparisons between value and entries in the range.
 * 				The first parameter passed the comparison callback is the search value.
 * 				The second value passed to the comparison callback are entries in the range.
 * @param[out]	entry: buffer to hold the entry to the value if found, or set to null if the value
 * 				was not found.
 * @retval		true if the value was found.
 * @retval		false if the value was not found. */
inline bool linfind(const Range* r, const void* value, ICompare c, Entry* entry)
{
	*entry = linsearch(r, value, c);

	return eptr(entry) != 0;
}


/* binsearch ************************************************************************************//**
 * @brief		Searches an ascending range for the specified value. Returns an entry with the
 *				pointer and index set to the specified value if found. If multiple matching values
 *				exist in the range, returns an entry to the first occurance of the value. If not
 *				found, the entry's pointer will be set to null and the index will be set so that
 *				value can be inserted into the range at index and still keep the range sorted.
 * @param[in]	r: the range to search.
 * @param[in]	value: the value to search for in the range.
 * @param[in]	comp: callback to perform comparisons between value and entries in the range.
 * 				The first parameter passed the comparison callback is the search value.
 * 				The second value passed to the comparison callback are entries in the range.
 * @return		An entry with index set to the sort position of value and pointer set to the matching
 *				value, or null if not found. */
inline Entry binsearch(const Range* r, const void* value, ICompare comp)
{
	/* Use lower_range which handles duplicate values in the range. In addition, lower_range finds
	 * the index where the search value can be inserted to maintain a sorted range. */
	unsigned idx = lower_range(r, value, comp);
	void*    ptr = range_entry(r, idx);

	if(ptr && comp(value, ptr) == 0)
	{
		return make_entry(ptr, idx);
	}
	else
	{
		return make_entry(0, idx);
	}
}


/* linsearch ************************************************************************************//**
 * @brief		Searches a range for the specified value. If multiple matching values exist in the
 * 				range, returns an entry to the first occurance of the value. If not found, the
 * 				entry's pointer will be set to null.
 * @param[in]	r: the range to search.
 * @param[in]	value: the value to search for.
 * @param[in]	comp: callback to perform comparisons between value and entries in the range.
 * 				The first parameter passed the comparison callback is the search value.
 * 				The second value passed to the comparison callback are entries in the range.
 * @return		Entry of the first instance of the specified element. */
inline Entry linsearch(const Range* r, const void* value, ICompare comp)
{
	unsigned i;
	for(i = range_start(r); i < range_end(r); i++)
	{
		void* ptr = range_at(r, i);

		if(comp(value, ptr) == 0)
		{
			return make_entry(ptr, i);
		}
	}

	return make_entry(0, i);
}


#ifdef __cplusplus
}
#endif

#endif // SEARCH_H
/******************************************* END OF FILE *******************************************/
