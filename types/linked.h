/************************************************************************************************//**
 * @file		linked.h
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
 * @brief		Implementation of a circular linked list with forward and backward pointers. No
 * 				sentinel nodes are used meaning that the entire list can be traversed from any
 * 				intermediate node.
 *
 ***************************************************************************************************/
#ifndef LINKED_H
#define LINKED_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <stdbool.h>

#include "compare.h"
#include "utils.h"


/* Public Macros --------------------------------------------------------------------------------- */
#define LINKED_CONTAINER(list, node, field) \
	((list != 0) ? CONTAINER_OF(list, __typeof__(*node), field) : 0)

#define LINKED_FIRST_CONTAINER(list, node, field) \
	LINKED_CONTAINER(linked_first(list), node, field)

#define LINKED_LAST_CONTAINER(list, node, field) \
	LINKED_CONTAINER(linked_last(list), node, field)

#define LINKED_NEXT_CONTAINER(list, node, field) \
	((node != 0) ? LINKED_CONTAINER(linked_next(list, &(node->field)), node, field) : 0)

#define LINKED_PREV_CONTAINER(list, node, field) \
	((node != 0) ? LINKED_CONTAINER(linked_prev(list, &(node->field)), node, field) : 0)

#define LINKED_INSERT_BEFORE(list, a, b, field) \
	(linked_insert_before(list, (a) ? &(a->field) : 0, &(b->node))

#define LINKED_INSERT_AFTER(list, a, b, field) \
	(linked_insert_after(list, (a) ? &(a->field) : 0, &(b->node))

#define LINKED_FOREACH(list, ptr, next) \
	for((ptr) = linked_first((list)), (next) = linked_next((list), (ptr)); (ptr) != 0; \
		(ptr) = (next), (next) = linked_next((list), (next)))

#define LINKED_FOREACH_CONTAINER(list, ptr, next, field)       \
	for((ptr)  = LINKED_FIRST_CONTAINER((list), (ptr), field), \
	    (next) = LINKED_NEXT_CONTAINER((list), (ptr), field);  \
	    (ptr) != 0;                                            \
	    (ptr)  = (next),                                       \
	    (next) = LINKED_NEXT_CONTAINER((list), (ptr), field))


/* Public Types ---------------------------------------------------------------------------------- */
typedef struct Link {
	struct Link* next;
	struct Link* prev;
} Link;


/* Public Functions ------------------------------------------------------------------------------ */
inline void     linked_init          (Link** head) { *head = 0; }
inline void     linked_node_init     (Link*  node) { node->next = 0; node->prev = 0;    }
       unsigned linked_count         (const Link*);
inline bool     linked_empty         (const Link* head) { return head == 0;             }
inline void*    linked_first         (const Link* head) { return (void*)head;           }
inline void*    linked_last          (const Link* head) { return head ? head->prev : 0; }
inline void*    linked_next          (const Link*, const Link*);
inline void*    linked_prev          (const Link*, const Link*);
inline void     linked_rotate        (Link**, Link*);

inline void*    linked_append        (Link**, Link*);
inline void*    linked_prepend       (Link**, Link*);
inline void*    linked_insert_before (Link**, Link*, Link*);
inline void*    linked_insert_after  (Link**, Link*, Link*);
       void*    linked_compare_insert(Link**, Link*, ICompare);
inline void*    linked_remove        (Link**, Link*);
inline void*    linked_pop_front     (Link**);
inline void*    linked_pop_back      (Link**);

inline void*    linked_push_before   (Link*, Link*);
inline void*    linked_push_after    (Link*, Link*);
inline void*    linked_join          (Link*, Link*);


/* linked_next **********************************************************************************//**
 * @brief		Returns the next node in the linked list. Returns null if the next node loops back
 * 				to the start of the list. */
inline void* linked_next(const Link* head, const Link* n)
{
	return (!head || !n || head == n->next) ? 0 : n->next;
}


/* linked_prev **********************************************************************************//**
 * @brief		Returns the previous node in the linked list. Returns null if the previous node loops
 * 				back to the start of the list. */
inline void* linked_prev(const Link* head, const Link* n)
{
	return (!head || !n || head == n->prev) ? 0 : n->prev;
}


/* linked_rotate ********************************************************************************//**
 * @brief		Sets the head of the list to the specified node. */
inline void linked_rotate(Link** head, Link* n)
{
	*head = n;
}


/* linked_append ********************************************************************************//**
 * @brief		Appends a link onto the linked list. Returns a pointer to the newly inserted node. */
inline void* linked_append(Link** head, Link* node)
{
	if(!head || !node)
	{
		return 0;
	}
	else if(*head)
	{
		linked_push_before(node, *head);
	}
	else
	{
		node->next = node;
		node->prev = node;
		*head = node;
	}

	return node;
}


/* linked_prepend *******************************************************************************//**
 * @brief		Prepends a link onto the linked list. Returns a pointer to the newly inserted
 * 				node. */
inline void* linked_prepend(Link** head, Link* node)
{
	if(!head || !node)
	{
		return 0;
	}
	else if(*head)
	{
		*head = linked_push_before(node, *head);
	}
	else
	{
		node->next = node;
		node->prev = node;
		*head = node;
	}

	return node;
}


/* linked_insert_before *************************************************************************//**
 * @brief		Inserts the new node 'a' before the node 'b'. If 'pos' is null, 'a' is appended
 * 				to the linked list. Returns a pointer to the newly inserted node. */
inline void* linked_insert_before(Link** head, Link* a, Link* b)
{
	if(!b)
	{
		return linked_append(head, a);
	}
	else if(b == linked_first(*head))
	{
		return linked_prepend(head, a);
	}
	else
	{
		return linked_push_before(a, b);
	}
}


/* linked_insert_after **************************************************************************//**
 * @brief		Inserts the new node 'b' after the node existing node 'a'. If 'a' is null, 'b' is
 * 				appended to the linked list. Returns a pointer to the newly inserted node. */
inline void* linked_insert_after(Link** head, Link* a, Link* b)
{
	if(!a)
	{
		return linked_append(head, b);
	}
	else
	{
		return linked_push_after(a, b);
	}
}


/* linked_remove ********************************************************************************//**
 * @brief		Removes a node from the linked list and returns the removed node. */
inline void* linked_remove(Link** head, Link* node)
{
	if(!head || !node)
	{
		return 0;
	}
	else if(linked_first(*head) == node)
	{
		*head = linked_next(*head, node);
	}

	node->prev->next = node->next;
	node->next->prev = node->prev;

	return node;
}


/* linked_pop_front *****************************************************************************//**
 * @brief		Removes the first node in the linked list and returns a pointer to the removed
 * 				node. */
inline void* linked_pop_front(Link** head)
{
	return head ? linked_remove(head, linked_first(*head)) : 0;
}


/* linked_pop_back ******************************************************************************//**
 * @brief		Removes the last node in the linked list and returns a pointer to the removed
 * 				node. */
inline void* linked_pop_back(Link** head)
{
	return head ? linked_remove(head, linked_last(*head)) : 0;
}


/* linked_push_before ***************************************************************************//**
 * @brief		Places a new node 'a' before existing node 'b'. Returns a pointer to the newly
 * 				inserted node. */
inline void* linked_push_before(Link* a, Link* b)
{
	a->next = b;
	a->prev = b->prev;
	b->prev->next = a;	/* Order is important. Must assign prev->next... */
	b->prev       = a;	/* ...before changing prev. */
	return a;
}


/* linked_push_after ****************************************************************************//**
 * @brief		Places a new node 'b' after existing node 'a'. Returns a pointer to the newly
 * 				inserted node. */
inline void* linked_push_after(Link* a, Link* b)
{
	b->next = a->next;
	b->prev = a;
	a->next->prev = b;	/* Order is important. Must assign next->prev... */
	a->next       = b;	/* ...before changing next. */
	return b;
}


/* linked_join **********************************************************************************//**
 * @brief		Joins a linked list starting from node 'b' into another list after node 'a'.
 * 				Returns a pointer to node 'b' in the new combined list. Example:
 *
 * 					Before:
 * 					+--> x --> y --> z --> a >--+   +--> b --> c --> d >--+
 * 					|                           |   |                     |
 * 					+---------------------------+   +---------------------+
 *
 * 					After:
 * 					+--> x --> y --> z --> a --> b --> c --> d >--+
 * 					|                                             |
 * 					+---------------------------------------------+
 */
inline void* linked_join(Link* a, Link* b)
{
	a->next->prev = b->prev;
	b->prev->next = a->next;
	a->next = b;
	b->prev = a;
	return b;
}


#ifdef __cplusplus
}
#endif

#endif // SLIST_H
/******************************************* END OF FILE *******************************************/
