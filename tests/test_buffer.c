/************************************************************************************************//**
 * @file		test_byteorder.c
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

#include "test_buffer.h"
#include "tharness.h"
#include "buffer.h"


/* Private Variables ----------------------------------------------------------------------------- */
Buffer buffer;
uint8_t buffer_data[40];


TEST(test_buffer_init)
{
	buffer_init(&buffer, buffer_data, 0, sizeof(buffer_data));

	EXPECT(!buffer_set_length(&buffer, 50));

	EXPECT(buffer_read    (&buffer) == buffer_data);
	EXPECT(buffer_write   (&buffer) == buffer_data);
	EXPECT(buffer_size    (&buffer) == sizeof(buffer_data));
	EXPECT(buffer_length  (&buffer) == 0);
	EXPECT(buffer_free    (&buffer) == sizeof(buffer_data));
	EXPECT(buffer_tailroom(&buffer) == sizeof(buffer_data));
}


TEST(test_buffer_write)
{
	uint8_t temp1[] = { 0x01, 0x23, 0x45, };
	uint8_t temp2[] = { 0x67, 0x89, 0xAB, 0xCD, 0xEF };

	/* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 ... 40
	 * 01 23 45
	 * ^        ^
	 * |        |
	 * read     write
	 */
	EXPECT(buffer_push_mem(&buffer, temp1, sizeof(temp1)));
	EXPECT(buffer_length(&buffer) == sizeof(temp1));

	/* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 ... 40
	 * 67 89 AB CD EF
	 * ^              ^
	 * |              |
	 * read           write
	 */
	EXPECT(buffer_replace_at(&buffer, temp2, buffer_offset(&buffer, 0), sizeof(temp2)));
	EXPECT(buffer_length(&buffer) == sizeof(temp2));

	/* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 ... 40
	 * 67 89 AB CD EF 78 56 34 12
	 * ^                          ^
	 * |                          |
	 * read                       write
	 */
	EXPECT(buffer_push_u32(&buffer, le_u32(0x12345678)));

	/* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 ... 40
	 * 67 89 AB CD EF 78 56 34 12 12 34
	 * ^                                ^
	 * |                                |
	 * read                             write
	 */
	EXPECT(buffer_push_u16(&buffer, be_u16(0x1234)));

	/* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 ... 40
	 * 01       67 89 AB CD EF 78 56 34 12 12 34
	 * ^                                         ^
	 * |                                         |
	 * read                                      write
	 */
	void* ptr;
	ptr = buffer_reserve_at(&buffer, buffer_offset(&buffer, 0), 1);
	EXPECT(ptr != 0);
	EXPECT(le_set_u8(ptr, 0x01));

	/* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 ... 40
	 * 01 23 45 67 89 AB CD EF 78 56 34 12 12 34
	 * ^                                         ^
	 * |                                         |
	 * read                                      write
	 */
	ptr = buffer_reserve_at(&buffer, buffer_offset(&buffer, 1), 2);
	EXPECT(ptr != 0);
	EXPECT(be_set_u16(ptr, 0x2345));
}


TEST(test_buffer_peek)
{
	// uint8_t expected[] = { 0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x78,0x56,0x34,0x12,0x12,0x34 };
	uint8_t expected[] = { 0x78,0x56,0x34,0x12 };

	/* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 ... 40
	 * 01 23 45 67 89 AB CD EF 78 56 34 12 12 34
	 * ^                                         ^
	 * |                                         |
	 * read                                      write
	 */
	void* ptr;
	ptr = buffer_peek_at(&buffer, buffer_offset(&buffer, 8), 4);
	EXPECT(memcmp(expected, ptr, 4) == 0);

	ptr = buffer_peek_at(&buffer, buffer_offset(&buffer, 8), 8);
	EXPECT(!ptr);

	EXPECT(be_get_u8 (buffer_peek_u8 (&buffer)) == 0x01);
	EXPECT(be_get_u16(buffer_peek_u16(&buffer)) == 0x0123);
	EXPECT(be_get_u32(buffer_peek_u32(&buffer)) == 0x01234567);
	EXPECT(be_get_u64(buffer_peek_u64(&buffer)) == 0x0123456789ABCDEF);
}


TEST(test_buffer_pop)
{
	uint8_t expected0[] = { 0x01, 0x23, 0x45 };
	uint8_t expected1[] = { 0x67, 0x89, 0xAB, 0xCD, 0xEF };
	uint8_t buf[5];

	/* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 ... 40
	 * 01 23 45 67 89 AB CD EF 78 56 34 12 12 34
	 * ^^^^^^^^ ^^^^^^^^^^^^^^
	 * read 0   read 1
	 * ^                                         ^
	 * |                                         |
	 * read                                      write
	 */
	EXPECT(buffer_read_at(&buffer, buf, buffer_offset(&buffer, 0), 3));
	EXPECT(memcmp(buf, expected0, 3) == 0);

	EXPECT(buffer_read_at(&buffer, buf, buffer_offset(&buffer, 3), 5));
	EXPECT(memcmp(buf, expected1, 5) == 0);

	/* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 ... 40
	 * 01 23 45 67 89 AB CD EF 78 56 34 12 12 34
	 * ^^ ^^^^^ ^^^^^^^^^^^^^^ ^^^^^^^^^^^ ^^^^^
	 * u8 |     buf[5]         le u32      be u16
	 *    be u16
	 * ^                                         ^
	 * |                                         |
	 * read                                      write
	 */
	uint8_t temp1  = be_get_u8(buffer_pop_u8(&buffer));
	EXPECT(temp1 == 0x01);

	uint16_t temp2 = be_get_u16(buffer_pop_u16(&buffer));
	EXPECT(temp2 == 0x2345);

	buffer_pop_mem(&buffer, buf, 5);
	EXPECT(memcmp(buf, expected1, 5) == 0);

	uint32_t temp3 = le_get_u32(buffer_pop_u32(&buffer));
	EXPECT(temp3 == 0x12345678);

	uint16_t temp4 = be_get_u16(buffer_pop_u16(&buffer));
	EXPECT(temp4 == 0x1234);

	EXPECT(!buffer_pop(&buffer, 1));
}


TEST(test_buffer_be)
{
	uint8_t expected[] = {
		0x01,
		0x01, 0x02,
		0x01, 0x02, 0x03, 0x04,
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, };

	buffer_init(&buffer, buffer_data, 0, sizeof(buffer_data));

	/* data                                                                    data + size
	 * |                                                                                 |
	 * v                                                                                 v
	 * 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 ... 40
	 * 01 01 02 01 02 03 04 01 02 03 04 05 06 07 08
	 * ^^ ^^^^^ ^^^^^^^^^^^ ^^^^^^^^^^^^^^^^^^^^^^^
	 * u8 u16   u32         u64
	 * ^                                            ^
	 * |                                            |
	 * read                                         write
	 */
	EXPECT(buffer_push_u8 (&buffer, 0x01));
	EXPECT(buffer_push_u16(&buffer, be_u16(0x0102)));
	EXPECT(buffer_push_u32(&buffer, be_u32(0x01020304)));
	EXPECT(buffer_push_u64(&buffer, be_u64(0x0102030405060708)));

	EXPECT(memcmp(buffer_peek_at(&buffer, buffer_offset(&buffer, 0), 0), expected, buffer_length(&buffer)) == 0);
	EXPECT(be_get_u8 (buffer_pop_u8 (&buffer)) == 0x01);
	EXPECT(be_get_u16(buffer_pop_u16(&buffer)) == 0x0102);
	EXPECT(be_get_u32(buffer_pop_u32(&buffer)) == 0x01020304);
	EXPECT(be_get_u64(buffer_pop_u64(&buffer)) == 0x0102030405060708);
}


TEST(test_buffer_le)
{
	uint8_t expected[] = {
		0x01,
		0x02, 0x01,
		0x04, 0x03, 0x02, 0x01,
		0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, };

	buffer_init(&buffer, buffer_data, 0, sizeof(buffer_data));

	/* data                                                                    data + size
	 * |                                                                                 |
	 * v                                                                                 v
	 * 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 ... 40
	 * 01 02 01 04 03 02 01 08 07 06 05 04 03 02 01
	 * ^^ ^^^^^ ^^^^^^^^^^^ ^^^^^^^^^^^^^^^^^^^^^^^
	 * u8 u16   u32         u64
	 * ^                                            ^
	 * |                                            |
	 * read                                         write
	 */
	EXPECT(buffer_push_u8 (&buffer, 0x01));
	EXPECT(buffer_push_u16(&buffer, le_u16(0x0102)));
	EXPECT(buffer_push_u32(&buffer, le_u32(0x01020304)));
	EXPECT(buffer_push_u64(&buffer, le_u64(0x0102030405060708)));

	EXPECT(memcmp(buffer_peek_at(&buffer, buffer_offset(&buffer, 0), 0), expected, buffer_length(&buffer)) == 0);
	EXPECT(le_get_u8 (buffer_pop_u8 (&buffer)) == 0x01);
	EXPECT(le_get_u16(buffer_pop_u16(&buffer)) == 0x0102);
	EXPECT(le_get_u32(buffer_pop_u32(&buffer)) == 0x01020304);
	EXPECT(le_get_u64(buffer_pop_u64(&buffer)) == 0x0102030405060708);
}


TEST(test_string_cmp)
{
	EXPECT(string_cmp(MAKE_STRING("Alpha"), MAKE_STRING("Bravo")) < 0);
	EXPECT(string_cmp(MAKE_STRING("Delta"), MAKE_STRING("Charlie")) > 0);
	EXPECT(string_cmp(MAKE_STRING("Foxtrot"), MAKE_STRING("Foxtrot")) == 0);
}


TEST(test_string_token)
{
	String expected[] = {
		MAKE_STRING("One"),
		MAKE_STRING("Two"),
		MAKE_STRING("Three"),
		MAKE_STRING("Four"),
		MAKE_STRING("Five"),
	};

	/*                     0         1         2
	 *                     01234567890123456789012 */
	const char string[] = "One,Two.Three/Four_Five";

	String it    = MAKE_STRING(string);
	String one   = string_token(&it, "_./,");
	String two   = string_token(&it, "_./,");
	String three = string_token(&it, "_./,");
	String four  = string_token(&it, "_./,");
	String five  = string_token(&it, "_./,");

	EXPECT(!string_empty(&one));
	EXPECT(!string_empty(&two));
	EXPECT(!string_empty(&three));
	EXPECT(!string_empty(&four));
	EXPECT(!string_empty(&five));

	EXPECT(string_equal(one,   expected[0])); PRINT_LINE("%s", string_offset(&one, 0));
	EXPECT(string_equal(two,   expected[1])); PRINT_LINE("%s", string_offset(&two, 0));
	EXPECT(string_equal(three, expected[2])); PRINT_LINE("%s", string_offset(&three, 0));
	EXPECT(string_equal(four,  expected[3])); PRINT_LINE("%s", string_offset(&four, 0));
	EXPECT(string_equal(five,  expected[4])); PRINT_LINE("%s", string_offset(&five, 0));

	String six = string_token(&it, "_./,");

	EXPECT(string_empty(&six));
}


TEST(test_string_search)
{
	unsigned i;

	String searches[] =
	{
		MAKE_STRING("The"),
		MAKE_STRING("quick"),
		MAKE_STRING("brown"),
		MAKE_STRING("fox"),
		MAKE_STRING("jumps"),
		MAKE_STRING("over"),
		MAKE_STRING("the"),
		MAKE_STRING("lazy"),
		MAKE_STRING("dog"),
		MAKE_STRING("cat"),
	};

	unsigned expected[] = { 0, 4, 10, 16, 20, 26, 31, 35, 40, -1 };

	/* Indices:             0         1         2         3         4
	 *                      01234567890123456789012345678901234567890123 */
	String s = MAKE_STRING("The quick brown fox jumps over the lazy dog.");

	for(i = 0; i < sizeof(searches) / sizeof(searches[0]); i++)
	{
		String search = string_search(s, searches[i]);

		unsigned idx = string_offsetof(&s, string_start(&search));

		EXPECT(idx == expected[i]);

		PRINT_LINE("    %7s%.*s", "", string_length(&s), string_offset(&s, 0));

		if(string_start(&search))
		{
			PRINT("    %-7.*s", string_length(&searches[i]), string_offset(&searches[i], 0));
			PRINT("%*s^ %d\n", idx, "", idx);
		}
		else
		{
			PRINT("    %-7.*s", string_length(&searches[i]), string_offset(&searches[i], 0));
			PRINT("%d Not found\n", idx);
		}
	}
}


void test_buffer(void)
{
	tharness_run(test_buffer_init);
	tharness_run(test_buffer_write);
	tharness_run(test_buffer_peek);
	tharness_run(test_buffer_pop);
	tharness_run(test_buffer_be);
	tharness_run(test_buffer_le);

	tharness_run(test_string_cmp);
	tharness_run(test_string_token);
	tharness_run(test_string_search);
}

/******************************************* END OF FILE *******************************************/
