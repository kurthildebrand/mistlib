/************************************************************************************************//**
 * @file		selsort.h
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
 * @brief		Selection sort is an in-place comparison sort with O(n^2) time complexity.
 * @desc		Selection sort is very similar to Insertion sort in that after the kth iteration the
 *				first k elements are in their sorted order. However, Selection sort may only perform
 *				O(n) swaps as opposed to O(n^2) for Insertion sort. Insertion sort generally performs
 *				faster than Selection sort because Selection sort must scan through the remainder of
 *				the array to place the next element.
 *
 *				Selection sort has very little variability in running time.
 *
 *				Summary
 *				Best Case:	O(n^2)
 *				Worst Case:	O(n^2)
 *				Space:		O(1)
 *				Writes:		O(n)
 *				Stable:		No
 *
 ***************************************************************************************************/
#ifndef SELSORT_H
#define SELSORT_H

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
void selsort(Range*, ICompare);


#ifdef __cplusplus
}
#endif

#endif // SELSORT_H
/******************************************* END OF FILE *******************************************/
