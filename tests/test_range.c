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

#include "range.h"
#include "tharness.h"


/* Private Variables ----------------------------------------------------------------------------- */
static int data[20];
Range range;


TEST(test_range_init)
{
	EXPECT(10 == range_count(&range));
	EXPECT(sizeof(int) == range_elemsize(&range));
}


TEST(test_range_put)
{
	const int exp_data[10] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 };

	int temp;

	unsigned i;

	for(i = 0; i < range_count(&range); i++)
	{
		temp = i * 10;

		EXPECT(range_put(&range, &temp, i) == true);
	}

	temp = i * 10;

	EXPECT(range_put(&range, &temp, i) == false);

	for(i = 0; i < range_count(&range); i++)
	{
		/* The range points to data offset by 1. */
		if(exp_data[i] != data[i+1])
		{
			TEST_FAIL("Arrays not equal");
		}
	}

	TEST_PASS("Arrays are equal");
}


TEST(test_range_get)
{
	int temp;

	unsigned i;

	/* Leave the range untouched between test_range_put and test_range_get. This means that the data
	 * (0, 10, 20, ... 90) is still in the range. */
	for(i = 0; i < range_count(&range); i++)
	{
		EXPECT(range_get(&range, &temp, i) == true);
		EXPECT((int)i*10 == temp);
	}

	/* Expect range_get to fail for out of bounds access. The output buffer (temp) should be
	 * unchanged (90). */
	EXPECT(range_get(&range, &temp, i) == false);
	EXPECT(90 == temp);
}


TEST(test_range_put_many)
{
	const int lots[10] = { 0, 100, 200, 300, 400, 500, 600, 700, 800, 900 };

	EXPECT(range_put_many(&range, lots, 0, sizeof(lots) / sizeof(lots[0])) == true);
	EXPECT(range_put_many(&range, lots, 4, sizeof(lots) / sizeof(lots[0])) == false);
}


TEST(test_range_get_many)
{
	unsigned i;
	const int expected[10] = { 0, 100, 200, 300, 400, 500, 600, 700, 800, 900 };
	int buffer[10];

	/* Try to get all the data out of the range */
	EXPECT(range_get_many(&range, buffer, 0, range_count(&range)) == true);

	for(i = 0; i < range_count(&range); i++)
	{
		if(expected[i] != buffer[i])
		{
			TEST_FAIL("Arrays not equal");
		}
	}

	TEST_PASS("Arrays are equal");

	/* Try to get range_count data at a higher index */
	EXPECT(range_get_many(&range, buffer, 2, range_count(&range)) == false);

	for(i = 0; i < range_count(&range); i++)
	{
		if(expected[i] != buffer[i])
		{
			TEST_FAIL("Arrays not equal");
		}
	}

	/* Try to get starting at an out of bounds index */
	EXPECT(range_get_many(&range, buffer, range_count(&range) + 1, range_count(&range)) == false);

	for(i = 0; i < range_count(&range); i++)
	{
		if(expected[i] != buffer[i])
		{
			TEST_FAIL("Arrays not equal");
		}
	}

	/* Try to get more than range_count data */
	EXPECT(range_get_many(&range, buffer, 0, range_count(&range) + 2) == false);

	for(i = 0; i < range_count(&range); i++)
	{
		if(expected[i] != buffer[i])
		{
			TEST_FAIL("Arrays not equal");
		}
	}
}


TEST(test_range_swap)
{
	const int expected[10] = { 900, 800, 700, 600, 500, 400, 300, 200, 100, 0 };

	unsigned i, j;
	for(i = 0, j = range_count(&range)-1; i < j; i++, j--)
	{
		EXPECT(range_swap(&range, i, j) == true);
	}

	for(i = 0; i < range_count(&range); i++)
	{
		int temp;

		EXPECT(range_get(&range, &temp, i) == true);
		EXPECT(expected[i] == temp);
	}

	EXPECT(range_swap(&range, 0, range_count(&range)) == false);
	EXPECT(range_swap(&range, range_count(&range), 0) == false);
}


TEST(test_range_entry)
{
	const int expected[10] = { 900, 800, 700, 600, 500, 400, 300, 200, 100, 0 };

	int* ptr;

	unsigned i;
	for(i = 0; i < range_count(&range); i++)
	{
		ptr = range_entry(&range, i);

		EXPECT(ptr != 0);
		EXPECT(expected[i] == *ptr);
	}

	EXPECT(range_entry(&range, range_count(&range)) == 0);
}


TEST(test_range_indexof)
{
	unsigned i;
	for(i = 0; i < range_count(&range); i++)
	{
		unsigned idx = range_indexof(&range, &data[i+1]);

		EXPECT(i == idx);
	}

	EXPECT((unsigned)-1 == range_indexof(&range, &data[12]));
	EXPECT((unsigned)-1 == range_indexof(&range, &data[0]));
}


TEST(test_range_zero_init)
{
	int temp = 20;

	memset(&range, 0, sizeof(range));

	EXPECT(range_start(&range) == 0);
	EXPECT(range_end(&range) == 0);
	EXPECT(range_count(&range) == 0);
	EXPECT(range_empty(&range));
	EXPECT(range_elemsize(&range) == 0);
	EXPECT(range_entry(&range, 0) == 0);
	EXPECT(range_entry(&range, 1) == 0);
	EXPECT(range_entry(&range, 10) == 0);
	EXPECT(range_offset(&range, 0) == 0);
	EXPECT(range_offset(&range, 1) == 0);
	EXPECT(range_offset(&range, 10) == 0);
	EXPECT(range_at(&range, 0) == 0);
	EXPECT(range_at(&range, 1) == 0);
	EXPECT(range_at(&range, 10) == 0);
	EXPECT(!range_put(&range, &temp, 0));
	EXPECT(!range_put(&range, &temp, 1));
	EXPECT(!range_put(&range, &temp, 10));
	EXPECT(!range_get(&range, &temp, 0));
	EXPECT(!range_get(&range, &temp, 1));
	EXPECT(!range_get(&range, &temp, 10));
	EXPECT(!range_swap(&range, 0, 1));
	EXPECT(!range_swap(&range, 0, 10));
}


void test_range(void)
{
	memset(data, 0, sizeof(data));

	/* Point the range to the second (idx 1) element of data. This allows test_range_indexof to test
	 * computing an index for a below range pointer. */
	range = make_range(&data[1], 10, sizeof(data[0]));

	tharness_run(test_range_init);
	tharness_run(test_range_put);
	tharness_run(test_range_get);
	tharness_run(test_range_put_many);
	tharness_run(test_range_get_many);
	tharness_run(test_range_swap);
	tharness_run(test_range_entry);
	tharness_run(test_range_indexof);
	tharness_run(test_range_zero_init);

	// @TODO test operations on range_slice
}


/******************************************* END OF FILE *******************************************/
