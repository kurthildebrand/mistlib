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

#include "test_pool.h"

#include "pool.h"
#include "tharness.h"


/* Private Types --------------------------------------------------------------------------------- */
//typedef struct {
//	union {
//		Link link;
//		int  data;
//	};
//} Int_Pool;


/* Private Functions ----------------------------------------------------------------------------- */
static void show_pool(const Pool*);


/* Private Variables ----------------------------------------------------------------------------- */
//Int_Pool pool_data[10];
int  pool_data[10];
Pool pool;
int* ptrs[10];


TEST(test_pool_reserve)
{
	unsigned i;

//	/* Reserve the first 4 slots */
//	for(i = 0; i < 4; i++)
//	{
//		ptrs[i] = pool_reserve(&pool);
//
//		EXPECT(ptrs[i] != 0);
//
//		*ptrs[i] = (i + 1) * 10;
//
//		show_pool(&pool);
//	}
//
//	EXPECT(pool_release(&pool, ptrs[0]) == true);
//	EXPECT(pool_release(&pool, ptrs[1]) == true);
//
//	/* Reserve the rest of the pool */
//	for(; i < 12; i++)
//	{
//		ptrs[i-2] = pool_reserve(&pool);
//
//		EXPECT(ptrs[i] != 0);
//
//		*ptrs[i-2] = (i+1) * 10;
//
//		show_pool(&pool);
//	}

	/* Reserve the first 4 slots */
	for(i = 0; i < 10; i++)
	{
		ptrs[i] = pool_reserve(&pool);

		EXPECT(ptrs[i] != 0);

		*ptrs[i] = (i + 1) * 10;

		show_pool(&pool);
	}

	EXPECT(pool_full(&pool) == true);
	PRINT_LINE("Pool count = %d\n", pool_count(&pool));

	int* ptr = pool_reserve(&pool);

	EXPECT_MESSAGE(ptr == 0, "pool_reserve did not prevent out of memory allocation.");
}


TEST(test_pool_release)
{
	unsigned i;

	/* Release data in a random order */
	EXPECT(pool_release(&pool, ptrs[7]));	show_pool(&pool);
	EXPECT(pool_release(&pool, ptrs[9]));	show_pool(&pool);
	EXPECT(pool_release(&pool, ptrs[5]));	show_pool(&pool);
	EXPECT(pool_release(&pool, ptrs[2]));	show_pool(&pool);
	EXPECT(pool_release(&pool, ptrs[3]));	show_pool(&pool);
	EXPECT(pool_release(&pool, ptrs[6]));	show_pool(&pool);
	EXPECT(pool_release(&pool, ptrs[0]));	show_pool(&pool);
	EXPECT(pool_release(&pool, ptrs[4]));	show_pool(&pool);
	EXPECT(pool_release(&pool, ptrs[8]));	show_pool(&pool);
	EXPECT(pool_release(&pool, ptrs[1]));	show_pool(&pool);

	EXPECT(pool_empty(&pool));
	PRINT_LINE("Pool count = %d", pool_count(&pool));

	/* Reserve pointers */
	for(i = 0; i < pool_size(&pool); i++)
	{
		ptrs[i] = pool_reserve(&pool);

		EXPECT(ptrs[i] != 0);

		if(ptrs[i] != 0)
		{
			*ptrs[i] = (i+1) * 100;
			show_pool(&pool);
		}
	}

	EXPECT(pool_full(&pool) == true);
	PRINT_LINE("Pool count = %d\n", pool_count(&pool));
}


void test_pool(void)
{
	pool_init(&pool, pool_data, sizeof(pool_data) / sizeof(pool_data[0]), sizeof(pool_data[0]));

	tharness_run(test_pool_reserve);
	tharness_run(test_pool_release);
}


//static void show_pool(const Pool* p)
//{
//	unsigned i;
//
//	PRINT("   i next     prev     ptr      data\n");
//
//	for(i = 0; i < sizeof(pool_data) / sizeof(pool_data[0]); i++)
//	{
//		Int_Pool* ptr = pool_entry(p, i);
//
//		PRINT(" %3d %p %p %p %d\n", i, ptr->link.next, ptr->link.prev, ptr, ptr->data);
//	}
//
//	PRINT("\n");
//}


static void show_pool(const Pool* p)
{
	unsigned i;

	uint32_t freemask = p->freemask;

	for(i = 0; i < sizeof(pool_data) / sizeof(pool_data[0]); i++)
	{
		int* ptr = pool_entry(p, i);

		PRINT("%d: ", i);

		if(freemask & (1 << i))
		{
			PRINT("1");
		}
		else
		{
			PRINT("0");
		}

		PRINT(" %d\n", *ptr);
	}
}


/******************************************* END OF FILE *******************************************/
