/************************************************************************************************//**
 * @file		test_ipv6.c
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

#include "test_ipv6.h"

#include "icmp6.h"
#include "tharness.h"


/* Private Variables ----------------------------------------------------------------------------- */
uint8_t  icmp6_packet_data[128];
uint8_t  icmp6_packet_data2[128];
IPPacket icmp6_packet;
IPPacket icmp6_packet2;

IPAddress icmp6_src = { .data =
	{0x00, 0x11, 0x22, 0x33,
	 0x44, 0x55, 0x66, 0x77,
	 0x88, 0x99, 0xAA, 0xBB,
	 0xCC, 0xDD, 0xEE, 0xFF } };

IPAddress icmp6_dest = { .data = {
	0xFF, 0xEE, 0xDD, 0xCC,
	0xBB, 0xAA, 0x99, 0x88,
	0x77, 0x66, 0x55, 0x44,
	0x33, 0x22, 0x11, 0x00 }};


TEST(test_icmpv6_append_error)
{
	ipv6_init (&icmp6_packet, icmp6_packet_data, 0, sizeof(icmp6_packet_data));
	ipv6_clear(&icmp6_packet);

	ipv6_set_src      (&icmp6_packet, &icmp6_src);
	ipv6_set_dest     (&icmp6_packet, &icmp6_dest);
	ipv6_set_hop_limit(&icmp6_packet, 64);

	IPExthdr eh = ipv6_eh_first(&icmp6_packet);

	EXPECT(icmpv6_append_error(&eh, ICMPV6_TIME_EXCEEDED, ICMPV6_TE_HOP_LIMIT_EXCEEDED, 0, 0, 0));
}


TEST(test_icmpv6_echo)
{
	ipv6_init (&icmp6_packet, icmp6_packet_data, 0, sizeof(icmp6_packet_data));
	ipv6_clear(&icmp6_packet);

	ipv6_set_src      (&icmp6_packet, &icmp6_src);
	ipv6_set_dest     (&icmp6_packet, &icmp6_dest);
	ipv6_set_hop_limit(&icmp6_packet, 64);

	IPExthdr eh = ipv6_eh_first(&icmp6_packet);

	EXPECT(icmpv6_append_echo_req(&eh, 0x1234, 0x10, 0, 0));

	icmpv6_finalize(&eh);

	ipv6_init    (&icmp6_packet2, icmp6_packet_data2, 0, sizeof(icmp6_packet_data2));
	ipv6_clear   (&icmp6_packet2);
	ipv6_set_src (&icmp6_packet, &icmp6_dest);
	ipv6_set_dest(&icmp6_packet, &icmp6_src);
	ipv6_set_hop_limit(&icmp6_packet, 64);

	IPExthdr eh2 = ipv6_eh_first(&icmp6_packet2);

	EXPECT(icmpv6_append_echo_resp(&eh2, &eh));

	icmpv6_finalize(&eh2);

	EXPECT(0x1234 == icmpv6_echo_id    (&eh2));
	EXPECT(0x10   == icmpv6_echo_seqnum(&eh2));
}


void test_icmp6()
{
	tharness_run(test_icmpv6_append_error);
	tharness_run(test_icmpv6_echo);
}


/******************************************* END OF FILE *******************************************/
