/************************************************************************************************//**
 * @file		test_map.c
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

#include "list.h"
#include "map.h"


/* Private Functions ----------------------------------------------------------------------------- */
static int data1[] = { 1, 2, 3, 4, 5, 6, 7 };
static int data2[] = { 10, 20, 30, 40 };
static int data3[] = { 100, 200, 300, 400, 500, 600, 700, 800 };
static int data4[] = { 1000, 2000, 3000, 4000, 5000, 6000 };

static List sublist[3];
static Map map;


TEST(test_map_put)
{
	List list;

	list_init(&list, data2, 0, sizeof(data2) / sizeof(data2[0]), sizeof(data2[0]));
	key_set(&list, 3);
	EXPECT(map_put(&map, &list) == true);

	list_init(&list, data1, 0, sizeof(data1) / sizeof(data1[0]), sizeof(data1[0]));
	key_set(&list, 1);
	EXPECT(map_put(&map, &list) == true);

	list_init(&list, data3, 0, sizeof(data3) / sizeof(data3[0]), sizeof(data3[0]));
	key_set(&list, 5);
	EXPECT(map_put(&map, &list) == true);

// //	List* plist;
// //	plist = map_reserve(map, 3);
// //	plist->entries  = data2;
// //	plist->elemsize = sizeof(data2[0]);
// //	plist->count    = sizeof(data2) / sizeof(data2[0]);
// //	plist->size     = sizeof(data2) / sizeof(data2[0]);
// //
// //	plist = map_reserve(map, 1);
// //	plist->entries  = data1;
// //	plist->elemsize = sizeof(data1[0]);
// //	plist->count    = sizeof(data1) / sizeof(data1[0]);
// //	plist->size     = sizeof(data1) / sizeof(data1[0]);
// //
// //	plist = map_reserve(map, 5);
// //	plist->entries  = data3;
// //	plist->elemsize = sizeof(data3[0]);
// //	plist->count    = sizeof(data3) / sizeof(data3[0]);
// //	plist->size     = sizeof(data3) / sizeof(data3[0]);

	EXPECT(map_full(&map) == true);

	key_set(&list, 3);
	EXPECT(map_put(&map, &list) == false);
}


TEST(test_map_get)
{
	// List* list;
	Entry entry;
	Key   key;

	key = 1;
	EXPECT(map_find(&map, &key, compare_keys, &entry));

	key = 3;
	EXPECT(map_find(&map, &key, compare_keys, &entry));

	key = 5;
	EXPECT(map_find(&map, &key, compare_keys, &entry));

	key = 2;
	EXPECT(!map_find(&map, &key, compare_keys, &entry));

	key = 4;
	EXPECT(!map_find(&map, &key, compare_keys, &entry));

	key = 6;
	EXPECT(!map_find(&map, &key, compare_keys, &entry));
}


TEST(test_map_change_value)
{
	List replace;
	list_init(&replace, data4, 0, sizeof(data4) / sizeof(data4[0]), sizeof(data4[0]));
	key_set(&replace, 6);
	EXPECT(map_replace(&map, &replace) == false);

	key_set(&replace, 5);
	EXPECT(map_replace(&map, &replace) == true);
}


TEST(test_map_remove)
{
	Key key;
	Entry entry;

	key = 1;
	EXPECT(map_find(&map, &key, compare_keys, &entry));
	EXPECT(map_remove(&map, eidx(&entry)));

	key = 2;
	EXPECT(!map_find(&map, &key, compare_keys, &entry));

	key = 3;
	EXPECT(map_find(&map, &key, compare_keys, &entry));
	EXPECT(map_remove(&map, eidx(&entry)));

	key = 4;
	EXPECT(!map_find(&map, &key, compare_keys, &entry));

	key = 5;
	EXPECT(map_find(&map, &key, compare_keys, &entry));
	EXPECT(map_remove(&map, eidx(&entry)));

	key = 6;
	EXPECT(!map_find(&map, &key, compare_keys, &entry));

	EXPECT(map_empty(&map) == true);
}


TEST(test_map_put_duplicates)
{
	List list;

	map_clear(&map);

	EXPECT(map_empty(&map) == true);

	list_init(&list, data1, 0, sizeof(data1) / sizeof(data1[0]), sizeof(data1[0]));
	key_set(&list, 2);
	EXPECT(map_put(&map, &list) == true);

	EXPECT(map_put(&map, &list) == false);
}


void test_map(void)
{
	map_init(&map, sublist, 0, sizeof(sublist) / sizeof(sublist[0]), sizeof(sublist[0]), compare_keys);

	tharness_run(test_map_put);
	tharness_run(test_map_get);
	tharness_run(test_map_change_value);
	tharness_run(test_map_remove);
	tharness_run(test_map_put_duplicates);
}


/******************************************* END OF FILE *******************************************/
