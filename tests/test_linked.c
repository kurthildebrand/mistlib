/************************************************************************************************//**
 * @file		test_linked.c
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
#include <stdlib.h>

#include "test_linked.h"

#include "linked.h"
#include "range.h"
#include "tharness.h"
#include "utils.h"


/* Private Types --------------------------------------------------------------------------------- */
typedef struct {
	Link link;
	int data;
	int data2;
} IntLink;


typedef struct {
	int a, b, c;
	Link forward;
	Link backward;
} Int3Link;


/* Private Functions ----------------------------------------------------------------------------- */
static int  compare_links(const void*, const void*);
static void show_linked(Link**);


/* Private Variables ----------------------------------------------------------------------------- */
//Linked  linked;
Link*   linked;
IntLink links[10];


TEST(test_linked_empty)
{
	EXPECT(linked_count(linked) == 0);
	show_linked(&linked);
}


TEST(test_linked_append_one)
{
	linked_init(&linked);

	links[0].data = 101;

	linked_append(&linked, &links[0].link);

	IntLink* ptr = linked_first(linked);

	EXPECT(ptr);
	EXPECT(ptr->data == 101);

	ptr = linked_next(linked, &ptr->link);

	EXPECT(!ptr);

	EXPECT(linked_count(linked) == 1);
}


TEST(test_linked_append)
{
	linked_init(&linked);

	unsigned i;
	unsigned count = 0;
	for(i = 0; i < sizeof(links) / sizeof(links[0]); i++)
	{
		links[i].data = i * 10;
		linked_append(&linked, &links[i].link);
	}

	EXPECT(linked_count(linked) == sizeof(links) / sizeof(links[0]));

	IntLink* ptr = linked_first(linked);

	while(ptr)
	{
		EXPECT(ptr->data == (int)(count * 10));
		PRINT("%d\t%d\n", count, ptr->data);
		count++;
		ptr = linked_next(linked, &ptr->link);
	}
}


TEST(test_linked_prepend_one)
{
	linked_init(&linked);

	links[0].data = 202;

	linked_prepend(&linked, &links[0].link);

	IntLink* ptr = linked_first(linked);

	EXPECT(ptr);
	EXPECT(ptr->data == 202);

	ptr = linked_next(linked, &ptr->link);

	EXPECT(!ptr);

	EXPECT(linked_count(linked) == 1);
}


TEST(test_linked_prepend)
{
	linked_init(&linked);

	unsigned i;
	unsigned count = 0;
	for(i = 0; i < sizeof(links) / sizeof(links[0]); i++)
	{
		links[i].data = i * 10;
		linked_prepend(&linked, &links[i].link);
	}

	EXPECT(linked_count(linked) == sizeof(links) / sizeof(links[0]));

	IntLink* ptr = linked_last(linked);

	while(ptr)
	{
		EXPECT(ptr->data == (int)(count * 10));
		PRINT("%d\t%d\n", count, ptr->data);
		count++;
		ptr = linked_prev(linked, &ptr->link);
	}
}


TEST(test_linked_pop_front)
{
	linked_init(&linked);

	unsigned i;
	int count = 0;
	for(i = 0; i < sizeof(links) / sizeof(links[0]); i++)
	{
		links[i].data = i * 10;
		linked_append(&linked, &links[i].link);
	}

	IntLink* ptr = linked_pop_front(&linked);

	while(ptr)
	{
		EXPECT(ptr->data == count * 10);
		PRINT("%d\t%d\n", count, ptr->data);
		count++;
		ptr = linked_pop_front(&linked);
	}
}


TEST(test_linked_pop_back)
{
	linked_init(&linked);

	unsigned i;
	int count = 0;
	for(i = 0; i < sizeof(links) / sizeof(links[0]); i++)
	{
		links[i].data = i * 10;
		linked_prepend(&linked, &links[i].link);
	}

	IntLink* ptr = linked_pop_back(&linked);

	while(ptr)
	{
		EXPECT(ptr->data == count * 10);
		PRINT("%d\t%d\n", count, ptr->data);
		count++;
		ptr = linked_pop_back(&linked);
	}
}


TEST(test_linked_compare_insert)
{
	unsigned i;

	IntLink temps[] = {
		{ .data = 0,  .data2 = 0 },
		{ .data = 10, .data2 = 1 },
		{ .data = 10, .data2 = 2 },
		{ .data = 30, .data2 = 3 },
		{ .data = 40, .data2 = 4 },
		{ .data = 50, .data2 = 5 },
		{ .data = 60, .data2 = 6 },
		{ .data = 70, .data2 = 7 },
		{ .data = 80, .data2 = 8 },
		{ .data = 90, .data2 = 9 },
	};

	Range range = make_range(temps, sizeof(temps) / sizeof(temps[0]), sizeof(temps[0]));

	/* Shuffle range */
	for(i = range_start(&range); i < range_end(&range); i++)
	{
		range_swap(&range, i, rand() % (i+1));
	}

	for(i = 0; i < sizeof(temps) / sizeof(temps[0]); i++)
	{
		linked_compare_insert(&linked, &temps[i].link, compare_links);

		show_linked(&linked);
	}

	IntLink* p1 = linked_first(linked);
	IntLink* p2 = linked_next(linked, &p1->link);

	while(p2)
	{
		EXPECT(p1->data <= p2->data);
		PRINT("%d -> %d\n", p1->data, p2->data);
		p1 = linked_next(linked, &p1->link);
		p2 = linked_next(linked, &p2->link);
	}
}


void test_linked_container(void)
{
	Int3Link temps[] = {
		{ .a = 0,  .b = 0,   .c = 0    },
		{ .a = 10, .b = 100, .c = 1000 },
		{ .a = 20, .b = 200, .c = 2000 },
		{ .a = 30, .b = 300, .c = 3000 },
		{ .a = 40, .b = 400, .c = 4000 },
		{ .a = 50, .b = 500, .c = 5000 },
		{ .a = 60, .b = 600, .c = 6000 },
		{ .a = 70, .b = 700, .c = 7000 },
	};

	unsigned i, j;

	linked_init(&linked);

	for(i = 0; i < sizeof(temps) / sizeof(temps[0]); i++)
	{
		linked_append(&linked, &temps[i].forward);
	}

	Int3Link* ptr;
	Int3Link* next;
	j = 0;

	LINKED_FOREACH_CONTAINER(linked, ptr, next, forward)
	{
		EXPECT(ptr->a == (int)j * 10,   "ptr->a = %d, j * 10   = %d", ptr->a, j*10);
		EXPECT(ptr->b == (int)j * 100,  "ptr->b = %d, j * 100  = %d", ptr->b, j*100);
		EXPECT(ptr->c == (int)j * 1000, "ptr->c = %d, j * 1000 = %d", ptr->c, j*1000);
		PRINT_LINE("%3d %4d %5d", ptr->a, ptr->b, ptr->c);
		j++;
	}
}


void test_linked(void)
{
	linked_init(&linked);
	tharness_run(test_linked_empty);
	tharness_run(test_linked_append_one);
	tharness_run(test_linked_append);
	tharness_run(test_linked_prepend_one);
	tharness_run(test_linked_prepend);
	tharness_run(test_linked_pop_front);
	tharness_run(test_linked_pop_back);
	tharness_run(test_linked_compare_insert);
	tharness_run(test_linked_container);
}


static int compare_links(const void* a, const void* b)
{
	const IntLink* link_a = a;
	const IntLink* link_b = b;
	return compare_int(&link_a->data, &link_b->data);
}


static void show_linked(Link** head)
{
	// unsigned count = 0;

	// IntLink* ptr = linked_first(l);

	// while(ptr)
	// {
	// 	PRINT("%d\t%d\n", count, ptr->data);
	// 	count++;
	// 	ptr = linked_next(l, &ptr->link);
	// }

	unsigned count = 0;

	Link* ptr;
	Link* next;

	LINKED_FOREACH(*head, ptr, next)
	{
		// IntLink* link = (IntLink*)ptr;
		IntLink* link = CONTAINER_OF(ptr, IntLink, link);

		if(link)
		{
			PRINT("%d\t%d\t%d\n", count, link->data, link->data2);
		}
		count++;
	}

	PRINT("\n");
}


/******************************************* END OF FILE *******************************************/
