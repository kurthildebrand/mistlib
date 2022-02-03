/************************************************************************************************//**
 * @file		test_order.c
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

#include "test_order.h"

#include "order.h"
#include "tharness.h"
#include "range.h"


/* Private Functions ----------------------------------------------------------------------------- */


/* Private Variables ----------------------------------------------------------------------------- */
static int data[20];
static Range range;


TEST(test_order_ascending)
{
	EXPECT(ascending(&range, compare_int) == true);
	EXPECT(descending(&range, compare_int) == false);
}


TEST(test_order_reverse)
{
	const int expected[20] = { 19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0 };

	reverse(&range);

	unsigned i;
	for(i = 0; i < sizeof(expected) / sizeof(expected[0]); i++)
	{
		if(data[i] != expected[i])
		{
			TEST_FAIL("Arrays not equal");
		}
	}

	TEST_PASS();
}


TEST(test_order_descending)
{
	EXPECT(descending(&range, compare_int) == true);
	EXPECT(ascending(&range, compare_int) == false);
}


TEST(test_order_equal)
{
	unsigned i;

	int data1[10];
	int data2[10];

	Range r1, r2;
	r1 = make_range(data1, sizeof(data1) / sizeof(data1[0]), sizeof(data1[0]));
	r2 = make_range(data2, sizeof(data2) / sizeof(data2[0]), sizeof(data2[0]));

	for(i = 0; i < range_count(&r1); i++)
	{
		data1[i] = i * 10;
		data2[i] = i * 10;
	}

	EXPECT(equivalent(&r1, &r2, compare_int) == true);
}


TEST(test_order_not_equal)
{
	unsigned i;

	int data1[10];
	int data2[10];

	Range r1, r2;
	r1 = make_range(data1, sizeof(data1) / sizeof(data1[0]), sizeof(data1[0]));
	r2 = make_range(data2, sizeof(data2) / sizeof(data2[0]), sizeof(data2[0]));

	for(i = 0; i < range_count(&r1); i++)
	{
		data1[i] = i * 10;
		data2[i] = i;
	}

	EXPECT(equivalent(&r1, &r2, compare_int) == false);
}


TEST(test_order_equal_pointing_to_same)
{
	unsigned i;

	int data1[10];

	Range r1, r2;
	r1 = make_range(data1, sizeof(data1) / sizeof(data1[0]), sizeof(data1[0]));
	r2 = make_range(data1, sizeof(data1) / sizeof(data1[0]), sizeof(data1[0]));

	for(i = 0; i < range_count(&r1); i++)
	{
		data1[i] = i * 10;
	}

	EXPECT(equivalent(&r1, &r2, compare_int) == true);
}


TEST(test_order_equal_same)
{
	unsigned i;

	int data1[10];

	Range r1;
	r1 = make_range(data1, sizeof(data1) / sizeof(data1[0]), sizeof(data1[0]));

	for(i = 0; i < range_count(&r1); i++)
	{
		data1[i] = i * 10;
	}

	EXPECT(equivalent(&r1, &r1, compare_int) == true);
}


TEST(test_order_equal_different)
{
	unsigned i;

	int data1[10];
	int data2[10];

	Range r1, r2;
	r1 = make_range(data1, sizeof(data1) / sizeof(data1[0]), sizeof(data1[0]));
	r2 = make_range(data2, sizeof(data2) / sizeof(data2[0]) - 4, sizeof(data1[0]));

	for(i = 0; i < range_count(&r1); i++)
	{
		data1[i] = i;
	}

	for(i = 0; i < range_count(&r2); i++)
	{
		data2[i] = i;
	}

	EXPECT(equivalent(&r1, &r2, compare_int) == false);
}


TEST(test_rotate_left_mid)
{
	int before[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	int after[]  = {3, 4, 5, 6, 7, 8, 0, 1, 2};

	Range r = make_range(before, sizeof(before) / sizeof(before[0]), sizeof(before[0]));

	unsigned n = rotate_left(&r, 3);

	unsigned i;
	for(i = 0; i < range_count(&r); i++)
	{
		if(before[i] != after[i])
		{
			TEST_FAIL("Arrays are not equal");
		}
	}

	TEST_PASS("Arrays are equal");

	EXPECT(6 == n);
}


TEST(test_rotate_left_start)
{
	int before[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	int after[]  = {0, 1, 2, 3, 4, 5, 6, 7, 8};

	Range r = make_range(before, sizeof(before) / sizeof(before[0]), sizeof(before[0]));

	unsigned n = rotate_left(&r, 0);

	unsigned i;
	for(i = 0; i < range_count(&r); i++)
	{
		if(before[i] != after[i])
		{
			TEST_FAIL("Arrays are not equal");
		}
	}

	TEST_PASS("Arrays are equal");

	EXPECT(0 == n);
}


TEST(test_rotate_left_end)
{
	int before[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	int after[]  = {8, 0, 1, 2, 3, 4, 5, 6, 7};

	Range r = make_range(before, sizeof(before) / sizeof(before[0]), sizeof(before[0]));

	unsigned n = rotate_left(&r, 8);

	unsigned i;
	for(i = 0; i < range_count(&r); i++)
	{
		if(before[i] != after[i])
		{
			TEST_FAIL("Arrays are not equal");
		}
	}

	TEST_PASS("Arrays are equal");

	EXPECT(1 == n);
}


TEST(test_rotate_left_invalid)
{
	int before[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	int after[]  = {0, 1, 2, 3, 4, 5, 6, 7, 8};

	Range r = make_range(before, sizeof(before) / sizeof(before[0]), sizeof(before[0]));

	unsigned n = rotate_left(&r, 9);

	unsigned i;
	for(i = 0; i < range_count(&r); i++)
	{
		if(before[i] != after[i])
		{
			TEST_FAIL("Arrays are not equal");
		}
	}

	TEST_PASS("Arrays are equal");

	EXPECT(0 == n);
}


TEST(test_rotate_right_mid)
{
	int before[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	int after[]  = {4, 5, 6, 7, 8, 0, 1, 2, 3};

	Range r = make_range(before, sizeof(before) / sizeof(before[0]), sizeof(before[0]));

	unsigned n = rotate_right(&r, 3);

	unsigned i;
	for(i = 0; i < range_count(&r); i++)
	{
		if(before[i] != after[i])
		{
			TEST_FAIL("Arrays are not equal");
		}
	}

	TEST_PASS("Arrays are equal");

	EXPECT(5 == n);
}


TEST(test_rotate_right_start)
{
	int before[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	int after[]  = {1, 2, 3, 4, 5, 6, 7, 8, 0};

	Range r = make_range(before, sizeof(before) / sizeof(before[0]), sizeof(before[0]));

	unsigned n = rotate_right(&r, 0);

	unsigned i;
	for(i = 0; i < range_count(&r); i++)
	{
		if(before[i] != after[i])
		{
			TEST_FAIL("Arrays are not equal");
		}
	}

	TEST_PASS("Arrays are equal");

	EXPECT(8 == n);
}


TEST(test_rotate_right_end)
{
	int before[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	int after[]  = {0, 1, 2, 3, 4, 5, 6, 7, 8};

	Range r = make_range(before, sizeof(before) / sizeof(before[0]), sizeof(before[0]));

	unsigned n = rotate_right(&r, 8);

	unsigned i;
	for(i = 0; i < range_count(&r); i++)
	{
		if(before[i] != after[i])
		{
			TEST_FAIL("Arrays are not equal");
		}
	}

	TEST_PASS("Arrays are equal");

	EXPECT(0 == n);
}


TEST(test_rotate_right_invalid)
{
	int before[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	int after[]  = {0, 1, 2, 3, 4, 5, 6, 7, 8};

	Range r = make_range(before, sizeof(before) / sizeof(before[0]), sizeof(before[0]));

	unsigned n = rotate_right(&r, 9);

	unsigned i;
	for(i = 0; i < range_count(&r); i++)
	{
		if(before[i] != after[i])
		{
			TEST_FAIL("Arrays are not equal");
		}
	}

	TEST_PASS("Arrays are equal");

	EXPECT(0 == n);
}


void test_order(void)
{
	unsigned i;
	for(i = 0; i < sizeof(data) / sizeof(data[0]); i++)
	{
		data[i] = i;
	}

	range = make_range(data, sizeof(data) / sizeof(data[0]), sizeof(data[0]));

	tharness_run(test_order_ascending);
	tharness_run(test_order_reverse);
	tharness_run(test_order_descending);
	tharness_run(test_order_equal);
	tharness_run(test_order_not_equal);
	tharness_run(test_order_equal_pointing_to_same);
	tharness_run(test_order_equal_same);
	tharness_run(test_order_equal_different);
	tharness_run(test_rotate_left_mid);
	tharness_run(test_rotate_left_start);
	tharness_run(test_rotate_left_end);
	tharness_run(test_rotate_left_invalid);
	tharness_run(test_rotate_right_mid);
	tharness_run(test_rotate_right_start);
	tharness_run(test_rotate_right_end);
	tharness_run(test_rotate_right_invalid);
}


/******************************************* END OF FILE *******************************************/
