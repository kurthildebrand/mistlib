/************************************************************************************************//**
 * @file		list.h
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
#ifndef LIST_H
#define LIST_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <stdbool.h>

#include "key.h"
#include "range.h"
#include "utils.h"


/* Public Macros --------------------------------------------------------------------------------- */
/* MAKE_LIST ************************************************************************************//**
 * @brief		Initializes a List at compile time. This macro may be called with a varying number of
 *				arguments.
 * @param[in]	k: the key of the List.
 * @param[in]	d: pointer to the List's data.
 * @param[in]	s: total number of entries in the List.
 * @param[in]	e: the number of bytes per entry.
 * @desc		MAKE_LIST(d): Initializes a List to point to a raw C array. The size and element size
 *					are automatically set using sizeof. In addition, the List's count is set to size
 *					to create a full List. The macro performs the following operation:
 *
 *						const int data[] = {
 *							10, 20, 30, 40, 50
 *						};
 *
 *						const List l1 = {
 *							.entries  = data,
 *							.elemsize = sizeof(data[0]),
 *							.size     = sizeof(data) / sizeof(data[0]),
 *							.count    = sizeof(data) / sizeof(data[0]),
 *						};
 *
 *						const List l2 = { MAKE_LIST(data) };
 *
 *				MAKE_LIST(k,d): Performs the same initialization as MAKE_LIST(1) but also sets the
 *					List's key.
 *
 *				MAKE_LIST(d,s,e): Sets the List's pointer, size, and element size. The List's count
 *					is set to size creating a full List.
 *
 *				MAKE_LIST(k,d,s,e): Sets the List's key, pointer, size, and element size. */
#define MAKE_LIST(...) 			APPEND_NARGS(MAKE_LIST, __VA_ARGS__)
#define MAKE_LIST1(d)			MAKE_LIST3(     (d), sizeof((d))/sizeof((d)[0]), sizeof((d)[0]))
#define MAKE_LIST2(k,d)			MAKE_LIST4((k), (d), sizeof((d))/sizeof((d)[0]), sizeof((d)[0]))
#define MAKE_LIST3(d,s,e)		(List){           .range=MAKE_RANGE3(d,s,e), .size=(s) }
#define MAKE_LIST4(k,d,s,e)		(List){ .key=(k), .range=MAKE_RANGE3(d,s,e), .size=(s) }


/* Public Types ---------------------------------------------------------------------------------- */
typedef struct {
	Key key;
	Range range;
	unsigned size;
} List;


/* Public Functions ------------------------------------------------------------------------------ */
inline bool     list_init         (List*, void*, unsigned, unsigned, unsigned);
inline void     list_const_init   (List*, const void*, unsigned, unsigned);
inline bool     list_init_range   (List*, Range, unsigned);
inline List     make_list         (void*, unsigned, unsigned, unsigned);
inline List     make_const_list   (const void*, unsigned, unsigned);
inline List     list_from_range   (Range, unsigned);

inline void     list_clear        (List* l)       { l->range.end = l->range.start;        }
inline Range*   list_range        (const List*);
inline Key      list_key          (const List* l) { return l->key;                        }
inline unsigned list_size         (const List* l) { return l->size;                       }
inline unsigned list_elemsize     (const List* l) { return range_elemsize(&l->range);     }
inline unsigned list_count        (const List* l) { return range_count(&l->range);        }
inline unsigned list_start        (const List* l) { return range_start(&l->range);        }
inline unsigned list_end          (const List* l) { return range_end(&l->range);          }
inline unsigned list_free         (const List* l) { return list_size(l) - list_count(l);  }
inline bool     list_empty        (const List* l) { return list_count(l) == 0;            }
inline bool     list_full         (const List* l) { return list_count(l) == list_size(l); }
inline void*    list_entry        (const List*, unsigned);

       bool     list_insert_many  (List*, const void*, unsigned, unsigned);
inline bool     list_append_many  (List*, const void*, unsigned);
inline bool     list_prepend_many (List*, const void*, unsigned);
       void*    list_reserve_many (List*, unsigned, unsigned);
inline bool     list_get_many     (const List*, void*, unsigned, unsigned);
inline bool     list_replace_many (List*, const void*, unsigned, unsigned);
       bool     list_remove_many  (List*, unsigned, unsigned);
inline bool     list_pop_many     (List*, unsigned);

inline bool     list_insert       (List*, const void*, unsigned);
inline bool     list_append       (List*, const void*);
inline bool     list_prepend      (List*, const void*);
inline void*    list_reserve      (List*, unsigned);
inline void*    list_reserve_front(List*);
inline void*    list_reserve_back (List*);
inline bool     list_get          (const List*, void*, unsigned);
inline bool     list_replace      (List*, const void*, unsigned);
inline bool     list_remove       (List*, unsigned);
inline bool     list_pop          (List*);
inline bool     list_swap         (List*, unsigned, unsigned);


/* list_init ************************************************************************************//**
 * @brief		Initializes a list to point to the specified data.
 * @param[in]	l: the list to initialize.
 * @param[in]	data: the buffer to hold list entries.
 * @param[in]	count: the current number of entries in the list.
 * @param[in]	size: the total number of entries that can be stored in the list.
 * @param[in]	elemsize: the size of an entry in bytes. */
inline bool list_init(List* l, void* data, unsigned count, unsigned size, unsigned elemsize)
{
	if(count > size)
	{
		return false;
	}

	range_init(&l->range, data, count, elemsize);
	l->size = size;
	return true;
}


/* list_const_init ******************************************************************************//**
 * @brief		Initializes a list to point to the specified const data. The list is initialized
 * 				full.
 * @param[in]	l: the list to initialize.
 * @param[in]	data: the buffer to hold list entries.
 * @param[in]	size: the total number of entries in the list.
 * @param[in]	elemsize: the size of an entry in bytes. */
inline void list_const_init(List* l, const void* data, unsigned size, unsigned elemsize)
{
	range_const_init(&l->range, data, size, elemsize);
	l->size = size;
}


/* list_init_range ******************************************************************************//**
 * @brief		Initializes a list from the specified range.
 * @param[in]	l: the list to initialize.
 * @param[in]	r: the range representing the array's entries.
 * @param[in]	count: the current number of entries in the list. */
inline bool list_init_range(List* l, Range r, unsigned count)
{
	if(count > range_count(&r))
	{
		return false;
	}

	range_slice(&l->range, &r, range_start(&r), range_start(&r) + count);
	l->size = range_count(&r);
	return true;
}


/* make_list ************************************************************************************//**
 * @brief		Creates a list pointing to the specified data.
 * @param[in]	data: the list's entries.
 * @param[in]	count: the current number of entries in the list.
 * @param[in]	size: the total number of entries in the list.
 * @param[in]	elemsize: the size of an entry in bytes. */
inline List make_list(void* data, unsigned count, unsigned size, unsigned elemsize)
{
	if(count > size)
	{
		return (List){
			.range = make_range(0, 0, 0),
			.size  = 0,
		};
	}
	else
	{
		return (List){
			.range = make_range(data, count, elemsize),
			.size  = size,
		};
	}
}


/* make_const_list ******************************************************************************//**
 * @brief		Creates a list pointing to the specified const data. The list is initialized full.
 * @param[in]	data: the list's const entries.
 * @param[in]	size: the total number of entries in the list.
 * @param[in]	elemsize: the size of an entry in bytes. */
inline List make_const_list(const void* data, unsigned size, unsigned elemsize)
{
	return (List){
		.range = make_const_range(data, size, elemsize),
		.size  = size,
	};
}


/* list_from_range ******************************************************************************//**
 * @brief		Creates a list from a range.
 * @param[in]	r: the range representing the list's entries.
 * @param[in]	count: the list's current count. */
inline List list_from_range(Range r, unsigned count)
{
	if(count > range_count(&r))
	{
		return (List){
			.range = make_range(0, 0, 0),
			.size  = 0,
		};
	}
	else
	{
		return (List){
			.range = make_range_slice(&r, range_start(&r), range_start(&r) + count),
			.size  = range_count(&r),
		};
	}
}


/* list_range ***********************************************************************************//**
 * @brief		Returns a range of valid entries for the list. */
inline Range* list_range(const List* l)
{
	return (Range*)&l->range;
}


/* list_entry ***********************************************************************************//**
 * @brief		Returns a pointer to the specified entry in the list.
 * @warning		This function removes the const qualifier from the returned data pointer. Do not try
 * 				to modify const data. However, non-const data can be safely modified even if the
 * 				const qualifier has been cast away.
 * @param[in]	l: the list to retrieve the entry from.
 * @param[in]	idx: the index of the entry to retrieve.
 * @return		Pointer to the specified entry. Null if the index is out of range. */
inline void* list_entry(const List* l, unsigned idx)
{
	Range* r = list_range(l);

	return range_entry(r, idx);
}


/* list_append_many *****************************************************************************//**
 * @brief		Copies 'count' elements to the end of the list.
 * @param[in]	l: the list to append to.
 * @param[in]	in: pointer to the buffer of elements to insert.
 * @param[in]	count: the number of new elements.
 * @retval		true if the new elements were appended successfully.
 * @retval		false if the list is full. */
inline bool list_append_many(List* l, const void* in, unsigned count)
{
	return list_insert_many(l, in, list_end(l), count);
}


/* list_prepend_many ****************************************************************************//**
 * @brief		Copies 'count' elements to the beginning of the list. Shifts existing elements down
 * 				to make room for the new elements.
 * @param[in]	l: the list to prepend to.
 * @param[in]	in: pointer to the buffer of elements to prepend.
 * @param[in]	count: the number of new elements.
 * @retval		true if the new elements were prepended successfully.
 * @retval		false if the list is full. */
inline bool list_prepend_many(List* l, const void* in, unsigned count)
{
	return list_insert_many(l, in, list_start(l), count);
}


/* list_get_many ********************************************************************************//**
 * @brief		Copies 'count' elements out of the list starting at the specified index.
 * @param[in]	l: the list to retrieve elements from.
 * @param[out]	out: buffer to hold a copy of the requested elements.
 * @param[in]	start: the index of the first element to copy from the list.
 * @param[in]	count: the number of elements to retrieve.
 * @retval		true if the elements were retrieved successfully.
 * @retval		false if the elements were not retrieved successfully because of invalid inputs. */
inline bool list_get_many(const List* l, void* out, unsigned start, unsigned count)
{
	Range* r = list_range(l);

	return range_get_many(r, out, start, count);
}


/* list_replace_many ****************************************************************************//**
 * @brief		Overwrites 'count' elements starting at the specified index. Grows the list if adding
 * 				'count' elements exceeds the previous end of the list AND the list has the capacity
 * 				to accept the new elements.
 * @param[in]	l: the list to replace elements on.
 * @param[in]	in: buffer of the new elements.
 * @param[in]	start: the index of the first element to start replacing.
 * @param[in]	count: the number of elements to replace.
 * @retval		true if the elements were replaced successfully.
 * @retval		false if the elements were not replaced successfully because of invalid inputs. */
inline bool list_replace_many(List* l, const void* in, unsigned start, unsigned count)
{
	if(start > list_end(l))
	{
		return false;
	}

	unsigned overlap = list_end(l) - start;

	if(count > overlap && !list_reserve_many(l, list_end(l), count - overlap))
	{
		return false;
	}

	Range* range = list_range(l);

	range_put_many(range, in, start, count);
	return true;
}


/* list_pop_many ********************************************************************************//**
 * @brief		Removes 'count' elements from the back of the list.
 * @param[in]	l: the list to pop elements from.
 * @param[in]	count: the number of element to pop.
 * @retval		true if the elements were popped successfully.
 * @retval		false if the elements were not popped. */
inline bool list_pop_many(List* l, unsigned count)
{
	return list_remove_many(l, list_count(l) - count, count);
}


/* list_insert **********************************************************************************//**
 * @brief		Inserts a new element into the list at the specified element. Shifts existing
 *				elements down to make room for the new element.
 * @param[in]	l: the list to insert the element into.
 * @param[in]	in: the new element.
 * @param[in]	idx: the index to insert the new element into.
 * @retval		true if the element was inserted successfully.
 * @retval		false if the list is full. */
inline bool list_insert(List* l, const void* in, unsigned idx)
{
	return list_insert_many(l, in, idx, 1);
}


/* list_append **********************************************************************************//**
 * @brief		Copies an element to the end of the list.
 * @param[in]	l: the list to append onto.
 * @param[in]	in: the element to copy.
 * @retval		true if the element was appended successfully.
 * @retval		false if the list is full. */
inline bool list_append(List* l, const void* in)
{
	return list_append_many(l, in, 1);
}


/* list_prepend *********************************************************************************//**
 * @brief		Copies an element to the start of the list. Shifts existing elements down to make
 *				room for the new element.
 * @param[in]	l: the list to prepend onto.
 * @param[in]	in: the element to copy.
 * @retval		true if the element was prepended successfully.
 * @retval		false if the list is full. */
inline bool list_prepend(List* l, const void* in)
{
	return list_prepend_many(l, in, 1);
}


/* list_reserve *********************************************************************************//**
 * @brief		Moves entries in the list to make room for a new entry and returns a pointer to the
 *				blank entry.
 * @warning		Beware that the reserved entry may move if the list is modified. Ensure that the
 *				pointer returned from this function has been discarded before the list is modified.
 * @param[in]	l: the list to emplace an element into.
 * @param[in]	idx: the index to emplace the new entry.
 * @return		Pointer to the blank entry. Null if the list is full or the index is out of
 *				bounds. */
inline void* list_reserve(List* l, unsigned idx)
{
	return list_reserve_many(l, idx, 1);
}


/* list_reserve_front ***************************************************************************//**
 * @brief		Shifts entries in the List to reserve room for a new entry at the front of the List.
 * @warning		Beware that the reserved entry may move if the list is modified. Ensure that the
 *				pointer returned from this function has been discarded before the list is modified.
 * @param[in]	l: the list to emplace onto.
 * @return		Pointer to the reserved entry. Null if reserving the entry failed. */
inline void* list_reserve_front(List* l)
{
	return list_reserve(l, list_start(l));
}


/* list_reserve_back ****************************************************************************//**
 * @brief		Reserves the last entry in the list.
 * @warning		Beware that the reserved entry may move if the list is modified. Ensure that the
 *				pointer returned from this function has been discarded before the list is modified.
 * @param[in]	l: the list to emplace onto.
 * @return		Pointer to the reserved entry. Null if reserving the entry failed. */
inline void* list_reserve_back(List* l)
{
	return list_reserve(l, list_end(l));
}


/* list_get *************************************************************************************//**
 * @brief		Retrieves a copy of the requested element from the list.
 * @param[in]	l: the list to retrieve an element from.
 * @param[out]	out: buffer to hold a copy of the element.
 * @param[in]	idx: the index of the element to retrieve.
 * @retval		true if the element was retrieved successfully.
 * @retval		false if the list is empty or the index is out of bounds. */
inline bool list_get(const List* l, void* out, unsigned idx)
{
	return list_get_many(l, out, idx, 1);
}


/* list_replace *********************************************************************************//**
 * @brief		Replaces an entry in the list with a new element.
 * @param[in]	l: the list containing the entry to replace.
 * @param[in]	in: the element to copy into the list.
 * @param[in]	idx: the index to copy the element to.
 * @retval		true if the entry was replaced successfully.
 * @retval		false if the list is empty or the index was out of bounds. */
inline bool list_replace(List* l, const void* in, unsigned idx)
{
	return list_replace_many(l, in, idx, 1);
}


/* list_remove **********************************************************************************//**
 * @brief		Removes an entry from the list. Shifts elements up overwriting the specified element.
 * @param[in]	l: the list to remove an entry from.
 * @param[in]	idx: the index of the entry to remove.
 * @retval		true if the entry was removed successfully.
 * @retval		false if the list is empty or the index was out of range. */
inline bool list_remove(List* l, unsigned idx)
{
	return list_remove_many(l, idx, 1);
}


/* list_pop *************************************************************************************//**
 * @brief		Removes the last entry from the list.
 * @param[in]	l: the list to pop the entry from.
 * @retval		true if the entry was popped successfully.
 * @retval		false if the list is empty. */
inline bool list_pop(List* l)
{
	if(!list_empty(l))
	{
		l->range.end--;
		return true;
	}
	else
	{
		return false;
	}
}


/* list_swap ************************************************************************************//**
 * @brief		Swaps two entries in the list.
 * @param[in]	l: the list containing the entries to swap.
 * @param[in]	first: the index of the first entry.
 * @param[in]	second: the index of the second entry.
 * @retval		true if the entries were swapped successfully.
 * @retval		false if at least one of the indices is out of bounds. */
inline bool list_swap(List* l, unsigned first, unsigned second)
{
	Range* r = list_range(l);

	return range_swap(r, first, second);
}


#ifdef __cplusplus
}
#endif

#endif // LIST_H
/******************************************* END OF FILE *******************************************/
