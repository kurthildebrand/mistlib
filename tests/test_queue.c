/************************************************************************************************//**
 * @file		test_queue.c
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

#include "queue.h"


/* Private Functions ----------------------------------------------------------------------------- */
static void* _Atomic ptrs[32];
static int   data[32];
static Queue queue;


TEST(test_queue_push)
{
	int i;

	queue_init(&queue, ptrs, sizeof(ptrs) / sizeof(ptrs[0]));

	/* Enqueue 10 items */
	for(i = 0; i < 10; i++)
	{
		data[i] = i * 100;
		EXPECT(queue_push(&queue, &data[i]));
	}

	/* Remove 10 items */
	for(i = 0; i < 10; i++)
	{
		int* ptr;

		EXPECT(queue_get(&queue, &ptr));
		EXPECT(*ptr == i * 100);
	}

	/* Fill up the queue */
	for(i = 0; i < (int)queue_size(&queue); i++)
	{
		data[i] = i * 10;
		EXPECT(queue_push(&queue, &data[i]));
	}

	/* Expect failure to enqueue an item into a full queue */
	int temp = -1;
	EXPECT(!queue_push(&queue, &temp));
	EXPECT(queue_full(&queue));
}


TEST(test_queue_entry)
{
	int i;

	for(i = 0; i < (int)queue_count(&queue); i++)
	{
		int* ptr = queue_entry(&queue, i);

		EXPECT(*ptr == i * 10);
	}
}


TEST(test_queue_pop)
{
	int* ptr;
	int i = 0;
	while(queue_peek(&queue, &ptr))
	{
		EXPECT(*ptr == i * 10);
		EXPECT(queue_pop(&queue));
		i++;
	}

	EXPECT(!queue_pop(&queue));
	EXPECT(queue_empty(&queue));
}


void test_queue(void)
{
	tharness_run(test_queue_push);
	tharness_run(test_queue_entry);
	tharness_run(test_queue_pop);
}


/******************************************* END OF FILE *******************************************/
