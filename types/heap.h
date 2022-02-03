/************************************************************************************************//**
 * @file		heap.h
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
 * @brief		Binary max heap data structure. This heap implementation keeps the old value in
 * 				removed entries. These behaviors allow this heap to be used to sort arrays.
 *
 ***************************************************************************************************/
#ifndef HEAP_H
#define HEAP_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher for inline support.
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <stdbool.h>

#include "compare.h"
#include "entry.h"
#include "range.h"


/* Public Type Declaration ----------------------------------------------------------------------- */
typedef struct {
	Key      key;
	Range    range;
	unsigned size;
	ICompare compare;
} Heap;


/* Public Functions ------------------------------------------------------------------------------ */
inline void     heap_init      (Heap*, void*, unsigned, unsigned, ICompare);
inline void     heap_init_range(Heap*, Range, ICompare);
inline void     heap_clear     (Heap* h)       { h->range.end = h->range.start;        }
inline Key      heap_key       (const Heap* h) { return h->key;                        }
inline unsigned heap_size      (const Heap* h) { return h->size;                       }
inline unsigned heap_elemsize  (const Heap* h) { return range_elemsize(&h->range);     }
inline unsigned heap_count     (const Heap* h) { return range_count(&h->range);        }
inline bool     heap_empty     (const Heap* h) { return heap_count(h) == 0;            }
inline bool     heap_full      (const Heap* h) { return heap_count(h) == heap_size(h); }
inline void*    heap_next      (const Heap*);
       void*    heap_entry     (const Heap*, unsigned);

       void     heapsort       (Range*, ICompare);
       bool     heap_push      (Heap*, const void*);
       void*    heap_reserve   (Heap*);
inline bool     heap_peek      (const Heap*, void*);
       bool     heap_peek_at   (const Heap*, void*, unsigned);
inline bool     heap_pop       (Heap*);
       Entry    heap_search    (const Heap*, const void*, ICompare);
       bool     heap_update    (Heap*, unsigned);
       bool     heap_remove    (Heap*, unsigned);


/* heap_init ************************************************************************************//**
 * @brief		Initializes a heap structure to point to the specified array with size number of
 *				elements of elemsize width.
 * @param[in]	h: the heap to initialize.
 * @param[in]	data: pointer to the heap's data buffer.
 * @param[in]	size: the number of entries in the data buffer.
 * @param[in]	elemsize: the number of bytes in an entry of the data buffer.
 * @param[in]	compare: comparison callback which compares and orders two elements in the heap. */
inline void heap_init(Heap* h, void* data, unsigned size, unsigned elemsize, ICompare compare)
{
	range_init(&h->range, data, 0, elemsize);
	h->size    = size;
	h->compare = compare;
}


/* heap_init_range ******************************************************************************//**
 * @brief		Initializes an empty heap with elements contained in the specified range.
 * @param[in]	h: the heap to initialize.
 * @param[in]	r: the range representing the heap's entries.
 * @param[in]	compare: comparison callback which compares two elements in the heap. */
inline void heap_init_range(Heap* h, Range r, ICompare compare)
{
	h->size    = range_count(&r);
	h->compare = compare;
	range_slice(&h->range, &r, range_start(&r), range_start(&r));
}


/* heap_next ************************************************************************************//**
 * @brief		Returns a pointer to the next element to be remoeved from the heap. The elements in
 *				the heap contain information vital for maintaining the heap and therefore must not
 *				be modified through the returned pointer.
 * @param[in]	h: the heap to retreieve a pointer from
 * @return		Pointer to the next element to be removed from the heap. Null if the heap is
 *				empty. */
inline void* heap_next(const Heap* h)
{
	return heap_entry(h, 0);
}


/* heap_peek ************************************************************************************//**
 * @brief		Retrieves the top item from the heap if the item exists.
 * @param[in]	h: the heap from which an element is retrieved.
 * @param[out]	out: output buffer to place the retrieved item.
 * @retval		true if the operation succeeds.
 * @retval		false if the heap is empty. */
inline bool heap_peek(const Heap* h, void* out)
{
	return heap_peek_at(h, out, 0);
}


/* heap_pop *************************************************************************************//**
 * @brief		Removes the top entry from the heap. The top entry will be moved to the back of the
 * 				heap.
 * @param[in]	h: the heap to remove the top entry from.
 * @retval		true if the top entry was removed successfully.
 * @retval		false if the heap is empty. */
inline bool heap_pop(Heap* h)
{
	return heap_remove(h, 0);
}


#ifdef __cplusplus
}
#endif

#endif // HEAP_H
/******************************************* END OF FILE *******************************************/
