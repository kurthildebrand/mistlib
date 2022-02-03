/************************************************************************************************//**
 * @file		selsort.c
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
#include "compare.h"
#include "search.h"
#include "selsort.h"


/* selsort **************************************************************************************//**
 * @brief		Sorts an array using the selection sort algorithm.
 * @param[in]	r: the range to sort.
 * @param[in]	compare: comparison callback function to compare two elements. */
void selsort(Range* r, const ICompare comp)
{
	Range s = *r;

	while(!range_empty(&s))
	{
		/* Search the remaining elements for the minimum element */
		Entry min = range_min(&s, comp);

		/* Swap the minimum element with the current element */
		range_swap(&s, range_start(&s), eidx(&min));

		s.start++;
	}

//	/* At every iteration, elements up to i will be sorted and elements i and above will be unsorted.
//	 * Repeatedly search the unsorted range for the minimum element and swap it to the current
//	 * location. */
//	unsigned i;
//	for(i = range_start(r); i < range_end(r); i++)
//	{
//		Range s = range_slice(r, i, range_end(r));
//
//		/* Search the remaining elements for the minimum element */
//		Entry min = range_min(&s, comp);
//
//		/* Swap the minimum element with the current element */
//		range_swap(&s, i, eidx(&min));
//	}
}


/******************************************* END OF FILE *******************************************/
