/************************************************************************************************//**
 * @file		test_search.c
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

#include "search.h"
#include "tharness.h"


/* Private Variables ----------------------------------------------------------------------------- */
static int data[] = { 3, 4, 4, 4, 4, 5, 7, 7, 7, 7, 8 };
static Range range;


TEST(test_lower_range)
{
	int search;

	search = 2;
	EXPECT(0 == lower_range(&range, &search, compare_int));

	search = 4;
	EXPECT(1 == lower_range(&range, &search, compare_int));

	search = 6;
	EXPECT(6 == lower_range(&range, &search, compare_int));

	search = 8;
	EXPECT(10 == lower_range(&range, &search, compare_int));

	search = 9;
	EXPECT(11 == lower_range(&range, &search, compare_int));
}


TEST(test_upper_range)
{
	int search;

	search = 2;
	EXPECT(0 == upper_range(&range, &search, compare_int));

	search = 4;
	EXPECT(5 == upper_range(&range, &search, compare_int));

	search = 6;
	EXPECT(6 == upper_range(&range, &search, compare_int));

	search = 8;
	EXPECT(11 == upper_range(&range, &search, compare_int));

	search = 9;
	EXPECT(11 == upper_range(&range, &search, compare_int));
}


TEST(test_equal_range)
{
	int value = 4;

	Range equal = equal_range(&range, &value, compare_int);

	EXPECT(1 == range_start(&equal));
	EXPECT(5 == range_end(&equal));
	EXPECT(4 == range_count(&equal));
	EXPECT(&data[1] == range_offset(&equal, 0));
}


void test_search(void)
{
	range_init(&range, data, sizeof(data) / sizeof(data[0]), sizeof(data[0]));

	tharness_run(test_lower_range);
	tharness_run(test_upper_range);
	tharness_run(test_equal_range);
}


/******************************************* END OF FILE *******************************************/
