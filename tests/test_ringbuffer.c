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

#include "ringbuffer.h"


/* Private Functions ----------------------------------------------------------------------------- */
static int data[8];
static RingBuffer ringbuffer;


TEST(test_ringbuffer_push)
{
	EXPECT(rb_init(&ringbuffer, data, sizeof(data) / sizeof(data[0]), sizeof(data[0])));

	int temp = 0;

	unsigned i;
	for(i = 0; i < rb_size(&ringbuffer); i++)
	{
		temp = i * 10;
		EXPECT(rb_push(&ringbuffer, &temp));
	}

	EXPECT(!rb_push(&ringbuffer, &temp));

	EXPECT(rb_full(&ringbuffer));
}


TEST(test_ringbuffer_pop)
{
	int temp;
	while(rb_peek(&ringbuffer, &temp))
	{
		EXPECT(rb_pop(&ringbuffer));
		PRINT("%d", temp);
	}

	EXPECT(!rb_pop(&ringbuffer));
	EXPECT(rb_empty(&ringbuffer));
}


TEST(test_ringbuffer_push_many)
{
	EXPECT(rb_init(&ringbuffer, data, sizeof(data) / sizeof(data[0]), sizeof(data[0])));

	int temp1[] = { 10, 20, 30, 40 };
	int temp2[] = { 50, 60 };
	int temp3[] = { 70, 80, 90, 100, 110, 120 };

	EXPECT(rb_push_many(&ringbuffer, temp1, sizeof(temp1) / sizeof(temp1[0])));

	EXPECT(rb_pop_many(&ringbuffer, sizeof(temp1) / sizeof(temp1[0])));

	EXPECT(rb_push_many(&ringbuffer, temp2, sizeof(temp2) / sizeof(temp2[0])));
	EXPECT(rb_push_many(&ringbuffer, temp3, sizeof(temp3) / sizeof(temp3[0])));

	unsigned i;
	for(i = 0; i < rb_count(&ringbuffer); i++)
	{
		int temp;
		EXPECT(rb_peek_at(&ringbuffer, &temp, i));
		EXPECT(temp == (int)((i + 5) * 10));
	}
}


TEST(test_ringbuffer_pop_many)
{
	unsigned count = rb_count(&ringbuffer);

	EXPECT(rb_pop_many(&ringbuffer, count));
	EXPECT(rb_empty(&ringbuffer));
	EXPECT(!rb_pop_many(&ringbuffer, 1));
}


void test_ringbuffer(void)
{
	tharness_run(test_ringbuffer_push);
	tharness_run(test_ringbuffer_pop);
	tharness_run(test_ringbuffer_push_many);
	tharness_run(test_ringbuffer_pop_many);
}


/******************************************* END OF FILE *******************************************/
