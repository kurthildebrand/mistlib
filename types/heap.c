/************************************************************************************************//**
 * @file		heap.c
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
 * @brief		Implementation of a binary max heap data structure.
 *
 ***************************************************************************************************/
#include "compare.h"
#include "heap.h"
#include "range.h"
#include "search.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern void     heap_init      (Heap*, void*, unsigned, unsigned, ICompare);
extern void     heap_init_range(Heap*, Range, ICompare);
extern void     heap_clear     (Heap*);
extern Key      heap_key       (const Heap*);
extern unsigned heap_size      (const Heap*);
extern unsigned heap_elemsize  (const Heap*);
extern unsigned heap_count     (const Heap*);
extern bool     heap_empty     (const Heap*);
extern bool     heap_full      (const Heap*);
extern void*    heap_next      (const Heap*);
extern bool     heap_peek      (const Heap*, void*);
extern bool     heap_pop       (Heap*);


/* Private Type Declarations --------------------------------------------------------------------- */
static void heap_siftdown(Heap*, unsigned);
static void heap_siftup  (Heap*, unsigned);


/* Inline Functions ------------------------------------------------------------------------------ */
static inline Range* heap_range  (const Heap*);
static inline void*  heap_at     (const Heap*, unsigned);
static inline bool   heap_compare(const Heap*, unsigned, unsigned);
static inline void   heap_swap   (Heap*, unsigned, unsigned);


/* heapsort *************************************************************************************//**
 * @brief		Sorts an array using the heap sort algorithm.
 * @param[in]	r: the range to sort.
 * @param[in]	comp: callback which compares two elements in the range. */
void heapsort(Range* r, const ICompare comp)
{
	/* Initialize a full heap. Can't use heap_init because heap_init sets the count to zero. */
	Heap h;
	h.range   = *r;
	h.size    = range_count(r);
	h.compare = comp;

	/* Heapify the array. Last node is at count-1. It's parent is at (count-2)/2 = count/2-1. Since i
	 * is decremented, i is initialized to count/2 so that when the loop starts, i = count/2-1. */
	unsigned i;
	for(i = heap_count(&h)/2; i-- > 0; )
	{
		heap_siftdown(&h, i);
	}

	/* Remove elements from the heap leaving a sorted array. */
	while(heap_pop(&h)) { }
}


/* heap_push ************************************************************************************//**
 * @brief		Copies a new value into the heap.
 * @param[in]	h: the heap to place a new value into.
 * @param[in]	in: the new value.
 * @retval		true if the new value was successfully copied into the heap.
 * @retval		false if the heap is full. */
bool heap_push(Heap* h, const void* in)
{
	void* ptr = heap_reserve(h);

	if(in && ptr)
	{
		unsigned count = (h->range.end++) - (h->range.start);

		/* Do not copy if 'in' points to the next entry in the heap (from heap_reserve) */
		if(ptr != in)
		{
			memmove(ptr, in, heap_elemsize(h));
		}

		heap_siftup(h, count);
		return true;
	}
	else
	{
		return false;
	}
}


/* heap_reserve *********************************************************************************//**
 * @brief		Returns a pointer to the next unused entry in the heap if the heap is not full. Call
 * 				heap_push to insert the reserved item.
 * @param[in]	h: the heap from which to reserve a new item.
 * @return		Pointer to the reserved item. Null if the heap is full. */
void* heap_reserve(Heap* h)
{
	if(!heap_full(h))
	{
		return heap_at(h, heap_count(h));
	}
	else
	{
		return 0;
	}
}


/* heap_peek_at *********************************************************************************//**
 * @brief		Retrieves the specified item from the heap if the item exists.
 * @param[in]	h: the heap from which an element is retrieved.
 * @param[out]	out: output buffer to place the retrieved item.
 * @param[in]	idx: the index from which to retrieve the item.
 * @retval		true if the operation succeeds.
 * @retval		false if the heap is empty. */
bool heap_peek_at(const Heap* h, void* out, unsigned idx)
{
	Range* r = heap_range(h);

	return range_get(r, out, idx);
}


/* heap_search **********************************************************************************//**
 * @brief		Searches a heap for an element with the specified key if it exists.
 * @warning		Expects elements stored in the heap to be items with a Key.
 * @warning		The index of the element may change if the heap is modified after a call to
 *				heap_searfh.
 * @param[in]	h: the heap to search.
 * @param[in]	value: the value to search the heap for.
 * @param[in]	c: the comparison callback which compares elements in the heap to the search
 * 				value. */
Entry heap_search(const Heap* h, const void* value, ICompare c)
{
	Range* r = heap_range(h);

	return linsearch(r, value, c);
}


/* heap_update **********************************************************************************//**
 * @brief		Updates the heap from the specified index. Use this function to restore the heap
 * 				property when the entry at the specfieid index changes.
 * @param[in]	h: the heap to update.
 * @param[in]	idx: the index of the entry which changes.
 * @retval		true if the heap was updated successfully.
 * @retval		false if the index is out of bounds. */
bool heap_update(Heap* h, unsigned idx)
{
	/* Determine if the updated entry needs to be sifted up or down by comparing the updated entry
	 * with its parent. Comparing with the parent requires only one comparison whereas comparing with
	 * children requires two. */
	// if(idx < range_start(&h->range) || idx >= range_end(&h->range))
	if(idx >= heap_count(h))
	{
		return false;
	}
	else if(idx != 0 && !heap_compare(h, (idx-1)/2, idx))
	{
		heap_siftup(h, idx);
	}
	else
	{
		heap_siftdown(h, idx);
	}

	return true;
}


/* heap_remove **********************************************************************************//**
 * @brief		Deletes the node at the specified index.
 * @param[in]	h: pointer to the heap.
 * @param[in]	idx: the index of the node to delete..
 * @retval		true if the operation succeeds.
 * @retval		false if the heap is empty. */
bool heap_remove(Heap* h, unsigned idx)
{
	/* Remove the element at idx if idx is in range */
	if(idx < heap_count(h))
	{
		/* Swap the removed entry with the last entry in the heap. */
		heap_swap(h, idx, (h->range.end-1) - (h->range.start));

		/* Decrement count. Do this after swapping to prevent swap from failing because of an out of
		 * bounds index. */
		h->range.end--;

		/* Restore the heap property */
		heap_update(h, idx);

		return true;
	}
	else
	{
		return false;
	}
}


/* heap_entry ***********************************************************************************//**
 * @brief		Returns a pointer to the element at the specified index.
 * @warning		Call heap_update with the entry's index if the entry is modified through the returned
 * 				pointer.
 * @param[in]	h: pointer to the heap.
 * @param[in]	idx: the index of the element to retrieve a pointer to.
 * @return		Pointer to the element at the specified index. */
void* heap_entry(const Heap* h, unsigned idx)
{
	Range* r = heap_range(h);

	return range_entry(r, idx);
}


/* heap_siftdown ********************************************************************************//**
 * @brief		Sifts a node down the heap till the heap property is satisfied.
 * @param[in]	h: pointer to the heap.
 * @param[in]	idx: the index of the node to sift down. */
static void heap_siftdown(Heap* h, unsigned idx)
{
	/* Loop ends when the left child (2*idx+1) goes beyond the end of the array */
	while(2*idx+1 < heap_count(h))
	{
		unsigned swap = 2*idx+1;

		/* Determine if the left or right child needs to be swapped. Swap the bigger one but don't
		 * swap if the right child is outside the bounds. */
		if(swap+1 < heap_count(h) && heap_compare(h, swap+1, swap))
		{
			swap = swap + 1;
		}

		/* If the parent (idx) is greater than the child (swap) then the heap property has been
		 * satisfied. */
		if(heap_compare(h, idx, swap))
		{
			break;
		}

		heap_swap(h, idx, swap);
		idx = swap;
	}
}


/* heap_siftup **********************************************************************************//**
 * @brief		Sifts a node up the heap till the heap property is satisfied.
 * @param[in]	h: pointer to the heap.
 * @param[in]	idx: the index of the node to sift down. */
static void heap_siftup(Heap* h, unsigned idx)
{
	/* Loop until the index reaches the root of the heap */
	while(idx != 0)
	{
		/* Get the parent of the node */
		unsigned swap = (idx-1)/2;

		/* If the parent (swap) >= child (idx) then the heap property has been
		 * satisfied */
		if(heap_compare(h, swap, idx))
		{
			break;
		}

		heap_swap(h, idx, swap);
		idx = swap;
	}
}


/* heap_range ***********************************************************************************//**
 * @brief		Returns a range of valid entries for the entire array. */
static inline Range* heap_range(const Heap* h)
{
	return (Range*)&h->range;
}


/* heap_at **************************************************************************************//**
 * @brief		Returns a pointer to the element at index i.
 * @warning		This function does not check for out of bounds access.
 * @param[in]	a: the array to retrieve the element from.
 * @param[in]	i: the index to retrieve from.
 * @return		Pointer to the element at index i. */
static inline void* heap_at(const Heap* h, unsigned idx)
{
	Range* r = heap_range(h);

	return range_at(r, idx);
}


/* heap_compare *********************************************************************************//**
 * @brief		Compares two items in the heap. Returns true if a >= b. Returns false if a < b. The
 *				comparison returns true if the max heap property is satisfied. Note that the
 *				comparison is done through a comparison callback. Therefore, a max heap can be turned
 *				into a min heap by providing a comparison function which swaps the comparison.
 *				Consider:
 *
 * 					Normal Compare (Max Heap)				Reversed Compare (Min Heap)
 * 					a > b  returns >0, heap_compare true	a > b  returns <0, heap_compare false
 * 					a == b returns  0, heap_compare true	a == b returns 0,  heap_compare true
 * 					a < b  returns <0, heap_compare false	a < b  returns >0, heap_compare true
 *
 * @param[in]	h: pointer to the heap.
 * @param[in]	a: index of the entry closest to the root of the heap.
 * @param[in]	b: index of the entry furthest from the root of the heap. */
static inline bool heap_compare(const Heap* h, unsigned a, unsigned b)
{
	return h->compare(heap_at(h, a), heap_at(h, b)) >= 0;
}


/* heap_swap ************************************************************************************//**
 * @brief		Swaps two elements in the heap. Does not perform an out of bounds check on the
 * 				indices
 * @param[in]	h: the heap containing the elements to swap.
 * @param[in]	a: index of the first element to swap.
 * @param[in]	b: index of the second element to swap. */
static inline void heap_swap(Heap* h, unsigned a, unsigned b)
{
	Range* r = heap_range(h);

	range_swap(r, a, b);
}


/******************************************* END OF FILE *******************************************/
