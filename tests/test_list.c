/************************************************************************************************//**
 * @file		test_list.c
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

#include "list.h"
#include "test_list.h"
#include "tharness.h"


/* Private Variables ----------------------------------------------------------------------------- */
static int data[9];
static List list;


TEST(test_list_append)
{
	int temp;
	unsigned i;
	for(i = 0; i < list_size(&list); i++)
	{
		temp = i * 10;

		EXPECT(list_append(&list, &temp) == true);
	}

	EXPECT(list_full(&list) == true);
	EXPECT_MESSAGE(list_append(&list, &temp) == false, "list_append() did not prevent out-of-bounds access.\n");
}


TEST(test_list_get)
{
	int temp;
	unsigned i;
	for(i = 0; i < list_count(&list); i++)
	{
		EXPECT(list_get(&list, &temp, i) == true);
		EXPECT(i * 10 == (unsigned)temp);
	}

	EXPECT(list_get(&list, &temp, list_size(&list)) == false);
}


TEST(test_list_entry)
{
	unsigned i;
	for(i = 0; i < list_count(&list); i++)
	{
		int* ptr = list_entry(&list, i);

		EXPECT(ptr != 0);
		EXPECT(&data[i] == ptr);
	}

	EXPECT(list_entry(&list, list_size(&list)) == 0);
}


TEST(test_list_pop)
{
	unsigned count = list_count(&list);

	EXPECT(list_pop_many(&list, 3) == true);
	EXPECT(list_count(&list) == count - 3);

	count = list_count(&list);

	unsigned i;
	for(i = 0; i < count; i++)
	{
		EXPECT(list_pop(&list) == true);
	}

	EXPECT(list_empty(&list) == true);
	EXPECT(list_pop(&list) == false);
}


TEST(test_list_prepend)
{
	int temp;
	unsigned i;
	for(i = 0; i < list_size(&list); i++)
	{
		temp = i * 10;

		EXPECT(list_prepend(&list, &temp) == true);
	}

	EXPECT(list_full(&list) == true);
	EXPECT(list_prepend(&list, &temp) == false);
}


TEST(test_list_replace)
{
	int temp;
	unsigned i;
	for(i = 0; i < list_count(&list); i++)
	{
		temp = i * 100;

		EXPECT(list_replace(&list, &temp, i) == true);
	}

	EXPECT(list_replace(&list, &temp, list_size(&list)) == false);
}


TEST(test_list_swap)
{
	unsigned i, j;
	for(i = 0, j = list_count(&list) - 1; i < j; i++, j--)
	{
		EXPECT(list_swap(&list, i, j) == true);
	}

	for(i = 0; i < list_count(&list); i++)
	{
		int temp;

		EXPECT(list_get(&list, &temp, i) == true);
		EXPECT((list_count(&list) - i - 1) * 100 == (unsigned)temp);
	}
}


TEST(test_list_remove)
{
	unsigned count = list_count(&list);

	unsigned i;
	for(i = 0; i < count; i++)
	{
		EXPECT(list_remove(&list, 0) == true);
	}

	EXPECT(list_empty(&list) == true);
	EXPECT(list_remove(&list, 0) == false);
}


TEST(test_list_insert_many)
{
	int data1[] = { 10, 20, 30 };
	int data2[] = { 40, 50, 60 };
	int data3[] = { 70, 80, 90, 100 };

	list_clear(&list);

	EXPECT(list_insert_many(&list, data1, 0, 3) == true);
	EXPECT(list_insert_many(&list, data2, 1, 3) == true);
	EXPECT(list_insert_many(&list, data3, 2, 4) == false);
}


TEST(test_list_append_many)
{
	int data1[] = { 10, 20, 30 };
	int data2[] = { 40, 50, 60 };
	int data3[] = { 70, 80, 90, 100 };

	list_clear(&list);

	EXPECT(list_append_many(&list, data1, 3) == true);
	EXPECT(list_append_many(&list, data2, 3) == true);
	EXPECT(list_append_many(&list, data3, 4) == false);
}


TEST(test_list_prepend_many)
{
	int data1[] = { 10, 20, 30 };
	int data2[] = { 40, 50, 60 };
	int data3[] = { 70, 80, 90, 100 };

	list_clear(&list);

	EXPECT(list_prepend_many(&list, data2, 3) == true);
	EXPECT(list_prepend_many(&list, data1, 3) == true);
	EXPECT(list_prepend_many(&list, data3, 4) == false);
}


TEST(test_list_get_many)
{
	unsigned i;
	int fill[] = { 10, 20, 30, 40, 50, 60, 70, 80 };
	int expected1[] = { 10, 20, 30 };
	int expected2[] = { 20, 30, 40, 50 };
	int buf[4];

	list_clear(&list);
	list_insert_many(&list, fill, 0, sizeof(fill) / sizeof(fill[0]));

	EXPECT(list_get_many(&list, buf, 0, 3) == true);

	for(i = 0; i < 3; i++)
	{
		if(buf[i] != fill[i])
		{
			TEST_FAIL("Arrays not equal");
		}
	}

	TEST_PASS("Arrays equal");

	for(i = 0; i < 3; i++)
	{
		if(buf[i] != expected1[i])
		{
			TEST_FAIL("Arrays not equal");
		}
	}

	TEST_PASS("Arrays equal");

	EXPECT(list_get_many(&list, buf, 1, 4) == true);

	for(i = 0; i < 4; i++)
	{
		if(buf[i] != expected2[i])
		{
			TEST_FAIL("Arrays not equal");
		}
	}

	TEST_PASS("Arrays equal");

	EXPECT(list_get_many(&list, buf, 9, 1) == false);
	EXPECT(list_get_many(&list, buf, 0, 10) == false);
}


TEST(test_list_replace_many)
{
	unsigned i;
	int fill1[] = { 10, 20, 30, 40, 50 };
	int fill2[] = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 };
	int expected[] = { 100, 200, 300, 400, 500 };

	list_clear(&list);
	list_insert_many(&list, fill1, 0, sizeof(fill1) / sizeof(fill1[0]));

	EXPECT(list_replace_many(&list, fill2, 0, 5) == true);

	for(i = 0; i < 5; i++)
	{
		if(expected[i] != fill2[i])
		{
			TEST_FAIL("Arrays not equal");
		}
	}

	TEST_PASS("Arrays are equal");

	EXPECT(list_replace_many(&list, fill2, 0, 10) == false);
}


TEST(test_list_remove_many)
{
	unsigned i;
	int fill[] = { 100, 200, 300, 400, 500, 600, 700, 800, 900 };
	int expected[] = { 100, 200, 600, 700, 800, 900 };

	list_clear(&list);
	list_append_many(&list, fill, 9);

	EXPECT(list_remove_many(&list, 2, 3) == true);

	for(i = 0; i < 6; i++)
	{
		if(expected[i] != data[i])
		{
			TEST_FAIL("Arrays not equal");
		}
	}

	TEST_PASS("Arrays are equal");

	EXPECT(list_remove_many(&list, 9, 1) == false);
	EXPECT(list_remove_many(&list, 0, 10) == false);
}


void test_list(void)
{
	list_init(&list, data, 0, sizeof(data) / sizeof(data[0]), sizeof(data[0]));

	tharness_run(test_list_append);
	tharness_run(test_list_get);
	tharness_run(test_list_entry);
	tharness_run(test_list_pop);
	tharness_run(test_list_prepend);
	tharness_run(test_list_replace);
	tharness_run(test_list_swap);
	tharness_run(test_list_remove);

	tharness_run(test_list_insert_many);
	tharness_run(test_list_append_many);
	tharness_run(test_list_prepend_many);
	tharness_run(test_list_get_many);
	tharness_run(test_list_replace_many);
	tharness_run(test_list_remove_many);
}


/******************************************* END OF FILE *******************************************/
