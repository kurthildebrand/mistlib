/************************************************************************************************//**
 * @file		stack.h
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
 * @brief		Public structures and functions for a stack.
 *
 ***************************************************************************************************/
#ifndef STACK_H
#define STACK_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <stdbool.h>

#include "list.h"


/* Public Types ---------------------------------------------------------------------------------- */
typedef struct Stack {
	List list;
} Stack;


/* Public Functions ------------------------------------------------------------------------------ */
inline void     stack_init      (Stack*, void*, unsigned, unsigned);
inline void     stack_init_range(Stack*, Range);
inline void     stack_clear     (Stack* s)       { list_clear(&s->list);           }
inline void*    stack_entry     (const Stack*, unsigned);
inline Key      stack_key       (const Stack* s) { return list_key(&s->list);      }
inline unsigned stack_count     (const Stack* s) { return list_count(&s->list);    }
inline unsigned stack_size      (const Stack* s) { return list_size(&s->list);     }
inline unsigned stack_elemsize  (const Stack* s) { return list_elemsize(&s->list); }
inline bool     stack_empty     (const Stack* s) { return list_empty(&s->list);    }
inline bool     stack_full      (const Stack* s) { return list_full(&s->list);     }
inline void*    stack_next      (const Stack*);
inline bool     stack_push      (Stack*, const void*);
inline bool     stack_peek      (const Stack*, void*);
inline bool     stack_pop       (Stack*);
inline bool     stack_duplicate (Stack*);
inline bool     stack_swap      (Stack*);
       void     stack_rotate    (Stack*, unsigned);


/* stack_init ***********************************************************************************//**
 * @brief		Initializes an empty stack to point to the data buffer with size number of elements
 *				of elemsize width.
 * @param[in]	s: the stack to initialize.
 * @param[in]	data: the buffer to hold stack entries.
 * @param[in]	size: the capacity of the stack.
 * @param[in]	elemsize: the size of an entry in bytes. */
inline void stack_init(Stack* s, void* data, unsigned size, unsigned elemsize)
{
	list_init(&s->list, data, 0, size, elemsize);
}


/* stack_init_range *****************************************************************************//**
 * @brief		Initializes an empty stack with elements contained in the specified range.
 * @param[in]	s: the stack to initialize.
 * @param[in]	r: the range containing the stack's elements. */
inline void stack_init_range(Stack* s, Range r)
{
	list_init_range(&s->list, r, 0);
}


/* stack_entry **********************************************************************************//**
 * @brief		Returns a pointer to the specified entry in the stack indexed from the top of the
 *				stack. Index 0 is the top entry, index 1 is the second entry, etc.
 * @param[in]	s: the stack to retrieve an entry from.
 * @param[in]	i: the index of the item.
 * @return		Pointer to the entry at index i. Null if there is no entry at the desired index. */
inline void* stack_entry(const Stack* s, unsigned idx)
{
	return list_entry(&s->list, stack_count(s) - idx - 1);
}


/* stack_next ***********************************************************************************//**
 * @brief		Returns a pointer to the top entry in the stack.
 * @param[in]	s: the stack to retrieve the next entry from.
 * @return		Pointer to the top entry in the stack. Null if the stack is empty. */
inline void* stack_next(const Stack* s)
{
	return stack_entry(s, 0);
}


/* stack_push ***********************************************************************************//**
 * @brief		Pushes an element onto the stack.
 * @param[in]	s: the stack to operate on.
 * @param[in]	var: the new element to push onto the stack.
 * @retval		true if the new element was pushed successfully.
 * @retval		false if the stack is full. */
inline bool stack_push(Stack* s, const void* in)
{
	return list_append(&s->list, in);
}


/* stack_peek ***********************************************************************************//**
 * @brief		Gets the next element in the stack.
 * @param[in]	s: the stack to operate on.
 * @param[out]	out: the requested element if stack not empty.
 * @retval		true if peek() succeeded.
 * @retval		false if the stack was empty. */
inline bool stack_peek(const Stack* s, void* out)
{
	return list_get(&s->list, out, stack_count(s)-1);
}


/* stack_pop ************************************************************************************//**
 * @brief		Pops a element from the stack.
 * @param[in]	s: pointer to the stack to be operated on.
 * @retval		true of an element was popped successfully.
 * @retval		false if the stack was empty. */
inline bool stack_pop(Stack* s)
{
	return list_pop(&s->list);
}


/* stack_duplicate ******************************************************************************//**
 * @brief		Duplicates the top element of the stack.
 * @param[in]	s: the stack to operate on.
 * @retval		true if the top element was duplicated successfully.
 * @retval		false if the stack is either empty or full. */
inline bool stack_duplicate(Stack* s)
{
	/* Grab a pointer to the current top entry. This pointer will be null if no entries exist in the
	 * stack. Since pushing entries onto the stack does not modify previous entries, this pointer can
	 * be passed directly to stack_push() to append a duplicate. */
	void* top = stack_next(s);

	return top && stack_push(s, top);
}


/* stack_swap ***********************************************************************************//**
 * @brief		Swaps the top and second from the top elements in the stack.
 * @param[in]	s: the stack to operate on.
 * @retval		true if the 2 top elements were swapped successfully.
 * @retval		false if the swap failed because there were not enough elements. */
inline bool stack_swap(Stack* s)
{
	return list_swap(&s->list, stack_count(s)-1, stack_count(s)-2);
}


#ifdef __cplusplus
}
#endif

#endif // STACK_H
/******************************************* END OF FILE *******************************************/
