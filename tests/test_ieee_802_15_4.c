/************************************************************************************************//**
 * @file		test_ieee_802_15_4.c
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

#include "test_ieee_802_15_4.h"

#include "ieee_802_15_4.h"
#include "tharness.h"


/* Private Variables ----------------------------------------------------------------------------- */
Ieee154_Frame ieee154_frame;
uint8_t ieee154_frame_data[127];

//uint8_t data[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
uint8_t ieee154_payload1[] = "Hello world!";
uint8_t ieee154_payload2[] = "Feed me.";
uint8_t ieee154_data1[] = {0x11, 0x11};
uint8_t ieee154_data2[] = {0x22, 0x22, 0x22};
uint8_t ieee154_data3[] = {0x33, 0x33, 0x33, 0x33, 0x33};
uint8_t ieee154_data4[] = {0x44, 0x44, 0x44, 0x44};
uint8_t ieee154_data5[] = {0x55, 0x55, 0x55};
uint8_t ieee154_data6[] = {0x66, 0x66, 0x66, 0x66};

uint8_t datalong[] = {
	1,  2,  3,  4,  5,  6,  7,  8,
	9,  10, 11, 12, 13, 14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32,
	33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 51, 52, 53, 54, 55, 56,
	57, 58, 59, 60, 61, 62, 63,
};

TEST(test_ieee154_beacon_frame_init)
{
	uint16_t fctrl;

	/* 0           1
	 * 0123 4567 8901 2345
	 * 0000 0000 1000 0100		(0x2100)
	 * |  | |||| |||  | |
	 * |  | |||| |||  | +sam
	 * |  | |||| |||  +frame version
	 * |  | |||| ||+dam
	 * |  | |||| |+ie present
	 * |  | |||| +seqnum supress
	 * |  | |||+reserved
	 * |  | ||+pan id comp
	 * |  | |+ar
	 * |  | +frame pending
	 * |  +security
	 * +frame type */
	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));

	fctrl = ieee154_fctrl(&ieee154_frame);

	EXPECT(fctrl == 0x2100);
}


TEST(test_ieee154_set_ack_request)
{
	uint16_t fctrl;

	/* 0           1
	 * 0123 4567 8901 2345
	 * 0000 0100 1000 0100		(0x2120)
	 * |  | |||| |||  | |
	 * |  | |||| |||  | +sam
	 * |  | |||| |||  +frame version
	 * |  | |||| ||+dam
	 * |  | |||| |+ie present
	 * |  | |||| +seqnum supress
	 * |  | |||+reserved
	 * |  | ||+pan id comp
	 * |  | |+ar
	 * |  | +frame pending
	 * |  +security
	 * +frame type */
	EXPECT(ieee154_set_ack_request(&ieee154_frame, true));
	fctrl = ieee154_fctrl(&ieee154_frame);
	EXPECT(fctrl == 0x2120);
}


TEST(test_ieee154_set_seqnum)
{
	uint16_t fctrl;
	uint8_t expected[] = {
		0x20, 0x20,
		0xC1
	};

	/* 0           1
	 * 0123 4567 8901 2345
	 * 0000 0100 0000 0100		(0x2020)
	 * |  | |||| |||  | |
	 * |  | |||| |||  | +sam
	 * |  | |||| |||  +frame version
	 * |  | |||| ||+dam
	 * |  | |||| |+ie present
	 * |  | |||| +seqnum supress
	 * |  | |||+reserved
	 * |  | ||+pan id comp
	 * |  | |+ar
	 * |  | +frame pending
	 * |  +security
	 * +frame type */
	EXPECT(ieee154_set_seqnum(&ieee154_frame, 0xC1));
	fctrl = ieee154_fctrl(&ieee154_frame);
	EXPECT(fctrl == 0x2020);
	EXPECT(memcmp(expected, ieee154_ptr_start(&ieee154_frame), ieee154_length(&ieee154_frame)) == 0);
}


TEST(test_ieee154_set_addr)
{
	uint8_t dest_short[] = { 0x12, 0x34 };
	uint8_t dest_long[]  = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
	uint8_t src_short[]  = { 0x56, 0x78 };
	uint8_t src_long[]   = { 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x01 };

	/* 	PAN ID Compression Field:
	 * 	Dest.	Src.	Dest	Src		PAN ID
	 * 	Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 	0		0		0		0		0
	 * 	0		0		1		0		1
	 * 	0		0		x		1			<-- Invalid
	 *
	 * 	2		0		1		0		0
	 * 	3		0		1		0		0
	 * 	2		0		0		0		1
	 * 	3		0		0		0		1
	 * 	2,3		0		x		1			<-- Invalid
	 *
	 * 	0		2		0		1		0
	 * 	0		3		0		1		0
	 * 	0		2		0		0		1
	 * 	0		3		0		0		1
	 * 	0		2,3		1		x			<-- Invalid
	 *
	 * 	3		3		1		0		0
	 * 	3		3		0		0		1
	 * 	3		3		x		1			<-- Invalid
	 *
	 * 	2		2		1		1		0
	 * 	2		3		1		1		0
	 * 	3		2		1		1		0
	 *
	 * 	2		3		1		0		1
	 * 	3		2		1		0		1
	 * 	2		2		1		0		1
	 * 	2,3		2,3		0		x			<-- Invalid */
	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 0		0		0		0		0 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0, 0, 0, 0, 0, 0));
	EXPECT(!(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 0		0		1		0		1 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0xFACE, 0, 0, 0, 0, 0));
	EXPECT(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK);

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 0		0		x		1			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0, 0, 0, 0xBAAD, 0, 0));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 0		0		x		1			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0xFACE, 0, 0, 0xBAAD, 0, 0));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2		0		1		0		0 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0xFACE, dest_short, 2, 0, 0, 0));
	EXPECT(!(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 3		0		1		0		0 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0xFACE, dest_long, 8, 0, 0, 0));
	EXPECT(!(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2		0		0		0		1 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0, dest_short, 2, 0, 0, 0));
	EXPECT(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK);

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 3		0		0		0		1 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0, dest_long, 8, 0, 0, 0));
	EXPECT(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK);

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2,3		0		x		1			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0, dest_short, 2, 0xBAAD, 0, 0));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* Invalid */
	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2,3		0		x		1			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0, dest_long, 8, 0xBAAD, 0, 0));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2,3		0		x		1			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0xFACE, dest_short, 2, 0xBAAD, 0, 0));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2,3		0		x		1			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0xFACE, dest_long, 8, 0xBAAD, 0, 0));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 0		2		0		1		0 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0, 0, 0, 0xF00D, src_short, 2));
	EXPECT(!(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 0		3		0		1		0 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0, 0, 0, 0xF00D, src_long, 8));
	EXPECT(!(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 0		2		0		0		1 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0, 0, 0, 0, src_short, 2));
	EXPECT(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK);

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 0		3		0		0		1 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0, 0, 0, 0, src_long, 8));
	EXPECT(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK);

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 0		2,3		1		x			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0xFACE, 0, 0, 0, src_short, 2));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 0		2,3		1		x			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0xFACE, 0, 0, 0, src_long, 8));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 0		2,3		1		x			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0xFACE, 0, 0, 0xBAAD, src_short, 2));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 0		2,3		1		x			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0xFACE, 0, 0, 0xBAAD, src_long, 8));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 3		3		1		0		0 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0xFACE, dest_long, 8, 0, src_long, 8));
	EXPECT(!(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 3		3		0		0		1 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0, dest_long, 8, 0, src_long, 8));
	EXPECT(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK);

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 3		3		x		1			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0xFACE, dest_long, 8, 0xBAAD, src_long, 8));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 3		3		x		1			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0, dest_long, 8, 0xBAAD, src_long, 8));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2		2		1		1		0 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0xFACE, dest_short, 2, 0xF00D, src_short, 2));
	EXPECT(!(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2		3		1		1		0 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0xFACE, dest_short, 2, 0xF00D, src_long, 8));
	EXPECT(!(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 3		2		1		1		0 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0xFACE, dest_long, 8, 0xF00D, src_short, 2));
	EXPECT(!(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2		3		1		0		1 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0xFACE, dest_short, 2, 0, src_long, 8));
	EXPECT(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK);

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 3		2		1		0		1 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0xFACE, dest_long, 8, 0, src_short, 2));
	EXPECT(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK);

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2		2		1		0		1 */
	EXPECT(ieee154_set_addr(&ieee154_frame, 0xFACE, dest_short, 2, 0, src_short, 2));
	EXPECT(ieee154_fctrl(&ieee154_frame) & IEEE154_PAN_ID_COMPRESS_MASK);

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2,3		2,3		0		x			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0, dest_short, 2, 0, src_short, 2));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2,3		2,3		0		x			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0, dest_short, 2, 0, src_long, 8));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2,3		2,3		0		x			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0, dest_long, 8, 0, src_short, 2));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2,3		2,3		0		x			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0, dest_short, 2, 0xBAAD, src_short, 2));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2,3		2,3		0		x			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0, dest_short, 2, 0xBAAD, src_long, 8));

	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);

	/* PAN ID Compression Field:
	 * Dest.	Src.	Dest	Src		PAN ID
	 * Addr.	Addr.	PAN ID	PAN ID	Compression
	 * 2,3		2,3		0		x			<-- Invalid */
	EXPECT(!ieee154_set_addr(&ieee154_frame, 0, dest_long, 8, 0xBAAD, src_short, 2));
}


TEST(test_ieee154_header_ie)
{
	uint16_t fctrl;
	uint8_t dest_short[] = { 0x12, 0x34 };
	uint8_t src_short[]  = { 0x56, 0x78 };

	uint8_t expected[] = {
		0x00, 0xAA,
		0xC1,
		0xFA, 0xCE,
		0x12, 0x34,
		0xF0, 0x0D,
		0x56, 0x78,
		0x02, 0x0D, 0x11, 0x11,
		0x83, 0x0D, 0x22, 0x22, 0x22,
		0x05, 0x0E, 0x33, 0x33, 0x33, 0x33, 0x33,
		0x00, 0x3F,
	};

	memset(ieee154_frame_data, 0, sizeof(ieee154_frame_data));
	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_seqnum(&ieee154_frame, 0xC1);
	ieee154_set_addr(&ieee154_frame, 0xFACE, dest_short, 2, 0xF00D, src_short, 2);

	Ieee154_IE hie = ieee154_ie_first(&ieee154_frame);

	/* 11 15 */
	EXPECT(ieee154_hie_append(&hie, IEEE154_CSL_IE, &ieee154_data1, sizeof(ieee154_data1)));

	/* 15 20 */
	EXPECT(ieee154_hie_append(&hie, IEEE154_RIT_IE, &ieee154_data2, sizeof(ieee154_data2)));

	/* 20 27 */
	EXPECT(ieee154_hie_append(&hie, IEEE154_DSME_PAN_DESCRIPTOR_IE, &ieee154_data3, sizeof(ieee154_data3)));

	/* 27 29 */
	EXPECT(ieee154_hie_append(&hie, IEEE154_HT1_IE, 0, 0));

	/* 0           1
	 * 0123 4567 8901 2345
	 * 0000 0100 0101 0101		(0xAA20)
	 * |  | |||| |||  | |
	 * |  | |||| |||  | +sam
	 * |  | |||| |||  +frame version
	 * |  | |||| ||+dam
	 * |  | |||| |+ie present
	 * |  | |||| +seqnum supress
	 * |  | |||+reserved
	 * |  | ||+pan id comp
	 * |  | |+ar
	 * |  | +frame pending
	 * |  +security
	 * +frame type */
	fctrl = ieee154_fctrl(&ieee154_frame);
	EXPECT(fctrl == 0xAA00);

	EXPECT(memcmp(expected, ieee154_ptr_start(&ieee154_frame), ieee154_length(&ieee154_frame)) == 0);
}


TEST(test_ieee154_payload_ie)
{
	uint16_t fctrl;

	uint8_t expected[] = {
		0x00, 0xAA,
		0xC1,
		0xFA, 0xCE,
		0x12, 0x34,
		0xF0, 0x0D,
		0x56, 0x78,
		0x02, 0x0D, 0x11, 0x11,
		0x83, 0x0D, 0x22, 0x22, 0x22,
		0x05, 0x0E, 0x33, 0x33, 0x33, 0x33, 0x33,
		0x00, 0x3F,
		0x03, 0x90, 0x22, 0x22, 0x22,
		0x00, 0x88,
		0x00, 0xF8,
	};

	Ieee154_IE pie = ieee154_ie_first(&ieee154_frame);

	while(!ieee154_ie_is_last(&pie))
	{
		ieee154_ie_next(&pie);
	}

	EXPECT(ieee154_pie_append(&pie, IEEE154_VENDOR_SPECIFIC_PAYLOAD_IE, &ieee154_data2, sizeof(ieee154_data2)));
	EXPECT(ieee154_pie_append(&pie, IEEE154_MLME_IE, 0, 0));
	EXPECT(ieee154_pie_append(&pie, IEEE154_PT_IE, 0, 0));

	/* 0           1
	 * 0123 4567 8901 2345
	 * 0000 0100 0101 0101		(0xAA20)
	 * |  | |||| |||  | |
	 * |  | |||| |||  | +sam
	 * |  | |||| |||  +frame version
	 * |  | |||| ||+dam
	 * |  | |||| |+ie present
	 * |  | |||| +seqnum supress
	 * |  | |||+reserved
	 * |  | ||+pan id comp
	 * |  | |+ar
	 * |  | +frame pending
	 * |  +security
	 * +frame type */
	fctrl = ieee154_fctrl(&ieee154_frame);
	EXPECT(fctrl == 0xAA00);

	EXPECT(memcmp(expected, ieee154_ptr_start(&ieee154_frame), ieee154_length(&ieee154_frame)) == 0);
}


TEST(test_ieee154_nested_ie)
{
	uint16_t fctrl;

	Ieee154_IE ie, nie;

	uint8_t expected[] = {
		0x00, 0xAA,
		0xC1,
		0xFA, 0xCE,
		0x12, 0x34,
		0xF0, 0x0D,
		0x56, 0x78,
		0x02, 0x0D, 0x11, 0x11,
		0x83, 0x0D, 0x22, 0x22, 0x22,
		0x05, 0x0E, 0x33, 0x33, 0x33, 0x33, 0x33,
		0x00, 0x3F,
		0x03, 0x90, 0x22, 0x22, 0x22,
		0x14, 0x88,
		0x02, 0x1A, 0x11, 0x11,
		0x03, 0x1B, 0x22, 0x22, 0x22,
		0x09, 0x1C, 0x33, 0x33, 0x33, 0x33, 0x33, 0x44, 0x44, 0x44, 0x44,
		0x00, 0xF8,
	};

	ie = ieee154_ie_first(&ieee154_frame);

	while(ieee154_ie_is_valid(&ie))
	{
		if(ieee154_ie_is_pie(&ie) && ieee154_ie_type(&ie) == IEEE154_MLME_IE)
		{
			break;
		}

		ieee154_ie_next(&ie);
	}

	nie = ieee154_nie_first(&ie);
	EXPECT(ieee154_nie_append(&nie, IEEE154_TSCH_SYNC_IE, ieee154_data1, sizeof(ieee154_data1)));
	EXPECT(ieee154_nie_append(&nie, IEEE154_TSCH_SLOTFRAME_AND_LINK_IE, ieee154_data2, sizeof(ieee154_data2)));
	EXPECT(ieee154_nie_append(&nie, IEEE154_TSCH_TIMESLOT_IE, ieee154_data3, sizeof(ieee154_data3)));

	Buffer* buffer = ieee154_ie_reset_buffer(&nie);
	buffer_push_mem(buffer, ieee154_data4, sizeof(ieee154_data4));

	ieee154_ie_finalize(&nie);

	/* 0           1
	 * 0123 4567 8901 2345
	 * 0000 0100 0101 0101		(0xAA20)
	 * |  | |||| |||  | |
	 * |  | |||| |||  | +sam
	 * |  | |||| |||  +frame version
	 * |  | |||| ||+dam
	 * |  | |||| |+ie present
	 * |  | |||| +seqnum supress
	 * |  | |||+reserved
	 * |  | ||+pan id comp
	 * |  | |+ar
	 * |  | +frame pending
	 * |  +security
	 * +frame type */
	fctrl = ieee154_fctrl(&ieee154_frame);
	EXPECT(fctrl == 0xAA00);

	EXPECT(memcmp(expected, ieee154_ptr_start(&ieee154_frame), ieee154_length(&ieee154_frame)) == 0);

	unsigned i;
	for(i = 0; i < ieee154_length(&ieee154_frame); i++)
	{
		PRINT("%d ", i);

		if(expected[i] != ieee154_frame_data[i])
		{
			PRINT("> ");
		}
		else
		{
			PRINT("  ");
		}

		PRINT("%02X %02X\n", expected[i], ieee154_frame_data[i]);
	}
}


TEST(test_ieee154_payload)
{
	uint16_t fctrl;

	uint8_t expected[] = {
		0x00, 0xAA,
		0xC1,
		0xFA, 0xCE,
		0x12, 0x34,
		0xF0, 0x0D,
		0x56, 0x78,
		0x02, 0x0D, 0x11, 0x11,
		0x83, 0x0D, 0x22, 0x22, 0x22,
		0x05, 0x0E, 0x33, 0x33, 0x33, 0x33, 0x33,
		0x00, 0x3F,
		0x03, 0x90, 0x22, 0x22, 0x22,
		0x14, 0x88,
		0x02, 0x1A, 0x11, 0x11,
		0x03, 0x1B, 0x22, 0x22, 0x22,
		0x09, 0x1C, 0x33, 0x33, 0x33, 0x33, 0x33, 0x44, 0x44, 0x44, 0x44,
		0x00, 0xF8,
		'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '\0',
		'F', 'e', 'e', 'd', ' ', 'm', 'e', '.', '\0',
	};

	Buffer* buffer = ieee154_reset_buffer(&ieee154_frame);
	buffer_push_mem(buffer, ieee154_payload1, sizeof(ieee154_payload1));
	buffer_push_mem(buffer, ieee154_payload2, sizeof(ieee154_payload2));

	/* 0           1
	 * 0123 4567 8901 2345
	 * 0000 0100 0101 0101		(0xAA20)
	 * |  | |||| |||  | |
	 * |  | |||| |||  | +sam
	 * |  | |||| |||  +frame version
	 * |  | |||| ||+dam
	 * |  | |||| |+ie present
	 * |  | |||| +seqnum supress
	 * |  | |||+reserved
	 * |  | ||+pan id comp
	 * |  | |+ar
	 * |  | +frame pending
	 * |  +security
	 * +frame type */
	fctrl = ieee154_fctrl(&ieee154_frame);
	EXPECT(fctrl == 0xAA00);

	EXPECT(memcmp(expected, ieee154_ptr_start(&ieee154_frame), ieee154_length(&ieee154_frame)) == 0);
}


TEST(test_ieee154_read)
{
	uint16_t expected_hie[] = {
		IEEE154_CSL_IE,
		IEEE154_RIT_IE,
		IEEE154_DSME_PAN_DESCRIPTOR_IE,
		IEEE154_HT1_IE,
	};

	uint16_t expected_pie[] = {
		IEEE154_VENDOR_SPECIFIC_PAYLOAD_IE,
		IEEE154_MLME_IE,
		IEEE154_PT_IE,
	};

	uint16_t expected_nie[] = {
		IEEE154_TSCH_SYNC_IE,
		IEEE154_TSCH_SLOTFRAME_AND_LINK_IE,
		IEEE154_TSCH_TIMESLOT_IE,
	};

	uint8_t expected_dest_short[] = { 0x12, 0x34 };
	uint8_t expected_src_short[]  = { 0x56, 0x78 };

	uint16_t fctrl      = ieee154_fctrl     (&ieee154_frame);
	uint8_t  seqnum     = ieee154_seqnum    (&ieee154_frame);
	uint16_t dest_panid = ieee154_dest_panid(&ieee154_frame);
	void*    dest_addr  = ieee154_dest_addr (&ieee154_frame);
	uint16_t src_panid  = ieee154_src_panid (&ieee154_frame);
	void*    src_addr   = ieee154_src_addr  (&ieee154_frame);

	EXPECT(fctrl      == 0xAA00);
	EXPECT(seqnum     == 0xC1);
	EXPECT(dest_panid == 0xFACE);
	EXPECT(src_panid  == 0xF00D);
	EXPECT(memcmp(dest_addr, expected_dest_short, 2) == 0);
	EXPECT(memcmp(src_addr,  expected_src_short,  2) == 0);

//	Ieee154_IE nie, ie = ieee154_peek_ie(&ieee154_frame);
	Ieee154_IE nie, nie_parent;
	Ieee154_IE ie = ieee154_ie_first(&ieee154_frame);
//	Ieee154_IE nie, ie = ieee154_ie_first(&ieee154_frame);

	unsigned i = 0;
	while(ieee154_ie_is_valid(&ie) && ieee154_ie_is_hie(&ie))
	{
		EXPECT(i < sizeof(expected_hie) / sizeof(expected_hie[0]));
		EXPECT(ieee154_ie_type(&ie) == expected_hie[i]);
		PRINT("%X\n", ieee154_ie_type(&ie));
		i++;

		ieee154_ie_next(&ie);
	}

	i = 0;
	while(ieee154_ie_is_valid(&ie) && ieee154_ie_is_pie(&ie))
	{
		EXPECT(i < sizeof(expected_pie) / sizeof(expected_pie[0]));
		EXPECT(ieee154_ie_type(&ie) == expected_pie[i]);
		PRINT("%X\n", ieee154_ie_type(&ie));
		i++;

		if(ieee154_ie_type(&ie) == IEEE154_MLME_IE)
		{
			nie_parent = ie;
			nie = ieee154_nie_first(&nie_parent);
		}

		ieee154_ie_next(&ie);
	}

	i = 0;
	while(ieee154_ie_is_valid(&nie))
	{
		EXPECT(i < sizeof(expected_nie) / sizeof(expected_nie[0]));
		EXPECT(ieee154_ie_type(&nie) == expected_nie[i]);
		PRINT("%X\n", ieee154_ie_type(&nie));
		i++;

		ieee154_ie_next(&nie);
	}
}


TEST(test_tsch_sync_ie)
{
	uint8_t dest_short[] = { 0x12, 0x34 };
	uint8_t src_short[]  = { 0x56, 0x78 };

	Ieee154_IE ie, nie;

	/* 0           1
	 * 0123 4567 8901 2345
	 * 0000 0000 1100 0100		(0x2300)
	 * |  | |||| |||  | |
	 * |  | |||| |||  | +sam
	 * |  | |||| |||  +frame version
	 * |  | |||| ||+dam
	 * |  | |||| |+ie present
	 * |  | |||| +seqnum supress
	 * |  | |||+reserved
	 * |  | ||+pan id comp
	 * |  | |+ar
	 * |  | +frame pending
	 * |  +security
	 * +frame type */
	memset(ieee154_frame_data, 0, sizeof(ieee154_frame_data));
	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_addr(&ieee154_frame, 0xFACE, dest_short, 2, 0xF00D, src_short, 2);

	/* Append tsch_sync_ie */
	ie = ieee154_ie_first(&ieee154_frame);
	EXPECT(ieee154_hie_append(&ie, IEEE154_HT1_IE, 0, 0));
	EXPECT(ieee154_pie_append(&ie, IEEE154_MLME_IE, 0, 0));

	nie = ieee154_nie_first(&ie);
	EXPECT(tsch_sync_ie_append(&nie, 0x123456789A, 20));
	ieee154_ie_finalize(&nie);

	/* Read tsch_sync_ie */
	ie = ieee154_ie_first(&ieee154_frame);	/* IEEE154_HT1_IE       */
	ieee154_ie_next(&ie);					/* IEEE154_MLME_IE      */
	nie = ieee154_nie_first(&ie);			/* IEEE154_TSCH_SYNC_IE */

	EXPECT(0x123456789A == tsch_sync_ie_asn(&nie));
	EXPECT(20 == tsch_sync_ie_join_metric(&nie));
}


TEST(test_tsch_sf_link_ie)
{
	uint8_t dest_short[] = { 0x12, 0x34 };
	uint8_t src_short[]  = { 0x56, 0x78 };

	Ieee154_IE ie, nie;

	memset(ieee154_frame_data, 0, sizeof(ieee154_frame_data));
	ieee154_beacon_frame_init(&ieee154_frame, ieee154_frame_data, sizeof(ieee154_frame_data));
	ieee154_set_addr(&ieee154_frame, 0xFACE, dest_short, 2, 0xF00D, src_short, 2);

	/* Append tsch_sync_ie */
	ie = ieee154_ie_first(&ieee154_frame);
	EXPECT(ieee154_hie_append(&ie, IEEE154_HT1_IE, 0, 0));
	EXPECT(ieee154_pie_append(&ie, IEEE154_MLME_IE, 0, 0));

	nie = ieee154_nie_first(&ie);
	// EXPECT(tsch_sync_ie_append(&nie, 0x123456789A, 20));

	TschSFLinkIE link_ie;

	/* Append tsch_sync_ie */
	EXPECT(tsch_sf_link_ie_append(&nie, &link_ie));
	EXPECT(tsch_sf_desc_append  (&link_ie, 10, 5));
	EXPECT(tsch_link_info_append(&link_ie, 0, 2, TSCH_OPT_TX_LINK));
	EXPECT(tsch_link_info_append(&link_ie, 1, 3, TSCH_OPT_RX_LINK));

	EXPECT(tsch_sf_desc_append  (&link_ie, 11, 2));
	EXPECT(tsch_link_info_append(&link_ie, 10, 4, TSCH_OPT_SHARED_LINK));
	EXPECT(tsch_link_info_append(&link_ie, 11, 5, TSCH_OPT_TIMEKEEPING));
	EXPECT(tsch_link_info_append(&link_ie, 12, 6, TSCH_OPT_PRIORITY));
	ieee154_ie_finalize(&nie);

	/* Read tsch_sync_ie */
	ie = ieee154_ie_first(&ieee154_frame);	/* IEEE154_HT1_IE                     */
	ieee154_ie_next(&ie);					/* IEEE154_MLME_IE                    */
	nie = ieee154_nie_first(&ie);			/* IEEE154_TSCH_SLOTFRAME_AND_LINK_IE */

	EXPECT(tsch_sf_link_ie_read(&nie, &link_ie));
	EXPECT(2 == tsch_sf_link_ie_num_sf(&link_ie));

	EXPECT(tsch_sf_desc_read(&link_ie));
	EXPECT(10 == tsch_sf_desc_sf_handle(&link_ie));
	EXPECT(5  == tsch_sf_desc_sf_size  (&link_ie));
	EXPECT(2  == tsch_sf_desc_num_links(&link_ie));

	EXPECT(tsch_link_info_read(&link_ie));
	EXPECT(0 == tsch_link_info_timeslot(&link_ie));
	EXPECT(2 == tsch_link_info_offset  (&link_ie));
	EXPECT(TSCH_OPT_TX_LINK == tsch_link_info_options(&link_ie));

	EXPECT(tsch_link_info_read(&link_ie));
	EXPECT(1 == tsch_link_info_timeslot(&link_ie));
	EXPECT(3 == tsch_link_info_offset  (&link_ie));
	EXPECT(TSCH_OPT_RX_LINK == tsch_link_info_options(&link_ie));

	EXPECT(!tsch_link_info_read(&link_ie));

	EXPECT(tsch_sf_desc_read(&link_ie));
	EXPECT(11 == tsch_sf_desc_sf_handle(&link_ie));
	EXPECT(2  == tsch_sf_desc_sf_size  (&link_ie));
	EXPECT(3  == tsch_sf_desc_num_links(&link_ie));

	EXPECT(tsch_link_info_read(&link_ie));
	EXPECT(10 == tsch_link_info_timeslot(&link_ie));
	EXPECT(4 == tsch_link_info_offset  (&link_ie));
	EXPECT(TSCH_OPT_SHARED_LINK == tsch_link_info_options(&link_ie));

	EXPECT(tsch_link_info_read(&link_ie));
	EXPECT(11 == tsch_link_info_timeslot(&link_ie));
	EXPECT(5 == tsch_link_info_offset  (&link_ie));
	EXPECT(TSCH_OPT_TIMEKEEPING == tsch_link_info_options(&link_ie));

	EXPECT(tsch_link_info_read(&link_ie));
	EXPECT(12 == tsch_link_info_timeslot(&link_ie));
	EXPECT(6 == tsch_link_info_offset  (&link_ie));
	EXPECT(TSCH_OPT_PRIORITY == tsch_link_info_options(&link_ie));

	EXPECT(!tsch_link_info_read(&link_ie));
	EXPECT(!tsch_sf_desc_read(&link_ie));
}


void test_ieee_802_15_4()
{
	memset(ieee154_frame_data, 0, sizeof(ieee154_frame_data));
	tharness_run(test_ieee154_beacon_frame_init);
	tharness_run(test_ieee154_set_ack_request);
	tharness_run(test_ieee154_set_seqnum);
	tharness_run(test_ieee154_set_addr);
	tharness_run(test_ieee154_header_ie);
	tharness_run(test_ieee154_payload_ie);
	tharness_run(test_ieee154_nested_ie);
	tharness_run(test_ieee154_payload);
	tharness_run(test_ieee154_read);
	tharness_run(test_tsch_sync_ie);
	tharness_run(test_tsch_sf_link_ie);
}


/******************************************* END OF FILE *******************************************/
