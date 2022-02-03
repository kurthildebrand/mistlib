/************************************************************************************************//**
 * @file		test_range.c
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
#include "test_array.h"
#include "tharness.h"


/* Private Variables ----------------------------------------------------------------------------- */
static int data[20];
static Array array;


TEST(test_array_put)
{
	int temp = 0;

	unsigned i;
	for(i = 0; i < array_count(&array); i++)
	{
		temp = i * 10;

		EXPECT(array_put(&array, &temp, i) == true);
	}

	temp = i * 10;

	EXPECT(array_put(&array, &temp, i) == false,
		"Out-of-bounds access at index = %d, array.count = %d", array_count(&array));
}


TEST(test_array_get)
{
	int temp = 0;

	unsigned i;
	for(i = 0; i < array_count(&array); i++)
	{
		EXPECT(array_get(&array, &temp, i) == true);
		EXPECT((int)i * 10 == temp);
	}

	EXPECT(array_get(&array, &temp, i) == false,
		"Out-of-bounds access at index = %d, array.count = %d", i, array_count(&array));
}


TEST(test_array_entry)
{
	unsigned i;
	for(i = 0; i < array_count(&array); i++)
	{
		int* ptr = array_entry(&array, i);

		EXPECT(ptr != 0);
	}

	EXPECT(array_entry(&array, array_count(&array)) == 0,
		"Out-Of-Bounds access at index = %d, array.size = %d", i, array_count(&array));
}


TEST(test_array_swap)
{
	unsigned i, j;
	for(i = 0, j = array_count(&array)-1; i < j; i++, j--)
	{
		EXPECT(array_swap(&array, i, j) == true);
	}

	for(i = 0; i < array_count(&array); i++)
	{
		int temp = 0;

		EXPECT(array_get(&array, &temp, i) == true);

		EXPECT(10 * (array_count(&array)-i-1) == (unsigned)temp);
	}

	EXPECT(array_swap(&array, array_count(&array), 0) == false,
		"Out-of-bounds access at first = %d, array.count = %d\n", array_count(&array), array_count(&array));

	EXPECT(array_swap(&array, 0, array_count(&array)) == false,
		"Out-of-bounds access at second = %d, array.count = %d\n", array_count(&array), array_count(&array));
}


void test_array(void)
{
	array_init(&array, data, sizeof(data) / sizeof(data[0]), sizeof(data[0]));

	tharness_run(test_array_put);
	tharness_run(test_array_get);
	tharness_run(test_array_swap);
}


/******************************************* END OF FILE *******************************************/
