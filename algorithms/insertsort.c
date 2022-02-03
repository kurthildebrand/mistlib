/************************************************************************************************//**
 * @file		insertsort.c
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
#include "insertsort.h"
#include "order.h"
#include "search.h"


/* insertsort ***********************************************************************************//**
 * @brief		Sorts an array of elements using the insertion sort algorithm.
 * @param[in]	r: the range to sort.
 * @param[in]	comp: comparison callback function to compare two elements. */
void insertsort(Range* r, ICompare comp)
{
	unsigned i, j;

	/* At every iteration, elements up to i will be sorted and elements i and above will be unsorted.
	 * Bubble the element at i down until the lower range is sorted. Then move to the next
	 * element. */
	for(i = range_start(r); i < range_end(r); i++)
	{
		/* Swap elements until the lower range is sorted. The only unsorted element is the one
		 * element at j. Therefore, if element at j-1 < element at j, the lower range has been sorted
		 * and no further comparisons are needed. */
		for(j = i; j > range_start(r); j--)
		{
			if(comp(range_at(r,j-1), range_at(r,j)) > 0)
			{
				range_swap(r, j-1, j);
			}
			else
			{
				break;
			}
		}
	}
}


/******************************************* END OF FILE *******************************************/
