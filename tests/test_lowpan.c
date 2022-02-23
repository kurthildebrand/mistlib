/************************************************************************************************//**
* @file		test_lowpan.c
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

#include "test_lowpan.h"

#include "lowpan.h"
#include "tharness.h"


/* Private Functions ----------------------------------------------------------------------------- */
static void hexdump(const void* data, unsigned len, unsigned linelen);


/* Private Variables ----------------------------------------------------------------------------- */
      Ieee154_Frame lowpan_frame;
      uint8_t  lowpan_frame_data[127];
const uint8_t  ieee154_src[8]  = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
const uint8_t  ieee154_dest[8] = { 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
      uint8_t  lowpan_packet_data[1280];
      IPPacket lowpan_packet;
      uint8_t  received_data[1280];
      IPPacket receive_packet;

IPAddress lowpan_context1 = { .data = {
	0x00, 0x10, 0x20, 0x30,
	0x40, 0x50, 0x60, 0x70,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
}};

uint8_t lowpan_data1[]  = { 0x11, 0x11, 0x11 };
uint8_t lowpan_data2[]  = { 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22 };
uint8_t lowpan_data3[]  = { 0x33, 0x33, 0x33, 0x33, 0x33 };
uint8_t lowpan_data4[]  = { 0x44, 0x44, 0x44 };
uint8_t lowpan_data5[]  = { 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
uint8_t lowpan_data6[]  = { 0x66, 0x66, 0x66, 0x66 };
uint8_t lowpan_data7[]  = { 0x77, 0x77, 0x77, 0x77 };
uint8_t lowpan_data8[]  = { 0x88, 0x88, 0x88, 0x88, 0x88 };
uint8_t lowpan_data9[]  = { 0x99, 0x99 };
uint8_t lowpan_data10[] = { 0xAA, 0xAA, 0xAA };


// =============================================================================================== //
// Test Lowpan Context                                                                             //
// =============================================================================================== //
TEST(test_lowpan_ctx_init)
{
	const IPAddress link_local = { .data = { 0xFE, 0x80 }};

	lowpan_ctx_init();

	const IPAddress* addr = lowpan_ctx_search_id(0);

	EXPECT(addr != 0);
	EXPECT(memcmp(&link_local, addr, sizeof(IPAddress)) == 0);
}


TEST(test_lowpan_ctx_put)
{
	unsigned i;

	IPAddress addrs[16];

	/* Initialize the addresses (0x11, 0x22, 0x33, ... 0xFF) */
	for(i = 0; i < sizeof(addrs) / sizeof(addrs[0]); i++)
	{
		uint8_t val = (i << 8) | (i << 0);
		memset(addrs[i].data, val, sizeof(IPAddress));
	}

	EXPECT(!lowpan_ctx_put(0, &addrs[0]), "Expected ctx zero already set.");

	for(i = 1; i < 16; i++)
	{
		EXPECT(lowpan_ctx_put(i, &addrs[i]));
	}

	EXPECT(lowpan_ctx_count() == 16);
}


TEST(test_lowpan_ctx_search_id)
{
	unsigned i;

	IPAddress  addrs[16];
	IPAddress  link_local = { .data = { 0xFE, 0x80 }};

	/* Initialize the addresses (0x11, 0x22, 0x33, ... 0xFF) */
	for(i = 0; i < sizeof(addrs) / sizeof(addrs[0]); i++)
	{
		uint8_t val = (i << 8) | (i << 0);
		memset(addrs[i].data, val, sizeof(IPAddress));
	}

	/* Search via IDs */
	const IPAddress* addr = lowpan_ctx_search_id(0);
	EXPECT(addr);
	EXPECT(memcmp(link_local.data, addr->data, sizeof(IPAddress)) == 0);

	for(i = 1; i < sizeof(addrs) / sizeof(addrs[0]); i++)
	{
		addr = lowpan_ctx_search_id(i);
		EXPECT(addr);
		EXPECT(memcmp(addrs[i].data, addr->data, sizeof(IPAddress)) == 0);
	}

	addr = lowpan_ctx_search_id(16);
	EXPECT(!addr);
}


TEST(test_lowpan_ctx_search_addr)
{
	unsigned i;

	IPAddress addrs[16];
	IPAddress link_local = { .data = { 0xFE, 0x80 }};
	IPAddress not_found  = { .data = {
		0x00, 0x11, 0x22, 0x33,
		0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB,
		0xCC, 0xDD, 0xEE, 0xFF,
	}};

	/* Initialize the addresses (0x11, 0x22, 0x33, ... 0xFF) */
	for(i = 0; i < sizeof(addrs) / sizeof(addrs[0]); i++)
	{
		uint8_t val = (i << 8) | (i << 0);
		memset(addrs[i].data, val, sizeof(IPAddress));
	}

	/* Search via address */
	Key ctx = lowpan_ctx_search_addr(&link_local, 0, 2);
	EXPECT(ctx == 0);

	for(i = 1; i < sizeof(addrs) / sizeof(addrs[0]); i++)
	{
		ctx = lowpan_ctx_search_addr(&addrs[i], 0, 16);
		EXPECT((unsigned)ctx == i);
	}

	ctx = lowpan_ctx_search_addr(&not_found, 0, 16);
	EXPECT(ctx == -1);
}


TEST(test_lowpan_ctx_remove)
{
	IPAddress link_local = { .data = { 0xFE, 0x80 }};

	unsigned i;
	for(i = 0; i < 16; i++)
	{
		EXPECT(lowpan_ctx_remove(i));
	}

	EXPECT( lowpan_ctx_count() == 0);
	EXPECT(!lowpan_ctx_search_id(0));
	EXPECT( lowpan_ctx_search_addr(&link_local, 0, 16) == -1);
}





// =============================================================================================== //
// Test Lowpan                                                                                     //
// =============================================================================================== //
void test_lowpan_init(Ieee154_Frame* f)
{
	ieee154_data_frame_init(f, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr(f, 0, ieee154_dest, 8, 0, ieee154_src, 8);
}

/* LOWPAN_IPHC
* 	  0                                       1
* 	  0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5
* 	+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
* 	| 0 | 1 | 1 |  TF   |NH | HLIM  |CID|SAC|  SAM  | M |DAC|  DAM  |
* 	+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
* 	|      SCI      |      DCI      |
* 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* 	|ECN|   DSCP    |  rsv  |             Flow Label                |
* 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* 	|          Next Header          |           Hop Limit           |
* 	+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
* 	|                        Source Address                         |
* 	+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
* 	|                      Destination Address                      |
* 	+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+ */


/* Note: 6LOWPAN header start at index 18 in the ieee 802.15.4 frame. */
TEST(test_lowpan_tcfl_none)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,0x55,
		0x66,0x66,0x77,0x77,0x88,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0xCC,0xCC,
		0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);
	ipv6_finalize         (&lowpan_packet);

	test_lowpan_init      (&lowpan_frame);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);
}


TEST(test_lowpan_tcfl_tf_tc1)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x70,0x00,0x40,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,
		0x55,0x66,0x66,0x77,0x77,0x88,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0xCC,
		0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0x01);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);
	ipv6_finalize         (&lowpan_packet);

	test_lowpan_init      (&lowpan_frame);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);
}


TEST(test_lowpan_tcfl_tf_tc2)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x70,0x00,0x20,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,
		0x55,0x66,0x66,0x77,0x77,0x88,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0xCC,
		0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0x80);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	test_lowpan_init      (&lowpan_frame);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);
}


TEST(test_lowpan_tcfl_tf_fl1)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x68,0x00,0x00,0x00,0x0C,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,
		0x44,0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,
		0x44,0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 12);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	test_lowpan_init      (&lowpan_frame);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);
}


TEST(test_lowpan_tcfl_tf_fl2)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x68,0x00,0x40,0x00,0x18,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,
		0x44,0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,
		0x44,0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0x01);
	ipv6_set_flow_label   (&lowpan_packet, 24);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	test_lowpan_init      (&lowpan_frame);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);
}


TEST(test_lowpan_tcfl_tf_tc_fl1)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x60,0x00,0x20,0x00,0x00,0x24,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,
		0x44,0x44,0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,
		0x44,0x44,0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0x80);
	ipv6_set_flow_label   (&lowpan_packet, 36);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	test_lowpan_init      (&lowpan_frame);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);
}


TEST(test_lowpan_tcfl_tf_tc_fl2)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x60,0x00,0x60,0x00,0x00,0x24,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,
		0x44,0x44,0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,
		0x44,0x44,0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0x81);
	ipv6_set_flow_label   (&lowpan_packet, 36);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	test_lowpan_init      (&lowpan_frame);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);
}


TEST(test_lowpan_src_64bit_link_local)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x10,0x3B,0x16,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x20,0x01,
		0x0D,0xB8,0x33,0x33,0x44,0x44,0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: fe80::1122:3344:5566:7788
	 *     Destination Address: 2001:db8:3333:4444:cccc:dddd:eeee:ffff
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const IPAddress src = { .data = {
		0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	test_lowpan_init      (&lowpan_frame);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_src_64bit_link_local\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_src_16bit_link_local)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x20,0x3B,0x16,0x11,0x22,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: fe80::ff:fe00:1122
	 *     Destination Address: 2001:db8:3333:4444:cccc:dddd:eeee:ffff
	*/
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x11,0x22,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const IPAddress src = { .data = {
		0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x11,0x22,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	test_lowpan_init      (&lowpan_frame);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_src_16bit_link_local\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_src_64bit_link_local_encapsulated)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x11,0x22,0x33,0x44,0x55,0x66,
		0x77,0x88,0x78,0x30,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0xCC,0xCC,
		0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: fe80::1122:3344:5566:7788
	 *     Destination Address: 2001:db8:3333:4444:cccc:dddd:eeee:ffff
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const uint8_t ieee154_addr[8] = {
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
	};

	const IPAddress src = { .data = {
		0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_addr, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_src_64bit_link_local_encapsulated\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_src_16bit_link_local_encapsulated)
{
	const uint8_t expected_frame[] = {
		0x41,0xAD,0xFA,0xCE,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x11,0x22,0x78,0x30,
		0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,
		0xFF,0xFF,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	* Internet Protocol Version 6, Src: fe80::ff:fe00:1122, Dst: 2001:db8:3333:4444:cccc:dddd:eeee:ffff
	*     0110 .... = Version: 6
	*     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	*     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	*     Payload Length: 0
	*     Next Header: No Next Header for IPv6 (59)
	*     Hop Limit: 22
	*     Source Address: fe80::ff:fe00:1122
	*     Destination Address: 2001:db8:3333:4444:cccc:dddd:eeee:ffff
	*/
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x11,0x22,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const uint8_t ieee154_addr[2] = { 0x11,0x22 };

	const IPAddress src = { .data = {
		0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x11,0x22,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0xFACE, ieee154_dest, 8, 0, ieee154_addr, 2);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_src_16bit_link_local_encapsulated\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_src_64bit_ctx)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0xD0,0x10,0x3B,0x16,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x20,
		0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 10:2030:4050:6070:1122:3344:5566:7788
	 *     Destination Address: 2001:db8:3333:4444:cccc:dddd:eeee:ffff
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const IPAddress src = { .data = {
		0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_src_64bit_ctx\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_src_16bit_ctx)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0xE0,0x10,0x3B,0x16,0x11,0x22,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,
		0x44,0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 10:2030:4050:6070:0:ff:fe00:1122
	 *     Destination Address: 2001:db8:3333:4444:cccc:dddd:eeee:ffff
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x11,0x22,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const IPAddress src = { .data = {
		0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x11,0x22,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_src_16bit_ctx\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_src_64bit_ctx_encapsulated)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x11,0x22,0x33,0x44,0x55,0x66,
		0x77,0x88,0x78,0xF0,0x10,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0xCC,
		0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 10:2030:4050:6070:1122:3344:5566:7788
	 *     Destination Address: 2001:db8:3333:4444:cccc:dddd:eeee:ffff
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const uint8_t ieee154_addr[8] = { 0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88 };

	const IPAddress src = { .data = {
		0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_addr, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_src_64bit_ctx_encapsulated\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_src_16bit_ctx_encapsulated)
{
	const uint8_t expected_frame[] = {
		0x41,0xAD,0xFA,0xCE,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x11,0x22,0x78,0xF0,
		0x10,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0xCC,0xCC,0xDD,0xDD,0xEE,
		0xEE,0xFF,0xFF,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 10:2030:4050:6070:0:ff:fe00:1122
	 *     Destination Address: 2001:db8:3333:4444:cccc:dddd:eeee:ffff
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x11,0x22,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const uint8_t ieee154_addr[2] = { 0x11,0x22 };

	const IPAddress src = { .data = {
		0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x11,0x22,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0xFACE, ieee154_dest, 8, 0, ieee154_addr, 2);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_src_16bit_ctx_encapsulated\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_src_128bit)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,0x55,
		0x66,0x66,0x77,0x77,0x88,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0xCC,0xCC,
		0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: 2001:db8:3333:4444:cccc:dddd:eeee:ffff
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_src_128bit\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_src_unspec)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x40,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0xCC,0xCC,
		0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: ::
	 *     Destination Address: 2001:db8:3333:4444:cccc:dddd:eeee:ffff
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const IPAddress src = { .data = { 0 }};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_src_unspec\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_dest_64bit_link_local)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x01,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,0x55,
		0x66,0x66,0x77,0x77,0x88,0x88,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: fe80::1122:3344:5566:7788
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_dest_64bit_link_local\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_dest_16bit_link_local)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x02,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,0x55,
		0x66,0x66,0x77,0x77,0x88,0x88,0x11,0x22,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: fe80::ff:fe00:1122
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x11,0x22,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x11,0x22,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_dest_16bit_link_local\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_dest_64bit_link_local_encapsulated)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x03,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,0x55,
		0x66,0x66,0x77,0x77,0x88,0x88,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: fe80::8877:6655:4433:2211
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,
	};

	const uint8_t ieee154_addr[8] = { 0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11, };

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_addr, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_dest_64bit_link_local_encapsulated\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_dest_16bit_link_local_encapsulated)
{
	const uint8_t expected_frame[] = {
		0x41,0xE9,0xFA,0xCE,0x22,0x11,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x78,0x03,
		0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,0x55,0x66,0x66,0x77,0x77,
		0x88,0x88,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: fe80::8877:6655:4433:2211
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x22,0x11,
	};

	const uint8_t ieee154_addr[2] = { 0x22,0x11, };

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x22,0x11,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0xFACE, ieee154_addr, 2, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_dest_16bit_link_local_encapsulated\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_dest_64bit_ctx)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x85,0x01,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,
		0x55,0x66,0x66,0x77,0x77,0x88,0x88,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: 10:2030:4050:6070:1122:3344:5566:7788
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_dest_64bit_ctx\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_dest_16bit_ctx)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x86,0x01,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,
		0x55,0x66,0x66,0x77,0x77,0x88,0x88,0x11,0x22,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: 10:2030:4050:6070:0:ff:fe00:1122
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x11,0x22,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x11,0x22,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_dest_16bit_ctx\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_dest_64bit_ctx_encapsulated)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x87,0x01,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,
		0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: 10:2030:4050:6070:8877:6655:4433:2211
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,
	};

	const uint8_t ieee154_addr[8] = { 0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11, };

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_addr, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_dest_64bit_ctx_encapsulated\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_dest_16bit_ctx_encapsulated)
{
	const uint8_t expected_frame[] = {
		0x41,0xE9,0xFA,0xCE,0x22,0x11,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x78,0x87,
		0x01,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,0x55,0x66,0x66,0x77,
		0x77,0x88,0x88,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: 10:2030:4050:6070:0:ff:fe00:2211
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x22,0x11,
	};

	const uint8_t ieee154_addr[2] = { 0x22,0x11, };

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
		0x00,0x00,0x00,0xFF,0xFE,0x00,0x22,0x11,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0xFACE, ieee154_addr, 2, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_dest_16bit_ctx_encapsulated\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_dest_128bit)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,0x55,
		0x66,0x66,0x77,0x77,0x88,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0xCC,0xCC,
		0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: 2001:db8:3333:4444:cccc:dddd:eeee:ffff
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0xCC,0xCC,0xDD,0xDD,0xEE,0xEE,0xFF,0xFF,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_dest_128bit\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_dest_8bit_multicast)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x0B,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,0x55,
		0x66,0x66,0x77,0x77,0x88,0x88,0x11,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: ff02::11
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0xFF,0x02,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0xFF,0x02,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_dest_8bit_multicast\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_dest_32bit_multicast)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x0A,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,0x55,
		0x66,0x66,0x77,0x77,0x88,0x88,0x04,0x11,0x22,0x33,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: ff04::11:2233
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0xFF,0x04,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x11,0x22,0x33,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0xFF,0x04,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x11,0x22,0x33,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_dest_32bit_multicast\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_dest_40bit_multicast)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x09,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,0x55,
		0x66,0x66,0x77,0x77,0x88,0x88,0x06,0x11,0x22,0x33,0x44,0x55,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: ff06::11:2233:4455
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0xFF,0x06,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x11,0x22,0x33,0x44,0x55,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0xFF,0x06,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x11,0x22,0x33,0x44,0x55,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_dest_40bit_multicast\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_dest_128bit_multicast)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x08,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,0x55,
		0x66,0x66,0x77,0x77,0x88,0x88,0xFF,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x22,
		0x33,0x44,0x55,0x66,0x77,0x88,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: ffc0::1122:3344:5566:7788
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0xFF,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0xFF,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,
		0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_dest_128bit_multicast\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_dest_ctx_multicast)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x8C,0x01,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,0x55,
		0x55,0x66,0x66,0x77,0x77,0x88,0x88,0xD0,0x0D,0x11,0x22,0x33,0x44,
	};

	/* Frame 1: 40 bytes on wire (320 bits), 40 bytes captured (320 bits) on interface unknown, id 0
	 * Internet Protocol Version 6
	 *     0110 .... = Version: 6
	 *     .... 0000 0000 .... .... .... .... .... = Traffic Class: 0x00 (DSCP: CS0, ECN: Not-ECT)
	 *     .... 0000 0000 0000 0000 0000 = Flow Label: 0x00000
	 *     Payload Length: 0
	 *     Next Header: No Next Header for IPv6 (59)
	 *     Hop Limit: 22
	 *     Source Address: 2001:db8:3333:4444:5555:6666:7777:8888
	 *     Destination Address: ffd0:d30:4050:6070::1122:3344
	 */
	const uint8_t expected_packet[] = {
		0x60,0x00,0x00,0x00,0x00,0x00,0x3B,0x16,0x20,0x01,0x0D,0xB8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,0xFF,0xD0,0x0D,0x30,0x40,0x50,0x60,0x70,
		0x00,0x00,0x00,0x00,0x11,0x22,0x33,0x44,
	};

	const IPAddress src = { .data = {
		0x20,0x01,0x0d,0xb8,0x33,0x33,0x44,0x44,
		0x55,0x55,0x66,0x66,0x77,0x77,0x88,0x88,
	}};

	const IPAddress dest = { .data = {
		0xFF,0xD0,0x0D,0x30,0x40,0x50,0x60,0x70,
		0x00,0x00,0x00,0x00,0x11,0x22,0x33,0x44,
	}};

	ipv6_init             (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear            (&lowpan_packet);
	ipv6_set_traffic_class(&lowpan_packet, 0);
	ipv6_set_flow_label   (&lowpan_packet, 0);
	ipv6_set_hop_limit    (&lowpan_packet, 22);
	ipv6_set_src          (&lowpan_packet, &src);
	ipv6_set_dest         (&lowpan_packet, &dest);

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init(&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, expected_packet, 40) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 40);

	// printf("test_lowpan_dest_ctx_multicast\n");
	// hexdump(lowpan_packet_data, ipv6_length(&lowpan_packet), 16);
}


TEST(test_lowpan_compress_without_eh)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x68,0x08,0x0D,0xF0,0x82,0x3A,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,0x66,
		0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,
		0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0x86,0x00,0x6E,0xD0,0x40,0x00,0x00,
		0x1E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x40,0xE0,0x00,0x01,0x51,
		0x80,0x00,0x00,0x38,0x40,0x00,0x00,0x00,0x00,0xFD,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	};

	uint8_t orig[256] = {
		0x60,0x0D,0xF0,0x82,0x00,0x30,0x3A,0xFF,
		0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
		0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
		0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,
		0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
		0x86,0x00,0x6E,0xD0,0x40,0x00,0x00,0x1E,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x03,0x04,0x40,0xE0,0x00,0x01,0x51,0x80,
		0x00,0x00,0x38,0x40,0x00,0x00,0x00,0x00,
		0xFD,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	};

	// hexdump(orig, 88, 16);

	ipv6_init(&lowpan_packet, orig, 88, sizeof(orig));

	/* Todo: deal with fragments in ipv6_init */
	memset(&lowpan_packet.fragments, 0, sizeof(lowpan_packet.fragments));

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr      (&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress       (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, orig, 88) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 88);

	// hexdump(lowpan_packet_data, 88, 16);
}


TEST(test_lowpan_compress_with_eh)
{
	const uint8_t expected_frame[] = {
		0x41,0xED,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,0x55,
		0x66,0x77,0x78,0x08,0x00,0x01,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,
		0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,
		0x55,0x44,0x33,0x22,0x11,0x00,0x3A,0x00,0x05,0x02,0x00,0x00,0x01,0x00,0x8F,0x00,
		0x6D,0xA3,0x00,0x00,0x00,0x01,0x04,0x00,0x00,0x00,0xFF,0x02,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,
	};

	uint8_t orig[256] = {
		0x60,0x00,0x00,0x00,0x00,0x24,0x00,0x01,
		0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
		0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
		0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,
		0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
		0x3A,0x00,0x05,0x02,0x00,0x00,0x01,0x00,
		0x8F,0x00,0x6D,0xA3,0x00,0x00,0x00,0x01,
		0x04,0x00,0x00,0x00,0xFF,0x02,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x02,
	};

	ipv6_init(&lowpan_packet, orig, 76, sizeof(orig));

	/* Todo: deal with fragments in ipv6_init */
	memset(&lowpan_packet.fragments, 0, sizeof(lowpan_packet.fragments));

	ieee154_data_frame_init(&lowpan_frame, lowpan_frame_data, sizeof(lowpan_frame_data));
	ieee154_set_addr(&lowpan_frame, 0, ieee154_dest, 8, 0, ieee154_src, 8);
	lowpan_compress (&lowpan_packet, &lowpan_frame);

	EXPECT(ieee154_length(&lowpan_frame) == sizeof(expected_frame));
	EXPECT(memcmp(expected_frame, lowpan_frame_data, ieee154_length(&lowpan_frame)) == 0);

	// hexdump(lowpan_frame_data, ieee154_length(&lowpan_frame), 16);

	ipv6_init (&lowpan_packet, lowpan_packet_data, 0, sizeof(lowpan_packet_data));
	ipv6_clear(&lowpan_packet);

	lowpan_decompress(&lowpan_packet, &lowpan_frame);

	EXPECT(memcmp(lowpan_packet_data, orig, 76) == 0);
	EXPECT(ipv6_length(&lowpan_packet) == 76);

	// hexdump(lowpan_packet_data, 76, 16);
}



// TEST(test_lowpan_compress_small)
// {
// 	IPAddress src = { .data = {
// 		0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
// 		0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
// 	}};

// 	IPAddress dest = { .data = {
// 		0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
// 		0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
// 	}};

// 	uint8_t small_text[] = "Hello world!";
// 	float   coords[4] = { 1.0f, 2.0f, 3.0f, 4.0f };

// 	test_lowpan_init(&frame);

// 	ipv6_init(&lowpan_packet, lowpan_packet_data, sizeof(lowpan_packet_data));
// 	key_set(&lowpan_packet, 0x1234);
// 	ipv6_set_hop_limit(&lowpan_packet, 23);
// 	ipv6_set_src(&lowpan_packet, &src);
// 	ipv6_set_dest(&lowpan_packet, &dest);

// 	IPv6_Header header = ipv6_header_start(&lowpan_packet);
// 	ipv6_append_header(&header, IPV6_HBH, 0, 0);

// 	IPOption option = IPOption_first(&header);

// 	ipv6_append_option(&option, 0xDD, coords, sizeof(coords), 8);
// 	ipv6_append_header(&header, IPV6_UDP, small_text, sizeof(small_text));
// 	ipv6_finalize(&lowpan_packet);

// 	lowpan_compress(&lowpan_packet, &frame);

// 	/* Simulate transmitting the frame */
// 	ieee154_finalize(&frame);

// 	Lowpan lowpan = lowpan_first(&frame);

// 	while(lowpan_is_valid(&lowpan))
// 	{
// 		if(lowpan_is_iphc(&lowpan))
// 		{
// 			IPv6_Header h = lowpan_iphc_header_start(&lowpan);

// 			while(ipv6_header_is_valid(&h) && ipv6_header_type(&h) != IPV6_UDP)
// 			{
// 				ipv6_header_next(&h);
// 			}

// 			if(ipv6_header_is_valid(&h))
// 			{
// 				Range range = ipv6_header_content(&h);
// 				EXPECT(memcmp(range_entry(&range, range_start(&range)), small_text, sizeof(small_text)) == 0);
// 				PRINT("%s\n", range_entry(&range, range_start(&range)));
// 			}
// 		}

// 		lowpan_next(&lowpan);
// 	}
// }


// TEST(test_lowpan_decompress_small)
// {
// 	IPAddress src = { .data = {
// 		0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
// 		0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
// 	}};

// 	IPAddress dest = { .data = {
// 		0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
// 		0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
// 	}};

// 	uint8_t small_text[] = "Hello world!";
// 	float   coords[4];

// 	memset(lowpan_packet_data, 0, sizeof(lowpan_packet_data));
// 	ipv6_init(&lowpan_packet, lowpan_packet_data, sizeof(lowpan_packet_data));

// 	/* Simulate receiving the frame */
// 	ieee154_receive(&frame, &frame.data, ieee154_length(&frame));

// 	unsigned size = lowpan_decompressed_size(&frame);

// 	EXPECT(size == 77);

// 	ipv6_preload(&lowpan_packet, size);

// 	Lowpan lowpan = lowpan_first(&frame);

// 	EXPECT(lowpan_is_valid(&lowpan));
// 	EXPECT(lowpan_is_iphc(&lowpan));
// 	EXPECT(lowpan_decompress(&lowpan_packet, &frame) == 77);

// 	EXPECT(ipv6_version(&lowpan_packet) == 6);
// 	EXPECT(ipv6_traffic_class(&lowpan_packet) == 0);
// 	EXPECT(ipv6_flow_label(&lowpan_packet) == 0);
// 	EXPECT(ipv6_next_header(&lowpan_packet) == IPV6_HBH);
// 	EXPECT(ipv6_hop_limit(&lowpan_packet) == 23);
// 	EXPECT(memcmp(&src, ipv6_src(&lowpan_packet), sizeof(IPAddress)) == 0);
// 	EXPECT(memcmp(&dest, ipv6_dest(&lowpan_packet), sizeof(IPAddress)) == 0);

// 	IPv6_Header header = ipv6_header_start(&lowpan_packet);
// 	EXPECT(ipv6_header_type(&header) == IPV6_HBH);

// 	IPOption option = IPOption_first(&header);
// 	EXPECT(IPOption_type(&option) == IPV6_OPT_TYPE_PADN);
// 	EXPECT(IPOption_length_content(&option) == 2);
// 	EXPECT(IPOption_next(&option));
// 	EXPECT(IPOption_type(&option) == 0xDD);
// 	EXPECT(IPOption_length_content(&option) == 4*4);

// 	Range range = IPOption_content(&option);
// 	EXPECT(range_count(&range) == 4 * 4);
// 	EXPECT(range_get_many(&range, coords, range_start(&range), range_count(&range)));
// 	EXPECT(coords[0] == 1.0f);
// 	EXPECT(coords[1] == 2.0f);
// 	EXPECT(coords[2] == 3.0f);
// 	EXPECT(coords[3] == 4.0f);

// 	EXPECT(ipv6_header_next(&header));
// 	EXPECT(ipv6_header_type(&header) == IPV6_UDP);
// 	range = ipv6_header_content(&header);
// 	EXPECT(memcmp(range_entry(&range, range_start(&range)), small_text, sizeof(small_text)) == 0);
// 	PRINT("%s\n", range_entry(&range, range_start(&range)));
// }


// TEST(test_lowpan_compress_large)
// {
// 	IPAddress src = { .data = {
// 		0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
// 		0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
// 	}};

// 	IPAddress dest = { .data = {
// 		0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
// 		0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
// 	}};

// 	uint8_t large_text[] = "The Lord is my shepherd;\n"\
// 		"I shall not want.\n"\
// 		"He makes me to lie down in green pastures;\n"\
// 		"He leads me beside the still waters.\n"\
// 		"He restores my soul;\n"\
// 		"He leads me in the paths of righteousness\n"\
// 		"For His name's sake.\n\n"\
// 		"Yea, though I walk through the valley of the shadow of death,\n"\
// 		"I will fear no evil;\n"\
// 		"For You are with me;\n\n"\
// 		"Your rod and Your staff, they comfort me.\n"\
// 		"You prepare a table before me in the presence of my enemies;\n"\
// 		"You anoint my head with oil;\n"\
// 		"My cup runs over.\n"\
// 		"Surely goodness and mercy shall follow me\n"\
// 		"All the days of my life;\n"\
// 		"And I will dwell in the house of the Lord\n"\
// 		"Forever. --Psalms 21";

// 	float coords[4] = { 1.0f, 2.0f, 3.0f, 4.0f };

// 	test_lowpan_init(&frame);

// 	ipv6_init(&lowpan_packet, lowpan_packet_data, sizeof(lowpan_packet_data));
// 	key_set(&lowpan_packet, 0x1234);
// 	ipv6_set_hop_limit(&lowpan_packet, 21);
// 	ipv6_set_src(&lowpan_packet, &src);
// 	ipv6_set_dest(&lowpan_packet, &dest);

// 	IPv6_Header header = ipv6_header_start(&lowpan_packet);
// 	ipv6_append_header(&header, IPV6_HBH, 0, 0);

// 	IPOption option = IPOption_first(&header);

// 	ipv6_append_option(&option, 0xDD, coords, sizeof(coords), 8);
// 	ipv6_append_header(&header, IPV6_UDP, large_text, sizeof(large_text));
// 	ipv6_finalize(&lowpan_packet);

// 	unsigned length = lowpan_compress(&lowpan_packet, &frame);

// 	/* Simulate transmitting the frame */
// 	ieee154_finalize(&frame);

// 	/* Simulate receiving the frame */
// 	ieee154_receive(&frame, &frame.data, ieee154_length(&frame));

// 	unsigned size = lowpan_decompressed_size(&frame);

// 	EXPECT(size == ipv6_length(&lowpan_packet));

// 	memset(received_data, 0, sizeof(received_data));
// 	ipv6_init(&receive_packet, received_data, size);

// 	/* Simulate receiving the first fragmented lowpan frame */
// 	Lowpan lowpan = lowpan_first(&frame);
// 	EXPECT(lowpan_type(&lowpan) == LOWPAN_FRAG);
// 	key_set(&receive_packet, lowpan_frag_tag(&lowpan));
// 	ipv6_preload(&receive_packet, size);

// 	EXPECT(lowpan_decompress(&receive_packet, &frame) == length);

// 	unsigned decomp_length;

// 	/* Simulate receiving the rest of the fragmented lowpan frames. */
// 	while(length != ipv6_length(&lowpan_packet))
// 	{
// 		test_lowpan_init(&frame);
// 		length = lowpan_compress(&lowpan_packet, &frame);
// 		decomp_length = lowpan_decompress(&receive_packet, &frame);
// 		EXPECT(decomp_length == length);
// 	}

// 	EXPECT(ipv6_version(&receive_packet) == 6);
// 	EXPECT(ipv6_traffic_class(&receive_packet) == 0);
// 	EXPECT(ipv6_flow_label(&receive_packet) == 0);
// 	EXPECT(ipv6_next_header(&receive_packet) == IPV6_HBH);
// 	EXPECT(ipv6_hop_limit(&receive_packet) == 21);
// 	EXPECT(memcmp(&src, ipv6_src(&receive_packet), sizeof(IPAddress)) == 0);
// 	EXPECT(memcmp(&dest, ipv6_dest(&receive_packet), sizeof(IPAddress)) == 0);

// 	header = ipv6_header_start(&receive_packet);
// 	EXPECT(ipv6_header_type(&header) == IPV6_HBH);

// 	option = IPOption_first(&header);
// 	EXPECT(IPOption_type(&option) == IPV6_OPT_TYPE_PADN);
// 	EXPECT(IPOption_length_content(&option) == 2);
// 	EXPECT(IPOption_next(&option));
// 	EXPECT(IPOption_type(&option) == 0xDD);
// 	EXPECT(IPOption_length_content(&option) == 4*4);

// 	Range range = IPOption_content(&option);
// 	EXPECT(range_count(&range) == 4 * 4);
// 	EXPECT(range_get_many(&range, coords, range_start(&range), range_count(&range)));
// 	EXPECT(coords[0] == 1.0f);
// 	EXPECT(coords[1] == 2.0f);
// 	EXPECT(coords[2] == 3.0f);
// 	EXPECT(coords[3] == 4.0f);

// 	EXPECT(ipv6_header_next(&header));
// 	EXPECT(ipv6_header_type(&header) == IPV6_UDP);
// 	range = ipv6_header_content(&header);
// 	EXPECT(memcmp(range_entry(&range, range_start(&range)), large_text, sizeof(large_text)) == 0);
// 	PRINT("%s\n", range_entry(&range, range_start(&range)));
// }


void test_lowpan(void)
{
	memset(lowpan_frame_data, 0, sizeof(lowpan_frame_data));

	tharness_run(test_lowpan_ctx_init);
	tharness_run(test_lowpan_ctx_put);
	tharness_run(test_lowpan_ctx_search_id);
	tharness_run(test_lowpan_ctx_search_addr);
	tharness_run(test_lowpan_ctx_remove);

	test_lowpan_ctx_init();
	test_lowpan_init(&lowpan_frame);
	lowpan_ctx_put(1, &lowpan_context1);

	tharness_run(test_lowpan_tcfl_none);
	tharness_run(test_lowpan_tcfl_tf_tc1);
	tharness_run(test_lowpan_tcfl_tf_tc2);
	tharness_run(test_lowpan_tcfl_tf_fl1);
	tharness_run(test_lowpan_tcfl_tf_fl2);
	tharness_run(test_lowpan_tcfl_tf_tc_fl1);
	tharness_run(test_lowpan_tcfl_tf_tc_fl2);

	tharness_run(test_lowpan_src_64bit_link_local);
	tharness_run(test_lowpan_src_16bit_link_local);
	tharness_run(test_lowpan_src_64bit_link_local_encapsulated);
	tharness_run(test_lowpan_src_16bit_link_local_encapsulated);
	tharness_run(test_lowpan_src_64bit_ctx);
	tharness_run(test_lowpan_src_16bit_ctx);
	tharness_run(test_lowpan_src_64bit_ctx_encapsulated);
	tharness_run(test_lowpan_src_16bit_ctx_encapsulated);
	tharness_run(test_lowpan_src_128bit);
	tharness_run(test_lowpan_src_unspec);
	tharness_run(test_lowpan_dest_64bit_link_local);
	tharness_run(test_lowpan_dest_16bit_link_local);
	tharness_run(test_lowpan_dest_64bit_link_local_encapsulated);
	tharness_run(test_lowpan_dest_16bit_link_local_encapsulated);
	tharness_run(test_lowpan_dest_64bit_ctx);
	tharness_run(test_lowpan_dest_16bit_ctx);
	tharness_run(test_lowpan_dest_64bit_ctx_encapsulated);
	tharness_run(test_lowpan_dest_16bit_ctx_encapsulated);
	tharness_run(test_lowpan_dest_128bit);
	tharness_run(test_lowpan_dest_8bit_multicast);
	tharness_run(test_lowpan_dest_32bit_multicast);
	tharness_run(test_lowpan_dest_40bit_multicast);
	tharness_run(test_lowpan_dest_128bit_multicast);
	tharness_run(test_lowpan_dest_ctx_multicast);

	tharness_run(test_lowpan_compress_without_eh);
	tharness_run(test_lowpan_compress_with_eh);

	/* Todo: test compression of large packets that require fragmentation */

	// tharness_run(test_lowpan_compress_small);
	// tharness_run(test_lowpan_decompress_small);
	// tharness_run(test_lowpan_compress_large);
}

static void hexdump(const void* data, unsigned len, unsigned linelen)
{
	const uint8_t* ptr = data;

	unsigned i,j;

	for(i = 0; i < len; i += linelen)
	{
		/* Address */
		printf("%04X ", i);

		/* One line of bytes */
		for(j = 0; j < linelen; j++)
		{
			if(i + j >= len) {
				printf("   ");
			} else {
				printf("%02X ", ptr[i+j]);
			}
		}

		/* Character representation of bytes */
		for(j = 0; j < linelen; j++)
		{
			if(i + j >= len) {
				break;
			} else if(ptr[i+j] < ' ' || ptr[i+j] > '~') {
				printf(".");
			} else {
				printf("%c", ptr[i+j]);
			}
		}

		printf("\n");
	}
}


/******************************************* END OF FILE *******************************************/
