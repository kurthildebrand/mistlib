/************************************************************************************************//**
 * @file		search.c
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
#include "order.h"
#include "search.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern unsigned upper_range(const Range*, const void*, ICompare);
extern unsigned lower_range(const Range*, const void*, ICompare);
extern Range    equal_range(const Range*, const void*, ICompare);
extern bool     binfind    (const Range*, const void*, ICompare, Entry*);
extern bool     linfind    (const Range*, const void*, ICompare, Entry*);
extern Entry    binsearch  (const Range*, const void*, ICompare);
extern Entry    linsearch  (const Range*, const void*, ICompare);


/* range_max ************************************************************************************//**
 * @brief		Returns an entry to the maximum element in the range.
 * @param[in]	r: the range to search for the maximum value.
 * @param[in]	compare: the callback to compare two elements in the range. */
Entry range_max(const Range* r, const ICompare compare)
{
	unsigned i;

	Entry max = make_range_entry(r, range_start(r));

	for(i = range_start(r); i < range_end(r); i++)
	{
		if(compare(range_at(r, i), eptr(&max)) > 0)
		{
			max = make_entry(range_at(r, i), i);
		}
	}

	return max;
}


/* range_min ************************************************************************************//**
 * @brief		Returns an entry to the minimum element in the range.
 * @param[in]	r: the range to search for the minimum value.
 * @param[in]	compare: the callback to compare two elements in the range. */
Entry range_min(const Range* r, ICompare compare)
{
	unsigned i;

	Entry min = make_range_entry(r, range_start(r));

	for(i = range_start(r); i < range_end(r); i++)
	{
		if(compare(range_at(r, i), eptr(&min)) < 0)
		{
			min = make_entry(range_at(r, i), i);
		}
	}

	return min;
}


/******************************************* END OF FILE *******************************************/
