/************************************************************************************************//**
 * @file		order.c
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
#include <stdlib.h>

#include "order.h"


/* Private Functions ----------------------------------------------------------------------------- */
static void reverse_subrange(Range*, unsigned, unsigned);


/* ascending ************************************************************************************//**
 * @brief		Checks that all elements in the range are in ascending order.
 * @param[in]	r: the range to check.
 * @param[in]	compare: the callback used to compare two elements in the range.
 * @retval		true if the array is ascending.
 * @retval		false if the array is not ascending. */
bool ascending(const Range* r, const ICompare compare)
{
	unsigned i;
	for(i = range_start(r)+1; i < range_end(r); i++)
	{
		if(compare(range_at(r, i-1), range_at(r, i)) > 0)
		{
			return false;
		}
	}

	return true;
}


/* descending ***********************************************************************************//**
 * @brief		Checks that all elements in the range are in descending order.
 * @param[in]	r: the range to check.
 * @param[in]	compare: the callback used to compare two elements in the range.
 * @retval		true if the range is descending.
 * @retval		false if the range is not descending. */
bool descending(const Range* r, const ICompare compare)
{
	unsigned i;
	for(i = range_start(r)+1; i < range_end(r); i++)
	{
		if(compare(range_at(r, i-1), range_at(r, i)) < 0)
		{
			return false;
		}
	}

	return true;
}


/* equivalent ***********************************************************************************//**
 * @brief		Checks two ranges and returns true if both arrays contain equivalent elements.
 * @param[in]	a: the first range to check.
 * @param[in]	b: the second range to check.
 * @param[in]	compare: the function to compare two elements from the different ranges.
 * @retval		true if the ranges are equivalent (elements not necessarily the same address but
 *				equivalent in value).
 * @retval		false if the ranges are not equivalent. */
bool equivalent(const Range* a, const Range* b, const ICompare compare)
{
	if(range_count(a) != range_count(b))
	{
		return false;
	}

	unsigned i;
	for(i = 0; i < range_count(a); i++)
	{
		const void* at_a = range_offset(a, i);
		const void* at_b = range_offset(b, i);

		if(compare(at_a, at_b) != 0)
		{
			return false;
		}
	}

	return true;
}


/* shuffle **************************************************************************************//**
 * @brief		Randomly shuffles the elements in a Range. */
void shuffle(Range* r)
{
	unsigned i;
	for(i = range_start(r); i < range_end(r); i++)
	{
		range_swap(r, i, rand() % (i+1));
	}
}


/* reverse **************************************************************************************//**
 * @brief		Reverses the order of the elements in a range from [begin, end). */
void reverse(Range* r)
{
	reverse_subrange(r, range_start(r), range_end(r));
}


/* rotate_left **********************************************************************************//**
 * @brief		Rotates the elements in an array from begin to end so that middle becomes the new
 *				element at begin. The range is [begin, end). For example:
 *
 *							start		middle					end
 *							|			|						|
 *					Before	0	1	2	3	4	5	6	7	8	|
 *
 *					After	3	4	5	6	7	8	0	1	2
 *													|
 *													return
 *
 * @param[in]	r: the range of elements to perform the rotation on.
 * @param[in]	mid: the index of the element to rotate to the start of the range.
 * @return		index of the item that was previously located at the start of the range. */
unsigned rotate_left(Range* r, unsigned mid)
{
	if(range_start(r) < mid && mid < range_end(r))
	{
		reverse_subrange(r, range_start(r), mid);
		reverse_subrange(r, mid, range_end(r));
		reverse_subrange(r, range_start(r), range_end(r));

		return range_end(r) - (mid - range_start(r));
	}
	else
	{
		return 0;
	}
}


/* rotate_right *********************************************************************************//**
 * @brief		Rotates the elements in an array from begin to end so that middle becomes the new
 *				element at end. The range is [begin, end). For example:
 *
 *							start		middle					end
 *							|			|						|
 *					Before	0	1	2	3	4	5	6	7	8	|
 *
 *					After	4	5	6	7	8	0	1	2	3
 *												|
 *												return
 *
 * @param[in]	r: the range of elements perform the rotation on.
 * @param[in]	mid: the index of the element to rotate to the end of the range.
 * @return		index of the item that was previously located at the start of the range. */
unsigned rotate_right(Range* r, unsigned mid)
{
	return rotate_left(r, mid+1);
}


/* reverse_subrange *****************************************************************************//**
 * @brief		Reverses a subrange of elements. This is an internal function and start, end indices
 *				are not checked for validity. It is expected that range_start() <= start and
 *				end <= range_end().
 * @param[in]	r: the range containing the subrange to reverse.
 * @param[in]	start: the start index of the subrange to reverse.
 * @param[in]	end: the end index of the subrange to reverse.*/
static void reverse_subrange(Range* r, unsigned start, unsigned end)
{
	while(start < end)
	{
		range_swap(r, start++, --end);
	}
}


/******************************************* END OF FILE *******************************************/
