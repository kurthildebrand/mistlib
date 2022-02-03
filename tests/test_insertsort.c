/************************************************************************************************//**
 * @file		test_insertsort.c
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

#include "array.h"
#include "compare.h"
#include "order.h"
#include "insertsort.h"
#include "tharness.h"


/* Private Variables ----------------------------------------------------------------------------- */
static int sorted_data[30];
static int unsorted_data[30];


TEST(test_insertsort_runner)
{
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

	insertsort(&range_sorted, compare_int);

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


void test_insertsort(void)
{
	tharness_run(test_insertsort_runner);
}


/******************************************* END OF FILE *******************************************/
