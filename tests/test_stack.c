/************************************************************************************************//**
 * @file		test_stack.c
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
#include <stdio.h>
#include <stdlib.h>

#include "tharness.h"

#include "array.h"
#include "order.h"
#include "stack.h"


/* Private Functions ----------------------------------------------------------------------------- */
static int data[10];
static Stack stack;


TEST(test_stack_push)
{
	int temp;

	unsigned i;

	for(i = 0; i < stack_size(&stack); i++)
	{
		temp = i * 10;

		EXPECT(stack_push(&stack, &temp) == true);
	}

	EXPECT(stack_full(&stack) == true);

	EXPECT(stack_peek(&stack, &temp) == true);
	EXPECT((stack_size(&stack) - 1) * 10 == (unsigned)temp);

	EXPECT(stack_push(&stack, &temp) == false);
}


TEST(test_stack_pop)
{
	int temp;

	unsigned i;

	unsigned count = stack_count(&stack);

	for(i = 0; i < count; i++)
	{
		EXPECT(stack_peek(&stack, &temp) == true);

		EXPECT(stack_pop(&stack) == true);

		EXPECT((stack_size(&stack) - i - 1) * 10 == (unsigned)temp);
	}

	EXPECT(stack_empty(&stack) == true);

	EXPECT(stack_pop(&stack) == false);
}


TEST(test_stack_duplicate)
{
	int i;
	int temp;

	stack_clear(&stack);

	EXPECT(stack_duplicate(&stack) == false, "stack_duplicate erroneously succeeded on an empty stack.");

	i = 10;
	EXPECT(stack_push(&stack, &i) == true);
	EXPECT(stack_duplicate(&stack) == true);
	EXPECT(stack_peek(&stack, &temp) == true);
	EXPECT(i == temp);

	i = 20;
	EXPECT(stack_push(&stack, &i) == true);
	EXPECT(stack_duplicate(&stack) == true);
	EXPECT(stack_peek(&stack, &temp) == true);
	EXPECT(i == temp);

	i = 30;
	EXPECT(stack_push(&stack, &i) == true);

	/* At this point the stack should be full */
	EXPECT(stack_duplicate(&stack) == false, "stack_duplicate did not prevent modifying a full stack.");
}


TEST(test_stack_swap)
{
	int i;

	stack_clear(&stack);

	EXPECT(stack_swap(&stack) == false, "stack_swap did not prevent modifying an empty stack.");

	i = 10;
	EXPECT(stack_push(&stack, &i) == true);
	i = 20;
	EXPECT(stack_push(&stack, &i) == true);

	int* a = stack_entry(&stack, 0);
	int* b = stack_entry(&stack, 1);

	EXPECT(stack_swap(&stack) == true);
	EXPECT(10 == *a);
	EXPECT(20 == *b);

	i = 30;
	EXPECT(stack_push(&stack, &i) == true);
	i = 40;
	EXPECT(stack_push(&stack, &i) == true);
	i = 50;
	EXPECT(stack_push(&stack, &i) == true);

	a = stack_entry(&stack, 0);
	b = stack_entry(&stack, 1);

	EXPECT(stack_swap(&stack) == true);
	EXPECT(40 == *a);
	EXPECT(50 == *b);
}


TEST(test_stack_rotate)
{
	int temp;

	unsigned i;

	stack_clear(&stack);

	for(i = 0; i < stack_size(&stack); i++)
	{
		temp = 10 * (i + 1);

		stack_push(&stack, &temp);
	}

	int* a = stack_entry(&stack, 0);
	int* b = stack_entry(&stack, 1);
	int* c = stack_entry(&stack, 2);
	int* d = stack_entry(&stack, 3);
	int* e = stack_entry(&stack, 4);

	stack_rotate(&stack, 1);
	EXPECT(10 == *a);
	EXPECT(50 == *b);
	EXPECT(40 == *c);
	EXPECT(30 == *d);
	EXPECT(20 == *e);

	stack_rotate(&stack, 2);
	EXPECT(30 == *a);
	EXPECT(20 == *b);
	EXPECT(10 == *c);
	EXPECT(50 == *d);
	EXPECT(40 == *e);

	stack_rotate(&stack, 7);
	EXPECT(50 == *a);
	EXPECT(40 == *b);
	EXPECT(30 == *c);
	EXPECT(20 == *d);
	EXPECT(10 == *e);
}


void test_stack(void)
{
	stack_init(&stack, data, 5, sizeof(data[0]));

	tharness_run(test_stack_push);
	tharness_run(test_stack_pop);
	tharness_run(test_stack_duplicate);
	tharness_run(test_stack_swap);
	tharness_run(test_stack_rotate);
}


/******************************************* END OF FILE *******************************************/
