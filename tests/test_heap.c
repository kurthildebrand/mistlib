/************************************************************************************************//**
 * @file		test_ringbuffer.c
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
#include "compare.h"
#include "heap.h"
#include "order.h"


/* Private Functions ----------------------------------------------------------------------------- */
static int sorted_data[30];
static int unsorted_data[30];
static int data[8];
static Heap heap;


TEST(test_heapsort)
{
	Range range_equal = make_range(sorted_data, 2, sizeof(sorted_data[0]));

	int temp = 10;
	range_put(&range_equal, &temp, 0);
	range_put(&range_equal, &temp, 1);

	heapsort(&range_equal, compare_int);


	Range range_sorted = make_range(sorted_data, sizeof(sorted_data) / sizeof(sorted_data[0]), sizeof(sorted_data[0]));
	Range range_unsorted = make_range(unsorted_data, sizeof(unsorted_data) / sizeof(unsorted_data[0]), sizeof(unsorted_data[0]));

	int i;
	int size = range_count(&range_unsorted);

	for(i = 0; i < size; i++)
	{
		int temp = rand() % 100;

		range_put(&range_sorted, &temp, i);
		range_put(&range_unsorted, &temp, i);
	}

	heapsort(&range_sorted, compare_int);

	unsigned j;
	for(j = 0; j < sizeof(sorted_data) / sizeof(sorted_data[0]) - 1; j++)
	{
		if(sorted_data[j+1] < sorted_data[j])
		{
			TEST_FAIL("Array is not sorted");
		}
	}

	TEST_PASS("Array is sorted");

	PRINT_LINE("Unsorted\tSorted");

	for(i = 0; i < size; i++)
	{
		if(i+1 < size && unsorted_data[i+1] < unsorted_data[i])
		{
			PRINT(" * %d\t", unsorted_data[i]);
		}
		else if(i+1 < size)
		{
			PRINT("   %d\t", unsorted_data[i]);
		}

		if(i+1 < size && sorted_data[i+1] < sorted_data[i])
		{
			PRINT(" * %d\n", sorted_data[i]);
		}
		else if(i+1 < size)
		{
			PRINT("   %d\n", sorted_data[i]);
		}
	}
}


TEST(test_heap_put)
{
	int temp;

	unsigned i;
	for(i = 0; i < heap_size(&heap); i++)
	{
		temp = rand() % 100;

		EXPECT(heap_push(&heap, &temp) == true);
	}

	EXPECT(heap_full(&heap) == true);

	EXPECT(heap_push(&heap, &temp) == false);
}


TEST(test_heap_get)
{
	int temp;

	unsigned count = heap_count(&heap);

	unsigned i;
	for(i = 0; i < count; i++)
	{
		EXPECT(heap_peek(&heap, &temp) == true);

		EXPECT(heap_pop(&heap) == true);
	}

	EXPECT(heap_empty(&heap) == true);

	EXPECT(heap_peek(&heap, &temp) == false);
	EXPECT(heap_pop(&heap) == false);
}


void test_heap(void)
{
	heap_init(&heap, data, sizeof(data) / sizeof(data[0]), sizeof(data[0]), compare_int);

	tharness_run(test_heapsort);
	tharness_run(test_heap_put);
	tharness_run(test_heap_get);
}

/******************************************* END OF FILE *******************************************/
