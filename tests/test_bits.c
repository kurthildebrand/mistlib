/************************************************************************************************//**
 * @file		test_bits.c
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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "tharness.h"

#include "bits.h"
#include "test_bits.h"


/* Private Functions ----------------------------------------------------------------------------- */
static uint8_t bits_data[5];
static Bits bits;


TEST(test_bits_set_all)
{
	uint8_t expected[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0x07 };

	bits_set_all(&bits);

	EXPECT(memcmp(expected, bits_data, sizeof(bits_data)) == 0);
}


TEST(test_bits_clear_all)
{
	uint8_t expected[] = { 0x00, 0x00, 0x00, 0x00, 0xF8 };

	bits_data[4] = 0xFF;

	bits_clear_all(&bits);

	EXPECT(memcmp(expected, bits_data, sizeof(bits_data)) == 0);
}


TEST(test_bits_set)
{
	uint8_t expected[] = { 0xB1, 0x63, 0xFC, 0xFF, 0x07 };

	bits_data[4] = 0x0;

	/* 0          1          2          3
 	 * 01234567 89012345 67890123 45678901 23456789
	 * 10001101 11000110 00111111 11111111 111xxxxx */
	EXPECT( bits_set(&bits, 0));
	EXPECT( bits_set_many(&bits, 4, 2));
	EXPECT( bits_set_many(&bits, 7, 3));
	EXPECT( bits_set_many(&bits, 13, 2));
	EXPECT( bits_set_many(&bits, 18, 8));
	EXPECT(!bits_set_many(&bits, 25, 15));
	EXPECT( bits_set_many(&bits, 18, 17));
	EXPECT(memcmp(expected, bits_data, sizeof(bits_data)) == 0);
}


TEST(test_bits_clear)
{
	// uint8_t expected[] = { ~0xB1, ~0x63, ~0xFC, ~0xFF, ~0xFF };
	uint8_t expected[] = { 0x4E, 0x9C, 0x03, 0x00, 0xF8 };
	memset(bits_data, 0xFF, sizeof(bits_data));

	/* 0          1          2          3
 	 * 01234567 89012345 67890123 45678901 23456789
	 * 01110010 00111001 11000000 00000000 000xxxxx */
	EXPECT( bits_clear(&bits, 0));
	EXPECT( bits_clear_many(&bits, 4, 2));
	EXPECT( bits_clear_many(&bits, 7, 3));
	EXPECT( bits_clear_many(&bits, 13, 2));
	EXPECT( bits_clear_many(&bits, 18, 8));
	EXPECT(!bits_clear_many(&bits, 25, 15));
	EXPECT( bits_clear_many(&bits, 18, 17));
	EXPECT(memcmp(expected, bits_data, sizeof(bits_data)) == 0);
}


TEST(test_bits_write_boundary)
{
	uint8_t buf[2] = { 0, 0 };
	bits_init(&bits, buf, 8);
	EXPECT(bits_write_many(&bits, 1, 5, 3));
	EXPECT(buf[0] == 0xE0);

	buf[0] = 0;
	buf[1] = 0;
	bits_init(&bits, buf, 16);
	EXPECT(bits_write_many(&bits, 1, 4, 12));
	EXPECT(buf[0] == 0xF0 && buf[1] == 0xFF);
}


TEST(test_bits_write)
{
	uint8_t expected[] = { 0xB1, 0x63, 0xFC, 0xFF, 0x07 };
	memset(bits_data, 0x0, sizeof(bits_data));

	/* 0          1          2          3
 	 * 01234567 89012345 67890123 45678901 23456789
	 * 10001101 11000110 00111111 11111111 111xxxxx */
	EXPECT(bits_write(&bits, 1, 0));
	EXPECT(bits_write_many(&bits, 0, 2, 3));
	EXPECT(bits_write_many(&bits, 1, 4, 2));
	EXPECT(bits_write(&bits, 0, 6));
	EXPECT(bits_write_many(&bits, 1, 7, 3));
	EXPECT(bits_write_many(&bits, 0, 10, 3));
	EXPECT(bits_write_many(&bits, 1, 13, 2));
	EXPECT(bits_write_many(&bits, 0, 15, 3));
	EXPECT(bits_write_many(&bits, 1, 18, 17));
	EXPECT(memcmp(expected, bits_data, sizeof(bits_data)) == 0);
}


TEST(test_bits_flip)
{
	// uint8_t expected[] = { ~0xB1, ~0x63, ~0xFC, ~0xFF, ~0xFF };
	uint8_t expected[] = { 0x4E, 0x9C, 0x03, 0x00, 0x00 };

	/* 0          1          2          3
 	 * 01234567 89012345 67890123 45678901 23456789
	 * 01110010 00111001 11000000 00000000 000xxxxx */
	EXPECT(bits_flip_many(&bits, 0, 35));
	EXPECT(!bits_flip_many(&bits, 0, 40));
	EXPECT(memcmp(expected, bits_data, sizeof(bits_data)) == 0);
}


TEST(test_bits_count)
{
	EXPECT(bits_zeros(&bits) == 1+2+3+2+6+8+3);
	EXPECT(bits_ones(&bits) == 3+1+3+3);
}


TEST(test_bits_zeros)
{
	uint8_t temp1[2] = { 0xFF, 0x3 };
	bits_init(&bits, temp1, 10);
	EXPECT(bits_zeros(&bits) == 0);
}


TEST(test_bits_ones)
{
	uint8_t temp2[2] = { 0x00, 0xF8 };
	bits_init(&bits, temp2, 10);
	EXPECT(bits_ones(&bits) == 0);
}


TEST(test_bits_next_zero)
{
	/* 0          1          2          3
 	 * 01234567 89012345 67890123 45678901 23456789
	 * 01110010 00111001 11000000 00000000 000xxxxx */
	EXPECT(bits_next_zero(&bits, 0) == 0);
	EXPECT(bits_next_zero(&bits, 1) == 4);
	EXPECT(bits_next_zero(&bits, 4) == 4);
	EXPECT(bits_next_zero(&bits, 8) == 8);
	EXPECT(bits_next_zero(&bits, 15) == 18);
}


TEST(test_bits_next_one)
{
	/* 0          1          2          3
 	 * 01234567 89012345 67890123 45678901 23456789
	 * 01110010 00111001 11000000 00000000 000xxxxx */
	EXPECT(bits_next_one(&bits, 0) == 1);
	EXPECT(bits_next_one(&bits, 1) == 1);
	EXPECT(bits_next_one(&bits, 7) == 10);
	EXPECT(bits_next_one(&bits, 18) > bits_count(&bits));
}


void test_bits(void)
{
//	memset(bits_data, 0, sizeof(bits_data));
//	bits_init(&bits, bits_data, 5);
//	bits_zeros(&bits);

	tharness_run(test_bits_zeros);
	tharness_run(test_bits_ones);
	tharness_run(test_bits_write_boundary);

	bits_init(&bits, bits_data, 35);
	tharness_run(test_bits_set_all);
	tharness_run(test_bits_clear_all);
	tharness_run(test_bits_set);
	tharness_run(test_bits_clear);
	tharness_run(test_bits_write);
	tharness_run(test_bits_flip);
	tharness_run(test_bits_count);
	tharness_run(test_bits_next_zero);
	tharness_run(test_bits_next_one);
}


/******************************************* END OF FILE *******************************************/
