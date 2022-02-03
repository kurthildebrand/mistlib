/************************************************************************************************//**
 * @file		test_ndp.c
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
#include "test_ndp.h"

#include "ndp.h"
#include "tharness.h"


/* Inline Function Instances --------------------------------------------------------------------- */
/* Private Macros -------------------------------------------------------------------------------- */
/* Private Types --------------------------------------------------------------------------------- */
/* Private Functions ----------------------------------------------------------------------------- */
static void hexdump(const void* data, unsigned len, unsigned linelen);


/* Private Variables ----------------------------------------------------------------------------- */
uint8_t  ndp_packet_data[128];
uint8_t  ndp_packet_data2[256];
IPPacket ndp_packet;
IPPacket ndp_packet2;
uint8_t  ndp_sllao[8] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
uint8_t  ndp_tllao[8] = { 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

IPAddress ndp_src = { .data =
	{0x00, 0x11, 0x22, 0x33,
	 0x44, 0x55, 0x66, 0x77,
	 0x88, 0x99, 0xAA, 0xBB,
	 0xCC, 0xDD, 0xEE, 0xFF } };

IPAddress ndp_dest = { .data = {
	0xFF, 0xEE, 0xDD, 0xCC,
	0xBB, 0xAA, 0x99, 0x88,
	0x77, 0x66, 0x55, 0x44,
	0x33, 0x22, 0x11, 0x00 }};


TEST(test_ndp_rs)
{
	const uint8_t expected1[] = {
		0x60,0x00,0x00,0x00,0x00,0x08,0x3A,0x40,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
		0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,
		0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0x85,0x00,0x7A,0xBD,0x00,0x00,0x00,0x00,
	};

	const uint8_t expected2[] = {
		0x60,0x00,0x00,0x00,0x00,0x18,0x3A,0x40,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
		0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,
		0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0x85,0x00,0xAC,0x9A,0x00,0x00,0x00,0x00,
		0x01,0x02,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x00,0x00,0x00,0x00,0x00,0x00,
	};

	ipv6_init         (&ndp_packet, ndp_packet_data, 0, sizeof(ndp_packet_data));
	ipv6_clear        (&ndp_packet);
	ipv6_set_src      (&ndp_packet, &ndp_src);
	ipv6_set_dest     (&ndp_packet, &ndp_dest);
	ipv6_set_hop_limit(&ndp_packet, 64);

	IPExthdr eh = ipv6_eh_first(&ndp_packet);
	EXPECT(ndp_append_rs(&eh));
	icmpv6_finalize(&eh);

	EXPECT(memcmp(expected1, ndp_packet_data, sizeof(expected1)) == 0);

	// hexdump(ndp_packet_data, ipv6_length(&ndp_packet), 16);

	IPOption opt = ndp_rs_opt_first(&eh);
	EXPECT(ndp_opt_append_sllao(&opt, ndp_sllao, 8));
	ipv6_opt_finalize(&opt);
	icmpv6_finalize(&eh);

	EXPECT(memcmp(expected2, ndp_packet_data, sizeof(expected2)) == 0);

	// hexdump(ndp_packet_data, ipv6_length(&ndp_packet), 16);
}


TEST(test_ndp_ra)
{
	const uint8_t expected1[] = {
		0x60,0x00,0x00,0x00,0x00,0x10,0x3A,0x40,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
		0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,
		0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0x86,0x00,0x4D,0x30,0x37,0x80,0x0E,0x10,
		0x00,0xBC,0x61,0x4E,0x05,0x39,0x7F,0xB1,
	};

	const uint8_t expected2[] = {
		0x60,0x00,0x00,0x00,0x00,0x38,0x3A,0x40,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
		0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,
		0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0x86,0x00,0x58,0x3E,0x37,0x80,0x0E,0x10,
		0x00,0xBC,0x61,0x4E,0x05,0x39,0x7F,0xB1,0x03,0x04,0x14,0x40,0x00,0x00,0x0E,0x10,
		0x00,0x00,0x09,0x60,0x00,0x00,0x00,0x00,0xFD,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
		0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x05,0x01,0x00,0x00,0x00,0x00,0x07,0xD0,
	};

	const IPAddress prefix = { .data = {
		0xFD, 0x11, 0x22, 0x33,
		0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB,
		0xCC, 0xDD, 0xEE, 0xFF,
	}};

	NdpRA ra_fields;
	ra_fields.cur_hop_limit   = 55;
	ra_fields.flags           = ICMPV6_NDP_RA_M;
	ra_fields.router_lifetime = 3600;
	ra_fields.reachable_time  = 12345678;
	ra_fields.retrans_timer   = 87654321;

	ipv6_init         (&ndp_packet, ndp_packet_data, 0, sizeof(ndp_packet_data));
	ipv6_clear        (&ndp_packet);
	ipv6_set_src      (&ndp_packet, &ndp_src);
	ipv6_set_dest     (&ndp_packet, &ndp_dest);
	ipv6_set_hop_limit(&ndp_packet, 64);

	IPExthdr eh = ipv6_eh_first(&ndp_packet);
	EXPECT(ndp_append_ra(&eh, &ra_fields));
	icmpv6_finalize(&eh);

	EXPECT(55              == ndp_ra_hop_limit(&eh));
	EXPECT(ICMPV6_NDP_RA_M == ndp_ra_flags(&eh));
	EXPECT(3600            == ndp_ra_router_life(&eh));
	EXPECT(12345678        == ndp_ra_reachable_time(&eh));
	EXPECT(87654321        == ndp_ra_retrans_time(&eh));

	EXPECT(memcmp(expected1, ndp_packet_data, sizeof(expected1)) == 0);

	// hexdump(ndp_packet_data, ipv6_length(&ndp_packet), 16);

	NdpPrefix prefix_fields;
	prefix_fields.prefix_length      = 20;
	prefix_fields.flags              = ICMPV6_NDP_PREFIX_A;
	prefix_fields.valid_lifetime     = 3600;
	prefix_fields.preferred_lifetime = 2400;
	prefix_fields.prefix             = &prefix;

	IPOption opt = ndp_ra_opt_first(&eh);
	EXPECT(ndp_opt_append_prefix(&opt, &prefix_fields));
	ipv6_opt_finalize(&opt);

	EXPECT(ndp_opt_append_mtu(&opt, 2000));
	ipv6_opt_finalize(&opt);

	icmpv6_finalize(&eh);

	opt = ndp_ra_opt_first(&eh);

	EXPECT(20                  == ndp_opt_prefix_length(&opt));
	EXPECT(ICMPV6_NDP_PREFIX_A == ndp_opt_prefix_flags(&opt));
	EXPECT(3600                == ndp_opt_prefix_valid(&opt));
	EXPECT(2400                == ndp_opt_prefix_preferred(&opt));
	EXPECT(memcmp(&prefix, ndp_opt_prefix_addr(&opt), sizeof(IPAddress)) == 0);

	ipv6_opt_next(&opt);

	EXPECT(2000 == ndp_opt_mtu(&opt));

	EXPECT(memcmp(expected2, ndp_packet_data, sizeof(expected2)) == 0);

	// hexdump(ndp_packet_data, ipv6_length(&ndp_packet), 16);
}


TEST(test_ndp_ns)
{
	const uint8_t expected1[] = {
		0x60,0x00,0x00,0x00,0x00,0x18,0x3A,0x40,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
		0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,
		0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0x87,0x00,0x76,0xCB,0x00,0x00,0x00,0x00,
		0xFD,0x22,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
	};

	const uint8_t expected2[] = {
		0x60,0x00,0x00,0x00,0x00,0x28,0x3A,0x40,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
		0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,
		0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0x87,0x00,0xA8,0xA8,0x00,0x00,0x00,0x00,
		0xFD,0x22,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
		0x01,0x02,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x00,0x00,0x00,0x00,0x00,0x00,
	};

	const IPAddress target = { .data = {
		0xFD, 0x22, 0x01, 0x23,
		0x45, 0x67, 0x89, 0xAB,
		0xCD, 0xEF, 0xAA, 0xBB,
		0xCC, 0xDD, 0xEE, 0xFF,
	}};

	ipv6_init         (&ndp_packet, ndp_packet_data, 0, sizeof(ndp_packet_data));
	ipv6_clear        (&ndp_packet);
	ipv6_set_src      (&ndp_packet, &ndp_src);
	ipv6_set_dest     (&ndp_packet, &ndp_dest);
	ipv6_set_hop_limit(&ndp_packet, 64);

	IPExthdr eh = ipv6_eh_first(&ndp_packet);
	EXPECT(ndp_append_ns(&eh, &target));
	icmpv6_finalize(&eh);

	EXPECT(memcmp(&target, ndp_ns_target(&eh), sizeof(IPAddress)) == 0);
	EXPECT(memcmp(expected1, ndp_packet_data, sizeof(expected1)) == 0);

	// hexdump(ndp_packet_data, ipv6_length(&ndp_packet), 16);

	IPOption opt = ndp_rs_opt_first(&eh);
	EXPECT(ndp_opt_append_sllao(&opt, ndp_sllao, 8));
	ipv6_opt_finalize(&opt);
	icmpv6_finalize(&eh);

	EXPECT(memcmp(ndp_sllao, buffer_read(ndp_opt_llao(&opt)), sizeof(ndp_sllao)) == 0);
	EXPECT(memcmp(expected2, ndp_packet_data, sizeof(expected2)) == 0);

	// hexdump(ndp_packet_data, ipv6_length(&ndp_packet), 16);
}


TEST(test_ndp_na)
{
	const uint8_t expected1[] = {
		0x60,0x00,0x00,0x00,0x00,0x18,0x3A,0x40,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
		0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,
		0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0x88,0x00,0xF5,0xCA,0x80,0x00,0x00,0x00,
		0xFD,0x22,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
	};

	const uint8_t expected2[] = {
		0x60,0x00,0x00,0x00,0x00,0x28,0x3A,0x40,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
		0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,
		0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0x88,0x00,0x04,0x86,0x80,0x00,0x00,0x00,
		0xFD,0x22,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
		0x02,0x02,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
	};

	const IPAddress address = { .data = {
		0xFD, 0x22, 0x01, 0x23,
		0x45, 0x67, 0x89, 0xAB,
		0xCD, 0xEF, 0xAA, 0xBB,
		0xCC, 0xDD, 0xEE, 0xFF,
	}};

	ipv6_init         (&ndp_packet, ndp_packet_data, 0, sizeof(ndp_packet_data));
	ipv6_clear        (&ndp_packet);
	ipv6_set_src      (&ndp_packet, &ndp_src);
	ipv6_set_dest     (&ndp_packet, &ndp_dest);
	ipv6_set_hop_limit(&ndp_packet, 64);

	IPExthdr eh = ipv6_eh_first(&ndp_packet);
	EXPECT(ndp_append_na(&eh, ICMPV6_NDP_NA_R, &address));
	icmpv6_finalize(&eh);

	EXPECT(ICMPV6_NDP_NA_R == ndp_na_flags(&eh));
	EXPECT(memcmp(&address, ndp_na_target(&eh), sizeof(IPAddress)) == 0);
	EXPECT(memcmp(expected1, ndp_packet_data, sizeof(expected1)) == 0);

	// hexdump(ndp_packet_data, ipv6_length(&ndp_packet), 16);

	IPOption opt = ndp_rs_opt_first(&eh);
	EXPECT(ndp_opt_append_tllao(&opt, ndp_tllao, 8));
	ipv6_opt_finalize(&opt);
	icmpv6_finalize(&eh);

	EXPECT(memcmp(ndp_tllao, buffer_read(ndp_opt_llao(&opt)), sizeof(ndp_tllao)) == 0);
	EXPECT(memcmp(expected2, ndp_packet_data, sizeof(expected2)) == 0);

	// hexdump(ndp_packet_data, ipv6_length(&ndp_packet), 16);
}


TEST(test_ndp_redir)
{
	const uint8_t expected1[] = {
		0x60,0x00,0x00,0x00,0x00,0x28,0x3A,0x40,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
		0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,
		0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0x89,0x00,0xB8,0x93,0x00,0x00,0x00,0x00,
		0xFD,0x22,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
		0xFD,0x22,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0xEF,0xCD,0xAB,0x89,0x67,0x45,0x23,0x01,
	};

	const uint8_t expected2[] = {
		0x60,0x00,0x00,0x00,0x00,0x80,0x3A,0x40,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
		0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,
		0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0x89,0x00,0x1A,0x2A,0x00,0x00,0x00,0x00,
		0xFD,0x22,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
		0xFD,0x22,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0xEF,0xCD,0xAB,0x89,0x67,0x45,0x23,0x01,
		0x04,0x0B,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x28,0x3A,0x40,
		0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
		0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
		0x88,0x00,0x04,0x86,0x80,0x00,0x00,0x00,0xFD,0x22,0x01,0x23,0x45,0x67,0x89,0xAB,
		0xCD,0xEF,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x02,0x02,0x88,0x99,0xAA,0xBB,0xCC,0xDD,
		0xEE,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
	};

	const IPAddress target = { .data = {
		0xFD, 0x22, 0x01, 0x23,
		0x45, 0x67, 0x89, 0xAB,
		0xCD, 0xEF, 0xAA, 0xBB,
		0xCC, 0xDD, 0xEE, 0xFF,
	}};

	const IPAddress dest = { .data = {
		0xFD, 0x22, 0xFF, 0xEE,
		0xDD, 0xCC, 0xBB, 0xAA,
		0xEF, 0xCD, 0xAB, 0x89,
		0x67, 0x45, 0x23, 0x01,
	}};

	ipv6_init         (&ndp_packet2, ndp_packet_data2, 0, sizeof(ndp_packet_data2));
	ipv6_clear        (&ndp_packet2);
	ipv6_set_src      (&ndp_packet2, &ndp_src);
	ipv6_set_dest     (&ndp_packet2, &ndp_dest);
	ipv6_set_hop_limit(&ndp_packet2, 64);

	IPExthdr eh = ipv6_eh_first(&ndp_packet2);
	EXPECT(ndp_append_redir(&eh, &target, &dest));
	icmpv6_finalize(&eh);

	EXPECT(memcmp(&target, ndp_redir_target(&eh), sizeof(IPAddress)) == 0);
	EXPECT(memcmp(&dest,   ndp_redir_dest(&eh), sizeof(IPAddress)) == 0);
	EXPECT(memcmp(expected1, ndp_packet_data2, sizeof(expected1)) == 0);

	// hexdump(ndp_packet_data2, ipv6_length(&ndp_packet2), 16);

	IPOption opt = ndp_redir_opt_first(&eh);
	EXPECT(ndp_opt_append_redir(&opt, ndp_packet_data, ipv6_length(&ndp_packet)));
	ipv6_opt_finalize(&opt);
	icmpv6_finalize(&eh);

	EXPECT(memcmp(expected2, ndp_packet_data2, sizeof(expected2)) == 0);

	hexdump(ndp_packet_data2, ipv6_length(&ndp_packet2), 16);
}


void test_ndp(void)
{
	tharness_run(test_ndp_rs);
	tharness_run(test_ndp_ra);
	tharness_run(test_ndp_ns);
	tharness_run(test_ndp_na);
	tharness_run(test_ndp_redir);
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
