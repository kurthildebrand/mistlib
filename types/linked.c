/************************************************************************************************//**
 * @file		linked.c
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
#include "linked.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern void     linked_init         (Link**);
extern bool     linked_empty        (const Link*);
extern void*    linked_first        (const Link*);
extern void*    linked_last         (const Link*);
extern void*    linked_next         (const Link*, const Link*);
extern void*    linked_prev         (const Link*, const Link*);
extern void     linked_rotate       (Link**, Link*);

extern void*    linked_append       (Link**, Link*);
extern void*    linked_prepend      (Link**, Link*);
extern void*    linked_insert_before(Link**, Link*, Link*);
extern void*    linked_insert_after (Link**, Link*, Link*);
extern void*    linked_pop_front    (Link**);
extern void*    linked_pop_back     (Link**);
extern void*    linked_remove       (Link**, Link*);

extern void*    linked_push_before  (Link*, Link*);
extern void*    linked_push_after   (Link*, Link*);
extern void*    linked_join         (Link*, Link*);


/* linked_count *********************************************************************************//**
 * @brief		Returns the number of entries in the linked list. */
unsigned linked_count(const Link* head)
{
	const Link* ptr = linked_first(head);
	unsigned count  = 0;

	while(ptr)
	{
		count++;
		ptr = linked_next(head, ptr);	/* Sets iterator to 0 if iterator loops back to head */
	}

	return count;
}


/* linked_compare_insert ************************************************************************//**
 * @brief		Compares each element in the list with the given node and inserts the node after the
 * 				existing node if comparison == 0. */
void* linked_compare_insert(Link** head, Link* node, ICompare compare)
{
	if(!head || !node)
	{
		return 0;
	}

	Link* pos = linked_first(*head);

	while(pos && compare(node, pos) >= 0)
	{
		pos = linked_next(*head, pos);
	}

	return linked_insert_before(head, node, pos);
}


/******************************************* END OF FILE *******************************************/
