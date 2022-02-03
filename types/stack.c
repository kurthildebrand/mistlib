/************************************************************************************************//**
 * @file		stack.c
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
 * @brief		Implementation for a simple LIFO stack.
 *
 ***************************************************************************************************/
#include "order.h"
#include "stack.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern void     stack_init      (Stack*, void*, unsigned, unsigned);
extern void     stack_init_range(Stack*, Range);
extern void     stack_clear     (Stack*);
extern void*    stack_entry     (const Stack*, unsigned);
extern Key      stack_key       (const Stack*);
extern unsigned stack_count     (const Stack*);
extern unsigned stack_size      (const Stack*);
extern unsigned stack_elemsize  (const Stack*);
extern bool     stack_empty     (const Stack*);
extern bool     stack_full      (const Stack*);
extern void*    stack_next      (const Stack*);
extern bool     stack_push      (Stack*, const void*);
extern bool     stack_peek      (const Stack*, void*);
extern bool     stack_pop       (Stack*);
extern bool     stack_duplicate (Stack*);
extern bool     stack_swap      (Stack*);


/* stack_rotate *********************************************************************************//**
 * @brief		Rotates the elements in the stack. The top element becomes the last, the second
 *				element becomes the first, the third becomes the second, etc. down to the last
 *				element becoming the second to last. This process is repeated n times. Example:
 *
 *					Before Rotate:	0	1	2	3	4	5
 *					After Rotate:	5	0	1	2	3	4 (n = 1)
 *					After Rotate:	4	5	0	1	2	3 (n = 2)
 *					After Rotate:	4	5	0	1	2	3 (n = 7)
 *
 * @param[in]	s: the stack to operate on
 * @param[in]	n: the number of times to rotate the stack. This number may be larger than the
 *				stack's count. The resulting rotate is computed using the modulus of the stack's
 *				current count. */
void stack_rotate(Stack* s, unsigned n)
{
	Range* r = list_range(&s->list);

	rotate_left(r, n % stack_count(s));
}


/******************************************* END OF FILE *******************************************/
