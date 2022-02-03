/************************************************************************************************//**
 * @file		list.c
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
 ***************************************************************************************************/
#include "list.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern bool     list_init         (List*, void*, unsigned, unsigned, unsigned);
extern void     list_const_init   (List*, const void*, unsigned, unsigned);
extern bool     list_init_range   (List*, Range, unsigned);
extern List     make_list         (void*, unsigned, unsigned, unsigned);
extern List     make_const_list   (const void*, unsigned, unsigned);
extern List     list_from_range   (Range, unsigned);

extern void     list_clear        (List*);
extern Range*   list_range        (const List*);
extern Key      list_key          (const List*);
extern unsigned list_size         (const List*);
extern unsigned list_elemsize     (const List*);
extern unsigned list_count        (const List*);
extern unsigned list_start        (const List*);
extern unsigned list_end          (const List*);
extern unsigned list_free         (const List*);
extern bool     list_empty        (const List*);
extern bool     list_full         (const List*);
extern void*    list_entry        (const List*, unsigned);

extern bool     list_append_many  (List*, const void*, unsigned);
extern bool     list_prepend_many (List*, const void*, unsigned);
extern bool     list_get_many     (const List*, void*, unsigned, unsigned);
extern bool     list_replace_many (List*, const void*, unsigned, unsigned);
extern bool     list_pop_many     (List*, unsigned);

extern bool     list_insert       (List*, const void*, unsigned);
extern bool     list_append       (List*, const void*);
extern bool     list_prepend      (List*, const void*);
extern void*    list_reserve      (List*, unsigned);
extern void*    list_reserve_front(List*);
extern void*    list_reserve_back (List*);
extern bool     list_get          (const List*, void*, unsigned);
extern bool     list_replace      (List*, const void*, unsigned);
extern bool     list_remove       (List*, unsigned);
extern bool     list_pop          (List*);
extern bool     list_swap         (List*, unsigned, unsigned);


/* Private Functions ----------------------------------------------------------------------------- */
static inline void* list_at(const List*, unsigned);


/* list_insert_many *****************************************************************************//**
 * @brief		Places 'count' elements at the specified index. Shifts existing elements down to make
 *				room for the new elements.
 * @param[in]	l: the list to operate on.
 * @param[in]	idx: the index at which the new elements will reside.
 * @param[in]	in: pointer to the element to insert.
 * @param[in]	count: the number of new elements.
 * @retval		true if the new elements were inserted successfully.
 * @retval		false if not enough room. */
bool list_insert_many(List* l, const void* in, unsigned idx, unsigned count)
{
	if(!in)
	{
		return false;
	}

	/* Reserve space at idx for the requested number of elements */
	void* ptr = list_reserve_many(l, idx, count);

	/* Check that reserving entries succeeded. If it failed, either because the index was out of
	 * range or there is not enough space in the list, then the pointer to the reserved entries will
	 * be null. */
	if(!ptr)
	{
		return false;
	}

	/* Copy the new elements into the list */
	memmove(ptr, in, count * list_elemsize(l));
	return true;
}


/* list_reserve_many ****************************************************************************//**
 * @brief		Movies entries in the list to make room for many consecutive entries and returns a
 *				pointer to the first blank entry.
 * @warning		Beware that the reserved entry may move if the list is modified. Ensure that the
 *				pointer returned from this function has been discarded before the list is modified.
 * @warning		No guarantees are given regarding the alignment of the returned pointer. If elements
 *				in the list aren't aligned, use list_insert(), list_prepend(), or list_append()
 *				instead.
 * @param[in]	l: the list to emplace elements into.
 * @param[in]	idx: the index at which the first blank entry will be located.
 * @param[in]	count: the number of blank entries to reserve.
 * @return		Pointer to the first blank entry. Null if the list is full, the index is
 *				out of bounds, or the list does not have capacity for 'count' more entries, or count
 * 				is zero. */
void* list_reserve_many(List* l, unsigned idx, unsigned count)
{
	/* Fail is the index is out of range or reserving count elements would overflow the list.
	 * Otherwise, if the reserved elements are at the end of the list, no list modification is
	 * required. If the reserved elements aren't at the end of the list, then existing elements need
	 * to move to make room for the new elements */
	if(idx > list_count(l) || count == 0 || count > list_free(l))
	{
		return 0;
	}
	else if(idx < list_count(l))
	{
		/* Move overlapping elements down to make room for count elements. Number of elements to move
		 * are counted from idx to the end of the list. */
		unsigned n = list_count(l) - idx;

		memmove(list_at(l, idx + count), list_at(l, idx), n * list_elemsize(l));
	}

	l->range.end += count;

	return list_at(l, idx);
}


/* list_remove_many *****************************************************************************//**
 * @brief		Removes 'count' elements from the specified index. If there is not 'count' elements
 * 				to remove, then elements from idx to the end of the list are removed.
 * @param[in]	l: the list to operate on.
 * @param[in]	idx: the index of the item to remove.
 * @param[in]	count: the number of elements to remove.
 * @retval		true if the elements were removed from the list.
 * @retval		false if more elements were requested to be removed than exist from the index. */
bool list_remove_many(List* l, unsigned idx, unsigned count)
{
	if(idx > list_count(l) || count > list_count(l) - idx)
	{
		return false;
	}
	else
	{
		/* Compute the number of elements that need to shift up to overwrite the existing
		 * elements. */
		unsigned n = list_count(l) - count - idx;

		/* Shift elements up. It's possible that no elements are required to be shifted up. In that
		 * case, subtracting count is enough to remove the elements from the list. */
		if(n != 0)
		{
			memmove(list_at(l, idx), list_at(l, idx + count), n * list_elemsize(l));
		}

		l->range.end -= count;

		return true;
	}
}


/* list_at **************************************************************************************//**
 * @brief		Returns a pointer to the entry at index idx.
 * @warning		Does not check idx for out of bounds access. */
static inline void* list_at(const List* l, unsigned idx)
{
	Range* r = list_range(l);

	return range_at(r, idx);
}


/******************************************* END OF FILE *******************************************/
