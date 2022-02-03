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

#include "tharness.h"

#include "byteorder.h"


// ----------------------------------------------------------------------------------------------- //
// General Tests                                                                                   //
// ----------------------------------------------------------------------------------------------- //
/* Network to Host ------------------------------------------------------------------------------- */
TEST(test_ntoh_u8)
{
	uint8_t x, y;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = ntoh_u8(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = ntoh_get_u8(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = ntoh_get_u8(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	ntoh_set_u8(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0x9A;
	ntoh_set_u8(0, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_ntoh_u16)
{
	uint16_t x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x1234;
	y = ntoh_u16(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	y = ntoh_get_u16(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x56 && buf[1] == 0x78);

	x = 0x1234;
	y = ntoh_get_u16(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0);

	ntoh_set_u16(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	ntoh_set_u16(0, x);
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);
}


TEST(test_ntoh_u32)
{
	uint32_t x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x12345678ul;
	y = ntoh_u32(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78);

	x = 0x9ABCDEF0ul;
	y = ntoh_get_u32(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x9A && buf[1] == 0xBC && buf[2] == 0xDE && buf[3] == 0xF0);

	x = 0x11223344ul;
	y = ntoh_get_u32(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0);

	x = 0x55667788ul;
	ntoh_set_u32(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);

	x = 0x99AABBCCul;
	ntoh_set_u32(0, x);
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);
}


TEST(test_ntoh_u64)
{
	uint64_t x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x123456789ABCDEF0ull;
	y = ntoh_u64(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78 &&
	       buf[4] == 0x9A && buf[5] == 0xBC && buf[6] == 0xDE && buf[7] == 0xF0);

	x = 0x1122334455667788ull;
	y = ntoh_get_u64(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x11 && buf[1] == 0x22 && buf[2] == 0x33 && buf[3] == 0x44 &&
	       buf[4] == 0x55 && buf[5] == 0x66 && buf[6] == 0x77 && buf[7] == 0x88);

	x = 0x99AABBCCDDEEFF00ull;
	y = ntoh_get_u64(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0 &&
	       buf[4] == 0 && buf[5] == 0 && buf[6] == 0 && buf[7] == 0);

	x = 0x0FEDCBA987654321ull;
	ntoh_set_u64(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);

	x = 0x8877665544332211ull;
	ntoh_set_u64(0, x);
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);
}


TEST(test_ntoh_i8)
{
	int8_t  x, y;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = ntoh_i8(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = ntoh_get_i8(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = ntoh_get_i8(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	ntoh_set_i8(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0;
	ntoh_set_i8(0, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_ntoh_i16)
{
	int16_t x, y;
	uint8_t buf[sizeof(x)];

	x = 0x1234;
	y = ntoh_i16(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	y = ntoh_get_i16(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x56 && buf[1] == 0x78);

	x = 0x1234;
	y = ntoh_get_i16(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0);

	ntoh_set_i16(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	ntoh_set_i16(0, x);
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);
}


TEST(test_ntoh_i32)
{
	int32_t x, y;
	uint8_t buf[sizeof(x)];

	x = 0x12345678L;
	y = ntoh_i32(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78);

	x = 0x9ABCDEF0L;
	y = ntoh_get_i32(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x9A && buf[1] == 0xBC && buf[2] == 0xDE && buf[3] == 0xF0);

	x = 0x11223344L;
	y = ntoh_get_i32(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0);

	x = 0x55667788L;
	ntoh_set_i32(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);

	x = 0x99AABBCCL;
	ntoh_set_i32(0, x);
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);
}


TEST(test_ntoh_i64)
{
	int64_t x, y;
	uint8_t buf[sizeof(x)];

	x = 0x123456789ABCDEF0LL;
	y = ntoh_i64(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78 &&
	       buf[4] == 0x9A && buf[5] == 0xBC && buf[6] == 0xDE && buf[7] == 0xF0);

	x = 0x1122334455667788LL;
	y = ntoh_get_i64(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x11 && buf[1] == 0x22 && buf[2] == 0x33 && buf[3] == 0x44 &&
	       buf[4] == 0x55 && buf[5] == 0x66 && buf[6] == 0x77 && buf[7] == 0x88);

	x = 0x99AABBCCDDEEFF00LL;
	y = ntoh_get_i64(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0 &&
	       buf[4] == 0 && buf[5] == 0 && buf[6] == 0 && buf[7] == 0);

	x = 0x0FEDCBA987654321LL;
	ntoh_set_i64(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);

	x = 0x8877665544332211LL;
	ntoh_set_i64(0, x);
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);
}



/* Host to Network ------------------------------------------------------------------------------- */
TEST(test_hton_u8)
{
	uint8_t x, y;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = hton_u8(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = hton_get_u8(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = hton_get_u8(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	hton_set_u8(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0x9A;
	hton_set_u8(0, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_hton_u16)
{
	uint16_t x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x1234;
	y = hton_u16(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	y = hton_get_u16(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x56 && buf[1] == 0x78);

	x = 0x1234;
	y = hton_get_u16(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0);

	hton_set_u16(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	hton_set_u16(0, x);
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);
}


TEST(test_hton_u32)
{
	uint32_t x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x12345678;
	y = hton_u32(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78);

	x = 0x9ABCDEF0;
	y = hton_get_u32(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x9A && buf[1] == 0xBC && buf[2] == 0xDE && buf[3] == 0xF0);

	x = 0x11223344;
	y = hton_get_u32(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0);

	x = 0x55667788;
	hton_set_u32(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);

	x = 0x99AABBCC;
	hton_set_u32(0, x);
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);
}


TEST(test_hton_u64)
{
	uint64_t x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x123456789ABCDEF0;
	y = hton_u64(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78 &&
	       buf[4] == 0x9A && buf[5] == 0xBC && buf[6] == 0xDE && buf[7] == 0xF0);

	x = 0x1122334455667788;
	y = hton_get_u64(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x11 && buf[1] == 0x22 && buf[2] == 0x33 && buf[3] == 0x44 &&
	       buf[4] == 0x55 && buf[5] == 0x66 && buf[6] == 0x77 && buf[7] == 0x88);

	x = 0x99AABBCCDDEEFF00;
	y = hton_get_u64(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0 &&
	       buf[4] == 0 && buf[5] == 0 && buf[6] == 0 && buf[7] == 0);

	x = 0x0FEDCBA987654321;
	hton_set_u64(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);

	x = 0x8877665544332211;
	hton_set_u64(0, x);
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);
}


TEST(test_hton_i8)
{
	int8_t  x, y;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = hton_i8(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = hton_get_i8(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = hton_get_i8(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	hton_set_i8(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0;
	hton_set_i8(0, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_hton_i16)
{
	int16_t x, y;
	uint8_t buf[sizeof(x)];

	x = 0x1234;
	y = hton_i16(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	y = hton_get_i16(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x56 && buf[1] == 0x78);

	x = 0x1234;
	y = hton_get_i16(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0);

	hton_set_i16(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	hton_set_i16(0, x);
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);
}


TEST(test_hton_i32)
{
	int32_t x, y;
	uint8_t buf[sizeof(x)];

	x = 0x12345678;
	y = hton_i32(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78);

	x = 0x9ABCDEF0;
	y = hton_get_i32(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x9A && buf[1] == 0xBC && buf[2] == 0xDE && buf[3] == 0xF0);

	x = 0x11223344;
	y = hton_get_i32(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0);

	x = 0x55667788;
	hton_set_i32(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);

	x = 0x99AABBCC;
	hton_set_i32(0, x);
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);
}


TEST(test_hton_i64)
{
	int64_t x, y;
	uint8_t buf[sizeof(x)];

	x = 0x123456789ABCDEF0;
	y = hton_i64(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78 &&
	       buf[4] == 0x9A && buf[5] == 0xBC && buf[6] == 0xDE && buf[7] == 0xF0);

	x = 0x1122334455667788;
	y = hton_get_i64(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x11 && buf[1] == 0x22 && buf[2] == 0x33 && buf[3] == 0x44 &&
	       buf[4] == 0x55 && buf[5] == 0x66 && buf[6] == 0x77 && buf[7] == 0x88);

	x = 0x99AABBCCDDEEFF00;
	y = hton_get_i64(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0 &&
	       buf[4] == 0 && buf[5] == 0 && buf[6] == 0 && buf[7] == 0);

	x = 0x0FEDCBA987654321;
	hton_set_i64(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);

	x = 0x8877665544332211;
	hton_set_i64(0, x);
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);
}



/* Big Endian ------------------------------------------------------------------------------------ */
TEST(test_be_u8)
{
	uint8_t x, y;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = be_u8(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = be_get_u8(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = be_get_u8(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	be_set_u8(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0x9A;
	be_set_u8(0, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_be_u16)
{
	uint16_t x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x1234;
	y = be_u16(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	y = be_get_u16(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x56 && buf[1] == 0x78);

	x = 0x1234;
	y = be_get_u16(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0);

	be_set_u16(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	be_set_u16(0, x);
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);
}


TEST(test_be_u32)
{
	uint32_t x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x12345678ul;
	y = be_u32(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78);

	x = 0x9ABCDEF0ul;
	y = be_get_u32(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x9A && buf[1] == 0xBC && buf[2] == 0xDE && buf[3] == 0xF0);

	x = 0x11223344ul;
	y = be_get_u32(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0);

	x = 0x55667788ul;
	be_set_u32(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);

	x = 0x99AABBCCul;
	be_set_u32(0, x);
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);
}


TEST(test_be_u64)
{
	uint64_t x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x123456789ABCDEF0ull;
	y = be_u64(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78 &&
	       buf[4] == 0x9A && buf[5] == 0xBC && buf[6] == 0xDE && buf[7] == 0xF0);

	x = 0x1122334455667788ull;
	y = be_get_u64(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x11 && buf[1] == 0x22 && buf[2] == 0x33 && buf[3] == 0x44 &&
	       buf[4] == 0x55 && buf[5] == 0x66 && buf[6] == 0x77 && buf[7] == 0x88);

	x = 0x99AABBCCDDEEFF00ull;
	y = be_get_u64(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0 &&
	       buf[4] == 0 && buf[5] == 0 && buf[6] == 0 && buf[7] == 0);

	x = 0x0FEDCBA987654321ull;
	be_set_u64(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);

	x = 0x8877665544332211ull;
	be_set_u64(0, x);
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);
}


TEST(test_be_i8)
{
	int8_t  x, y;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = be_i8(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = be_get_i8(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = be_get_i8(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	be_set_i8(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0;
	be_set_i8(0, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_be_i16)
{
	int16_t x, y;
	uint8_t buf[sizeof(x)];

	x = 0x1234;
	y = be_i16(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	y = be_get_i16(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x56 && buf[1] == 0x78);

	x = 0x1234;
	y = be_get_i16(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0);

	be_set_i16(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	be_set_i16(0, x);
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);
}


TEST(test_be_i32)
{
	int32_t x, y;
	uint8_t buf[sizeof(x)];

	x = 0x12345678L;
	y = be_i32(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78);

	x = 0x9ABCDEF0L;
	y = be_get_i32(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x9A && buf[1] == 0xBC && buf[2] == 0xDE && buf[3] == 0xF0);

	x = 0x11223344L;
	y = be_get_i32(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0);

	x = 0x55667788L;
	be_set_i32(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);

	x = 0x99AABBCCL;
	be_set_i32(0, x);
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);
}


TEST(test_be_i64)
{
	int64_t x, y;
	uint8_t buf[sizeof(x)];

	x = 0x123456789ABCDEF0ull;
	y = be_i64(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78 &&
	       buf[4] == 0x9A && buf[5] == 0xBC && buf[6] == 0xDE && buf[7] == 0xF0);

	x = 0x1122334455667788ull;
	y = be_get_i64(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x11 && buf[1] == 0x22 && buf[2] == 0x33 && buf[3] == 0x44 &&
	       buf[4] == 0x55 && buf[5] == 0x66 && buf[6] == 0x77 && buf[7] == 0x88);

	x = 0x99AABBCCDDEEFF00ull;
	y = be_get_i64(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0 &&
	       buf[4] == 0 && buf[5] == 0 && buf[6] == 0 && buf[7] == 0);

	x = 0x0FEDCBA987654321ull;
	be_set_i64(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);

	x = 0x8877665544332211ull;
	be_set_i64(0, x);
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);
}



/* Little Endian --------------------------------------------------------------------------------- */
TEST(test_le_u8)
{
	uint8_t x, y;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = le_u8(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = le_get_u8(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = le_get_u8(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	le_set_u8(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0x9A;
	le_set_u8(0, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_le_u16)
{
	uint16_t x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x1234;
	y = le_u16(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0x12 && buf[0] == 0x34);

	x = 0x5678;
	y = le_get_u16(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0x56 && buf[0] == 0x78);

	x = 0x1234;
	y = le_get_u16(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0 && buf[0] == 0);

	le_set_u16(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0x12 && buf[0] == 0x34);

	x = 0x5678;
	le_set_u16(0, x);
	EXPECT(buf[1] == 0x12 && buf[0] == 0x34);
}


TEST(test_le_u32)
{
	uint32_t x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x12345678ul;
	y = le_u32(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0x12 && buf[2] == 0x34 && buf[1] == 0x56 && buf[0] == 0x78);

	x = 0x9ABCDEF0ul;
	y = le_get_u32(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0x9A && buf[2] == 0xBC && buf[1] == 0xDE && buf[0] == 0xF0);

	x = 0x11223344ul;
	y = le_get_u32(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0 && buf[2] == 0 && buf[1] == 0 && buf[0] == 0);

	x = 0x55667788ul;
	le_set_u32(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0x55 && buf[2] == 0x66 && buf[1] == 0x77 && buf[0] == 0x88);

	x = 0x99AABBCCul;
	le_set_u32(0, x);
	EXPECT(buf[3] == 0x55 && buf[2] == 0x66 && buf[1] == 0x77 && buf[0] == 0x88);
}


TEST(test_le_u64)
{
	uint64_t x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x123456789ABCDEF0ull;
	y = le_u64(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0x12 && buf[6] == 0x34 && buf[5] == 0x56 && buf[4] == 0x78 &&
	       buf[3] == 0x9A && buf[2] == 0xBC && buf[1] == 0xDE && buf[0] == 0xF0);

	x = 0x1122334455667788ull;
	y = le_get_u64(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0x11 && buf[6] == 0x22 && buf[5] == 0x33 && buf[4] == 0x44 &&
	       buf[3] == 0x55 && buf[2] == 0x66 && buf[1] == 0x77 && buf[0] == 0x88);

	x = 0x99AABBCCDDEEFF00ull;
	y = le_get_u64(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0 && buf[6] == 0 && buf[5] == 0 && buf[4] == 0 &&
	       buf[3] == 0 && buf[2] == 0 && buf[1] == 0 && buf[0] == 0);

	x = 0x0FEDCBA987654321ull;
	le_set_u64(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0x0F && buf[6] == 0xED && buf[5] == 0xCB && buf[4] == 0xA9 &&
	       buf[3] == 0x87 && buf[2] == 0x65 && buf[1] == 0x43 && buf[0] == 0x21);

	x = 0x8877665544332211ull;
	le_set_u64(0, x);
	EXPECT(buf[7] == 0x0F && buf[6] == 0xED && buf[5] == 0xCB && buf[4] == 0xA9 &&
	       buf[3] == 0x87 && buf[2] == 0x65 && buf[1] == 0x43 && buf[0] == 0x21);
}


TEST(test_le_i8)
{
	int8_t  x, y;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = le_i8(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = le_get_i8(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = le_get_i8(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	le_set_i8(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0;
	le_set_i8(0, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_le_i16)
{
	int16_t  x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x1234;
	y = le_i16(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0x12 && buf[0] == 0x34);

	x = 0x5678;
	y = le_get_i16(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0x56 && buf[0] == 0x78);

	x = 0x1234;
	y = le_get_i16(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0 && buf[0] == 0);

	le_set_i16(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0x12 && buf[0] == 0x34);

	x = 0x5678;
	le_set_i16(0, x);
	EXPECT(buf[1] == 0x12 && buf[0] == 0x34);
}


TEST(test_le_i32)
{
	int32_t  x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x12345678L;
	y = le_i32(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0x12 && buf[2] == 0x34 && buf[1] == 0x56 && buf[0] == 0x78);

	x = 0x9ABCDEF0L;
	y = le_get_i32(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0x9A && buf[2] == 0xBC && buf[1] == 0xDE && buf[0] == 0xF0);

	x = 0x11223344L;
	y = le_get_i32(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0 && buf[2] == 0 && buf[1] == 0 && buf[0] == 0);

	x = 0x55667788L;
	le_set_i32(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0x55 && buf[2] == 0x66 && buf[1] == 0x77 && buf[0] == 0x88);

	x = 0x99AABBCCL;
	le_set_i32(0, x);
	EXPECT(buf[3] == 0x55 && buf[2] == 0x66 && buf[1] == 0x77 && buf[0] == 0x88);
}


TEST(test_le_i64)
{
	int64_t  x, y;
	uint8_t  buf[sizeof(x)];

	x = 0x123456789ABCDEF0LL;
	y = le_i64(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0x12 && buf[6] == 0x34 && buf[5] == 0x56 && buf[4] == 0x78 &&
	       buf[3] == 0x9A && buf[2] == 0xBC && buf[1] == 0xDE && buf[0] == 0xF0);

	x = 0x1122334455667788LL;
	y = le_get_i64(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0x11 && buf[6] == 0x22 && buf[5] == 0x33 && buf[4] == 0x44 &&
	       buf[3] == 0x55 && buf[2] == 0x66 && buf[1] == 0x77 && buf[0] == 0x88);

	x = 0x99AABBCCDDEEFF00LL;
	y = le_get_i64(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0 && buf[6] == 0 && buf[5] == 0 && buf[4] == 0 &&
	       buf[3] == 0 && buf[2] == 0 && buf[1] == 0 && buf[0] == 0);

	x = 0x0FEDCBA987654321LL;
	le_set_i64(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0x0F && buf[6] == 0xED && buf[5] == 0xCB && buf[4] == 0xA9 &&
	       buf[3] == 0x87 && buf[2] == 0x65 && buf[1] == 0x43 && buf[0] == 0x21);

	x = 0x8877665544332211LL;
	le_set_i64(0, x);
	EXPECT(buf[7] == 0x0F && buf[6] == 0xED && buf[5] == 0xCB && buf[4] == 0xA9 &&
	       buf[3] == 0x87 && buf[2] == 0x65 && buf[1] == 0x43 && buf[0] == 0x21);
}





// ----------------------------------------------------------------------------------------------- //
// Generic Tests                                                                                   //
// ----------------------------------------------------------------------------------------------- //
/* Network to Host ------------------------------------------------------------------------------- */
TEST(test_generic_ntoh_u8)
{
	uint8_t x, y;
	uint8_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0x9A;
	hton(p, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_generic_ntoh_u16)
{
	uint16_t x, y;
	uint16_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x1234;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x56 && buf[1] == 0x78);

	x = 0x1234;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0);

	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	hton(p, x);
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);
}


TEST(test_generic_ntoh_u32)
{
	uint32_t x, y;
	uint32_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x12345678ul;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78);

	x = 0x9ABCDEF0ul;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x9A && buf[1] == 0xBC && buf[2] == 0xDE && buf[3] == 0xF0);

	x = 0x11223344ul;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0);

	x = 0x55667788ul;
	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);

	x = 0x99AABBCCul;
	hton(p, x);
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);
}


TEST(test_generic_ntoh_u64)
{
	uint64_t x, y;
	uint64_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x123456789ABCDEF0ull;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78 &&
	       buf[4] == 0x9A && buf[5] == 0xBC && buf[6] == 0xDE && buf[7] == 0xF0);

	x = 0x1122334455667788ull;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x11 && buf[1] == 0x22 && buf[2] == 0x33 && buf[3] == 0x44 &&
	       buf[4] == 0x55 && buf[5] == 0x66 && buf[6] == 0x77 && buf[7] == 0x88);

	x = 0x99AABBCCDDEEFF00ull;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0 &&
	       buf[4] == 0 && buf[5] == 0 && buf[6] == 0 && buf[7] == 0);

	x = 0x0FEDCBA987654321ull;
	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);

	x = 0x8877665544332211ull;
	hton(p, x);
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);
}


TEST(test_generic_ntoh_i8)
{
	int8_t  x, y;
	int8_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0;
	hton(p, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_generic_ntoh_i16)
{
	int16_t x, y;
	int16_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x1234;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x56 && buf[1] == 0x78);

	x = 0x1234;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0);

	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	hton(p, x);
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);
}


TEST(test_generic_ntoh_i32)
{
	int32_t x, y;
	int32_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x12345678L;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78);

	x = 0x9ABCDEF0L;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x9A && buf[1] == 0xBC && buf[2] == 0xDE && buf[3] == 0xF0);

	x = 0x11223344L;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0);

	x = 0x55667788L;
	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);

	x = 0x99AABBCCL;
	hton(p, x);
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);
}


TEST(test_generic_ntoh_i64)
{
	int64_t x, y;
	int64_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x123456789ABCDEF0LL;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78 &&
	       buf[4] == 0x9A && buf[5] == 0xBC && buf[6] == 0xDE && buf[7] == 0xF0);

	x = 0x1122334455667788LL;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x11 && buf[1] == 0x22 && buf[2] == 0x33 && buf[3] == 0x44 &&
	       buf[4] == 0x55 && buf[5] == 0x66 && buf[6] == 0x77 && buf[7] == 0x88);

	x = 0x99AABBCCDDEEFF00LL;
	y = hton(0);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0 &&
	       buf[4] == 0 && buf[5] == 0 && buf[6] == 0 && buf[7] == 0);

	x = 0x0FEDCBA987654321LL;
	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);

	x = 0x8877665544332211LL;
	hton(p, x);
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);
}



/* Host to Network ------------------------------------------------------------------------------- */
TEST(test_generic_hton_u8)
{
	uint8_t x, y;
	uint8_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0x9A;
	hton(p, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_generic_hton_u16)
{
	uint16_t x, y;
	uint16_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x1234;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x56 && buf[1] == 0x78);

	x = 0x1234;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0);

	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	hton(p, x);
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);
}


TEST(test_generic_hton_u32)
{
	uint32_t x, y;
	uint32_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x12345678;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78);

	x = 0x9ABCDEF0;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x9A && buf[1] == 0xBC && buf[2] == 0xDE && buf[3] == 0xF0);

	x = 0x11223344;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0);

	x = 0x55667788;
	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);

	x = 0x99AABBCC;
	hton(p, x);
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);
}


TEST(test_generic_hton_u64)
{
	uint64_t x, y;
	uint64_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x123456789ABCDEF0;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78 &&
	       buf[4] == 0x9A && buf[5] == 0xBC && buf[6] == 0xDE && buf[7] == 0xF0);

	x = 0x1122334455667788;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x11 && buf[1] == 0x22 && buf[2] == 0x33 && buf[3] == 0x44 &&
	       buf[4] == 0x55 && buf[5] == 0x66 && buf[6] == 0x77 && buf[7] == 0x88);

	x = 0x99AABBCCDDEEFF00;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0 &&
	       buf[4] == 0 && buf[5] == 0 && buf[6] == 0 && buf[7] == 0);

	x = 0x0FEDCBA987654321;
	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);

	x = 0x8877665544332211;
	hton(p, x);
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);
}


TEST(test_generic_hton_i8)
{
	int8_t  x, y;
	int8_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0;
	hton(p, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_generic_hton_i16)
{
	int16_t x, y;
	int16_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x1234;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x56 && buf[1] == 0x78);

	x = 0x1234;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0);

	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	hton(p, x);
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);
}


TEST(test_generic_hton_i32)
{
	int32_t x, y;
	int32_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x12345678;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78);

	x = 0x9ABCDEF0;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x9A && buf[1] == 0xBC && buf[2] == 0xDE && buf[3] == 0xF0);

	x = 0x11223344;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0);

	x = 0x55667788;
	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);

	x = 0x99AABBCC;
	hton(p, x);
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);
}


TEST(test_generic_hton_i64)
{
	int64_t x, y;
	int64_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x123456789ABCDEF0;
	y = hton(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78 &&
	       buf[4] == 0x9A && buf[5] == 0xBC && buf[6] == 0xDE && buf[7] == 0xF0);

	x = 0x1122334455667788;
	y = hton(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x11 && buf[1] == 0x22 && buf[2] == 0x33 && buf[3] == 0x44 &&
	       buf[4] == 0x55 && buf[5] == 0x66 && buf[6] == 0x77 && buf[7] == 0x88);

	x = 0x99AABBCCDDEEFF00;
	y = hton(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0 &&
	       buf[4] == 0 && buf[5] == 0 && buf[6] == 0 && buf[7] == 0);

	x = 0x0FEDCBA987654321;
	hton(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);

	x = 0x8877665544332211;
	hton(p, x);
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);
}



/* Big Endian ------------------------------------------------------------------------------------ */
TEST(test_generic_be_u8)
{
	uint8_t x, y;
	uint8_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = be(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = be(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = be(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	be(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0x9A;
	be(p, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_generic_be_u16)
{
	uint16_t x, y;
	uint16_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x1234;
	y = be(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	y = be(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x56 && buf[1] == 0x78);

	x = 0x1234;
	y = be(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0);

	be(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	be(p, x);
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);
}


TEST(test_generic_be_u32)
{
	uint32_t x, y;
	uint32_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x12345678ul;
	y = be(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78);

	x = 0x9ABCDEF0ul;
	y = be(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x9A && buf[1] == 0xBC && buf[2] == 0xDE && buf[3] == 0xF0);

	x = 0x11223344ul;
	y = be(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0);

	x = 0x55667788ul;
	be(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);

	x = 0x99AABBCCul;
	be(p, x);
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);
}


TEST(test_generic_be_u64)
{
	uint64_t x, y;
	uint64_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x123456789ABCDEF0ull;
	y = be(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78 &&
	       buf[4] == 0x9A && buf[5] == 0xBC && buf[6] == 0xDE && buf[7] == 0xF0);

	x = 0x1122334455667788ull;
	y = be(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x11 && buf[1] == 0x22 && buf[2] == 0x33 && buf[3] == 0x44 &&
	       buf[4] == 0x55 && buf[5] == 0x66 && buf[6] == 0x77 && buf[7] == 0x88);

	x = 0x99AABBCCDDEEFF00ull;
	y = be(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0 &&
	       buf[4] == 0 && buf[5] == 0 && buf[6] == 0 && buf[7] == 0);

	x = 0x0FEDCBA987654321ull;
	be(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);

	x = 0x8877665544332211ull;
	be(p, x);
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);
}


TEST(test_generic_be_i8)
{
	int8_t  x, y;
	int8_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = be(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = be(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = be(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	be(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0;
	be(p, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_generic_be_i16)
{
	int16_t x, y;
	int16_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x1234;
	y = be(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	y = be(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x56 && buf[1] == 0x78);

	x = 0x1234;
	y = be(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0);

	be(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);

	x = 0x5678;
	be(p, x);
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34);
}


TEST(test_generic_be_i32)
{
	int32_t x, y;
	int32_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x12345678L;
	y = be(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78);

	x = 0x9ABCDEF0L;
	y = be(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x9A && buf[1] == 0xBC && buf[2] == 0xDE && buf[3] == 0xF0);

	x = 0x11223344L;
	y = be(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0);

	x = 0x55667788L;
	be(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);

	x = 0x99AABBCCL;
	be(p, x);
	EXPECT(buf[0] == 0x55 && buf[1] == 0x66 && buf[2] == 0x77 && buf[3] == 0x88);
}


TEST(test_generic_be_i64)
{
	int64_t x, y;
	int64_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x123456789ABCDEF0ull;
	y = be(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12 && buf[1] == 0x34 && buf[2] == 0x56 && buf[3] == 0x78 &&
	       buf[4] == 0x9A && buf[5] == 0xBC && buf[6] == 0xDE && buf[7] == 0xF0);

	x = 0x1122334455667788ull;
	y = be(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x11 && buf[1] == 0x22 && buf[2] == 0x33 && buf[3] == 0x44 &&
	       buf[4] == 0x55 && buf[5] == 0x66 && buf[6] == 0x77 && buf[7] == 0x88);

	x = 0x99AABBCCDDEEFF00ull;
	y = be(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0 &&
	       buf[4] == 0 && buf[5] == 0 && buf[6] == 0 && buf[7] == 0);

	x = 0x0FEDCBA987654321ull;
	be(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);

	x = 0x8877665544332211ull;
	be(p, x);
	EXPECT(buf[0] == 0x0F && buf[1] == 0xED && buf[2] == 0xCB && buf[3] == 0xA9 &&
	       buf[4] == 0x87 && buf[5] == 0x65 && buf[6] == 0x43 && buf[7] == 0x21);
}



/* Little Endian --------------------------------------------------------------------------------- */
TEST(test_generic_le_u8)
{
	uint8_t x, y;
	uint8_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = le(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = le(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = le(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	le(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0x9A;
	le(p, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_generic_le_u16)
{
	uint16_t x, y;
	uint16_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x1234;
	y = le(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0x12 && buf[0] == 0x34);

	x = 0x5678;
	y = le(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0x56 && buf[0] == 0x78);

	x = 0x1234;
	y = le(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0 && buf[0] == 0);

	le(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0x12 && buf[0] == 0x34);

	x = 0x5678;
	le(p, x);
	EXPECT(buf[1] == 0x12 && buf[0] == 0x34);
}


TEST(test_generic_le_u32)
{
	uint32_t x, y;
	uint32_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x12345678ul;
	y = le(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0x12 && buf[2] == 0x34 && buf[1] == 0x56 && buf[0] == 0x78);

	x = 0x9ABCDEF0ul;
	y = le(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0x9A && buf[2] == 0xBC && buf[1] == 0xDE && buf[0] == 0xF0);

	x = 0x11223344ul;
	y = le(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0 && buf[2] == 0 && buf[1] == 0 && buf[0] == 0);

	x = 0x55667788ul;
	le(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0x55 && buf[2] == 0x66 && buf[1] == 0x77 && buf[0] == 0x88);

	x = 0x99AABBCCul;
	le(p, x);
	EXPECT(buf[3] == 0x55 && buf[2] == 0x66 && buf[1] == 0x77 && buf[0] == 0x88);
}


TEST(test_generic_le_u64)
{
	uint64_t x, y;
	uint64_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x123456789ABCDEF0ull;
	y = le(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0x12 && buf[6] == 0x34 && buf[5] == 0x56 && buf[4] == 0x78 &&
	       buf[3] == 0x9A && buf[2] == 0xBC && buf[1] == 0xDE && buf[0] == 0xF0);

	x = 0x1122334455667788ull;
	y = le(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0x11 && buf[6] == 0x22 && buf[5] == 0x33 && buf[4] == 0x44 &&
	       buf[3] == 0x55 && buf[2] == 0x66 && buf[1] == 0x77 && buf[0] == 0x88);

	x = 0x99AABBCCDDEEFF00ull;
	y = le(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0 && buf[6] == 0 && buf[5] == 0 && buf[4] == 0 &&
	       buf[3] == 0 && buf[2] == 0 && buf[1] == 0 && buf[0] == 0);

	x = 0x0FEDCBA987654321ull;
	le(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0x0F && buf[6] == 0xED && buf[5] == 0xCB && buf[4] == 0xA9 &&
	       buf[3] == 0x87 && buf[2] == 0x65 && buf[1] == 0x43 && buf[0] == 0x21);

	x = 0x8877665544332211ull;
	le(p, x);
	EXPECT(buf[7] == 0x0F && buf[6] == 0xED && buf[5] == 0xCB && buf[4] == 0xA9 &&
	       buf[3] == 0x87 && buf[2] == 0x65 && buf[1] == 0x43 && buf[0] == 0x21);
}


TEST(test_generic_le_i8)
{
	int8_t x, y;
	int8_t* p = 0;
	uint8_t buf[sizeof(x)];

	x = 0x12;
	y = le(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x12);

	x = 0x34;
	y = le(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x34);

	x = 0x56;
	y = le(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0);

	x = 0x78;
	le(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[0] == 0x78);

	x = 0;
	le(p, x);
	EXPECT(buf[0] == 0x78);
}


TEST(test_generic_le_i16)
{
	int16_t x, y;
	int16_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x1234;
	y = le(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0x12 && buf[0] == 0x34);

	x = 0x5678;
	y = le(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0x56 && buf[0] == 0x78);

	x = 0x1234;
	y = le(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0 && buf[0] == 0);

	le(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[1] == 0x12 && buf[0] == 0x34);

	x = 0x5678;
	le(p     , x);
	EXPECT(buf[1] == 0x12 && buf[0] == 0x34);
}


TEST(test_generic_le_i32)
{
	int32_t x, y;
	int32_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x12345678L;
	y = le(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0x12 && buf[2] == 0x34 && buf[1] == 0x56 && buf[0] == 0x78);

	x = 0x9ABCDEF0L;
	y = le(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0x9A && buf[2] == 0xBC && buf[1] == 0xDE && buf[0] == 0xF0);

	x = 0x11223344L;
	y = le(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0 && buf[2] == 0 && buf[1] == 0 && buf[0] == 0);

	x = 0x55667788L;
	le(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[3] == 0x55 && buf[2] == 0x66 && buf[1] == 0x77 && buf[0] == 0x88);

	x = 0x99AABBCCL;
	le(p, x);
	EXPECT(buf[3] == 0x55 && buf[2] == 0x66 && buf[1] == 0x77 && buf[0] == 0x88);
}


TEST(test_generic_le_i64)
{
	int64_t x, y;
	int64_t* p = 0;
	uint8_t  buf[sizeof(x)];

	x = 0x123456789ABCDEF0LL;
	y = le(x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0x12 && buf[6] == 0x34 && buf[5] == 0x56 && buf[4] == 0x78 &&
	       buf[3] == 0x9A && buf[2] == 0xBC && buf[1] == 0xDE && buf[0] == 0xF0);

	x = 0x1122334455667788LL;
	y = le(&x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0x11 && buf[6] == 0x22 && buf[5] == 0x33 && buf[4] == 0x44 &&
	       buf[3] == 0x55 && buf[2] == 0x66 && buf[1] == 0x77 && buf[0] == 0x88);

	x = 0x99AABBCCDDEEFF00LL;
	y = le(p);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0 && buf[6] == 0 && buf[5] == 0 && buf[4] == 0 &&
	       buf[3] == 0 && buf[2] == 0 && buf[1] == 0 && buf[0] == 0);

	x = 0x0FEDCBA987654321LL;
	le(&y, x);
	memmove(buf, &y, sizeof(y));
	EXPECT(buf[7] == 0x0F && buf[6] == 0xED && buf[5] == 0xCB && buf[4] == 0xA9 &&
	       buf[3] == 0x87 && buf[2] == 0x65 && buf[1] == 0x43 && buf[0] == 0x21);

	x = 0x8877665544332211LL;
	le(p, x);
	EXPECT(buf[7] == 0x0F && buf[6] == 0xED && buf[5] == 0xCB && buf[4] == 0xA9 &&
	       buf[3] == 0x87 && buf[2] == 0x65 && buf[1] == 0x43 && buf[0] == 0x21);
}


void test_byteorder(void)
{
	tharness_run(test_ntoh_u8);
	tharness_run(test_ntoh_u16);
	tharness_run(test_ntoh_u32);
	tharness_run(test_ntoh_u64);
	tharness_run(test_ntoh_i8);
	tharness_run(test_ntoh_i16);
	tharness_run(test_ntoh_i32);
	tharness_run(test_ntoh_i64);
	tharness_run(test_hton_u8);
	tharness_run(test_hton_u16);
	tharness_run(test_hton_u32);
	tharness_run(test_hton_u64);
	tharness_run(test_hton_i8);
	tharness_run(test_hton_i16);
	tharness_run(test_hton_i32);
	tharness_run(test_hton_i64);
	tharness_run(test_be_u8);
	tharness_run(test_be_u16);
	tharness_run(test_be_u32);
	tharness_run(test_be_u64);
	tharness_run(test_be_i8);
	tharness_run(test_be_i16);
	tharness_run(test_be_i32);
	tharness_run(test_be_i64);
	tharness_run(test_le_u8);
	tharness_run(test_le_u16);
	tharness_run(test_le_u32);
	tharness_run(test_le_u64);
	tharness_run(test_le_i8);
	tharness_run(test_le_i16);
	tharness_run(test_le_i32);
	tharness_run(test_le_i64);

	tharness_run(test_generic_ntoh_u8);
	tharness_run(test_generic_ntoh_u16);
	tharness_run(test_generic_ntoh_u32);
	tharness_run(test_generic_ntoh_u64);
	tharness_run(test_generic_ntoh_i8);
	tharness_run(test_generic_ntoh_i16);
	tharness_run(test_generic_ntoh_i32);
	tharness_run(test_generic_ntoh_i64);
	tharness_run(test_generic_hton_u8);
	tharness_run(test_generic_hton_u16);
	tharness_run(test_generic_hton_u32);
	tharness_run(test_generic_hton_u64);
	tharness_run(test_generic_hton_i8);
	tharness_run(test_generic_hton_i16);
	tharness_run(test_generic_hton_i32);
	tharness_run(test_generic_hton_i64);
	tharness_run(test_generic_be_u8);
	tharness_run(test_generic_be_u16);
	tharness_run(test_generic_be_u32);
	tharness_run(test_generic_be_u64);
	tharness_run(test_generic_be_i8);
	tharness_run(test_generic_be_i16);
	tharness_run(test_generic_be_i32);
	tharness_run(test_generic_be_i64);
	tharness_run(test_generic_le_u8);
	tharness_run(test_generic_le_u16);
	tharness_run(test_generic_le_u32);
	tharness_run(test_generic_le_u64);
	tharness_run(test_generic_le_i8);
	tharness_run(test_generic_le_i16);
	tharness_run(test_generic_le_i32);
	tharness_run(test_generic_le_i64);
}

/******************************************* END OF FILE *******************************************/
