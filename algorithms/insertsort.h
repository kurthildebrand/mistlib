/************************************************************************************************//**
 * @file		insertsort.h
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
 * @brief		Insertion sort is an in-place comparison sort with O(n^2) time complexity.
 * @desc		Insertion sort is very similar to Selection sort in that after the kth iteration the
 *				first k elements are in their sorted order. Insertion sort performs more writes than
 *				selection sort (O(n^2) for insertion sort vs. O(n) for selection sort). However,
 *				Insertion sort may outperform selection sort in that insertion sort only scans as
 *				many elements as it needs to to place the next element.
 *
 *				Summary
 *				Best Case:	O(n) comparisons, O(1) swaps (for already sorted array).
 *				Worst Case:	O(n^2) comparisons and swaps
 *				Space:		O(1)
 *				Writes:		O(n)
 *				Stable:		Yes
 *
 ***************************************************************************************************/
#ifndef INSERTSORT_H
#define INSERTSORT_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher for inline support.
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include "compare.h"
#include "range.h"


/* Public Functions ------------------------------------------------------------------------------ */
void insertsort(Range*, ICompare);


#ifdef __cplusplus
}
#endif

#endif // INSERTSORT_H
/******************************************* END OF FILE *******************************************/
