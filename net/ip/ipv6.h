/************************************************************************************************//**
 * @file		ipv6.h
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
 * @brief
 * @ref			https://tools.ietf.org/html/rfc1700
 * @ref			https://tools.ietf.org/html/rfc8200
 * @ref			https://www.iana.org/assignments/ipv6-parameters/ipv6-parameters.xhtml#ipv6-parameters-2
 * @ref			https://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
 * @TODO		Rewrite ipv6 documentation in this header.
 * @TODO		Implement a way to get header end index without padding?
 *
 * 					Probably a way to do it with the extension headers with options. Basically find
 * 					the longest stretch of PAD options at the end of the header and set header end
 * 					index to the beginning of the stretch.
 *
 * @TODO		Refactor IPExthdr_is_valid and IPOption_is_valid
 *
 * 					Exthdr should be invalid if start == end
 * 					Also, do not check if exthdr if valid before appending. Instead, do a separate
 * 					check (don't rely on is_valid for everything; use is_valid only for user-facing
 * 					code).
 *
 * @desc		IPv6 Address Organization
 *
 * 					2001:0DB8:AC10:FE01:0000:0000:0000:0000
 * 					<-----------------> <----------------->
 * 					Subnet ID           Interface Id
 *
 * 				Multicast Addresses:
 * 				For multicast addresses, the 4 lsb's identify the address scope:
 *
 * 					ff00	reserved
 * 					ff01	interface-local	scope spans only a single interface on a
 * 							node, and is useful only for loopback transmission of multicast.
 * 					ff02	link-local scope spans the same topological region as the
 * 							corresponding unicast scope.
 * 					ff03	realm-local	scope is defined as larger than link-local, automatically
 * 							determined by network topology and must not be larger than the following
 * 							scopes.
 * 					ff04	admin-local	scope is the smallest scope that must be administratively
 * 							configured, i.e., not automatically derived from physical connectivity or
 * 							other, non-multicast-related configuration.
 * 					ff05	site-local scope is intended to span a single site belonging to an
 * 							organisation.
 * 					ff08	organization-local scope is intended to span all sites belonging to a
 * 							single organization.
 * 					ff0e	global spans all reachable nodes on the internet - it is unbounded.
 * 					ff0f	reserved
 *
 ***************************************************************************************************/
#ifndef IPV6_H
#define IPV6_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "buffer.h"
#include "key.h"
#include "linked.h"


/* Public Macros --------------------------------------------------------------------------------- */
#define IPV6_MTU                 (1280u)
#define IPV6_HDR_LENGTH          (40u)
#define IPV6_ADDR_SIZE           (16u)
#define IPV6_EXT_HDR_MAX_SIZE    (2048u)
#define IPV6_OPT_MAX_SIZE        (255u)

/* Public Types ---------------------------------------------------------------------------------- */
/* @ref https://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml */
/* IPv6 Protocol Numbers */
#define IPV6_TCP                 (6)
#define IPV6_UDP                 (17)
#define IPV6_IN_IPV6             (41)
#define IPV6_RSV_PROTOCOL        (46)
#define IPV6_ICMPV6              (58)

/* IPv6 Extension Headers */
#define IPV6_HBH                 (0)
#define IPV6_ROUTING             (43)
#define IPV6_FRAG_HEADER         (44)
#define IPV6_ENCAP_SEC_PROTOCOL  (50)
#define IPV6_AUTH_HEADER         (51)
#define IPV6_NO_NEXT_HEADER      (59)
#define IPV6_DEST_OPTS_HEADER    (60)
#define IPV6_MOBILITY            (135)
#define IPV6_HOST_IDENT_PROTOCOL (139)
#define IPV6_SHIM6_PROTOCOL      (140)
#define IPV6_RES1                (253)
#define IPV6_RES2                (254)
#define IPV6_INVALID             (255)

/* TODO: verify that extension headers are in order and not repeated. */
/* Extension headers must appear in the following order (see
 * https://tools.ietf.org/html/rfc2460#section-4.1) for more details):
 *
 * 		IPv6 header
 * 		Hop-by-Hop Options header
 * 		Destination Options header
 * 		Routing header
 * 		Fragment header
 * 		Authentication header
 * 		Encapsulating Security Payload header
 * 		Destination Options header
 * 		Upper-layer Header
 */
#define IPV6_FLAG_HBH                (1 << 0)
#define IPV6_FLAG_DEST_OPT1          (1 << 1)
#define IPV6_FLAG_ROUTING            (1 << 2)
#define IPV6_FLAG_FRAG_HEADER        (1 << 3)
#define IPV6_FLAG_AUTH_HEADER        (1 << 4)
#define IPV6_FLAG_ENCAP_SEC_PROTOCOL (1 << 5)
#define IPV6_FLAG_DEST_OPT2          (1 << 6)


/* Options
 *
 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- - - - - - - - -
 * 		|  Option Type  |  Opt Data Len |  Option Data
 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- - - - - - - - -
 *
 * 		Option Type		8-bit identifier of the type of option.
 *
 * 		Opt Data Len	8-bit unsigned integer. Length of the Option Data field of this option, in
 * 						octets.
 *
 * 		Option Data		Variable-length field. Option-Type-specific data.
 *
 * Option Type:
 *
 * 		00xxxxxx --	skip over this option and continue processing the header.
 *
 * 		01xxxxxx --	discard the packet.
 *
 * 		10xxxxxx --	discard the packet and, regardless of whether or not the packet's Destination
 * 					Address was a multicast address, send an ICMP Parameter Problem, Code 2, message
 * 					to the packet's Source Address, pointing to the unrecognized Option Type.
 *
 * 		11xxxxxx --	discard the packet and, only if the packet's Destination Address was not a
 * 					multicast address, send an ICMP Parameter Problem, Code 2, message to the
 * 					packet's Source Address, pointing to the unrecognized Option Type.
 *
 * 		xx0xxxxx --	Option Data does not change en route
 *
 * 		xx1xxxxx --	Option Data may change en route
 *
 * Pad Options
 * Pad1 option (alignment requirement: none)
 *
 * 		+-+-+-+-+-+-+-+-+
 * 		|       0       |
 * 		+-+-+-+-+-+-+-+-+
 *
 * 		NOTE! the format of the Pad1 option is a special case -- it does not have length and value
 * 		fields.
 *
 * 		The Pad1 option is used to insert 1 octet of padding into the Options area of a header. If
 * 		more than one octet of padding is required, the PadN option, described next, should be used,
 * 		rather than multiple Pad1 options.
 *
 * PadN option (alignment requirement: none)
 *
 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- - - - - - - - -
 * 		|       1       |  Opt Data Len |  Option Data
 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- - - - - - - - -
 *
 * 		The PadN option is used to insert two or more octets of padding into the Options area of a
 * 		header. For N octets of padding, the Opt Data Len field contains the value N-2, and the
 * 		Option Data consists of N-2 zero-valued octets. */
#define IPV6_OPT_TYPE_PROC_SHIFT      (6)
#define IPV6_OPT_TYPE_CHANGE_SHIFT    (5)

#define IPV6_OPT_TYPE_PROC_MASK       (0x3 << IPV6_OPT_TYPE_PROC_SHIFT)
#define IPV6_OPT_TYPE_CHANGE_MASK     (0x1 << IPV6_OPT_TYPE_CHANGE_SHIFT)

#define IPV6_OPT_TYPE_PAD1            (0x0)
#define IPV6_OPT_TYPE_PADN            (0x1)
#define IPV6_OPT_TYPE_PROC_SKIP       (0x0 << IPV6_OPT_TYPE_PROC_SHIFT)
#define IPV6_OPT_TYPE_PROC_DISCARD    (0x1 << IPV6_OPT_TYPE_PROC_SHIFT)
#define IPV6_OPT_TYPE_PROC_MULTI_ICMP (0x2 << IPV6_OPT_TYPE_PROC_SHIFT)
#define IPV6_OPT_TYPE_PROC_ICMP       (0x3 << IPV6_OPT_TYPE_PROC_SHIFT)
#define IPV6_OPT_TYPE_NO_CHANGE       (0x0 << IPV6_OPT_TYPE_CHANGE_SHIFT)
#define IPV6_OPT_TYPE_CHANGE          (0x1 << IPV6_OPT_TYPE_CHANGE_SHIFT)
#define IPV6_OPT_TYPE_INVALID         (255)


/* Hex 		Binary Value 	Description 								Reference			Alignment
 * Value	act chg rest
 * 0x00 	00 	0 	00000 	Pad1 										[[IPV6]]			(1)
 * 0x01 	00 	0 	00001 	PadN 										[[IPV6]]			(1)
 * 0xC2 	11 	0 	00010 	Jumbo Payload								[RFC2675]			(4n+2)
 * 0x63 	01 	1 	00011 	RPL Option									[RFC6553]			(2n)
 * 0x04 	00 	0 	00100 	Tunnel Encapsulation Limit 					[RFC2473]
 * 0x05 	00 	0 	00101 	Router Alert 								[RFC2711]			(2n)
 * 0x26 	00 	1 	00110 	Quick-Start 								[RFC4782]
 * 0x07 	00 	0 	00111 	CALIPSO 									[RFC5570]
 * 0x08 	00 	0 	01000 	SMF_DPD 									[RFC6621]
 * 0xC9 	11 	0 	01001 	Home Address 								[RFC6275]
 * 0x8A 	10 	0 	01010 	Endpoint Identification (DEPRECATED) 		[[CHARLES LYNN]]
 * 0x8B 	10 	0 	01011 	ILNP Nonce 									[RFC6744]
 * 0x8C 	10 	0 	01100 	Line-Identification Option 					[RFC6788]
 * 0x4D 	01 	0 	01101 	Deprecated 									[RFC7731]
 * 0x6D 	01 	1 	01101 	MPL Option 									[RFC7731]
 * 0xEE 	11 	1 	01110 	IP_DFF 										[RFC6971]
 * 0x0F 	00 	0 	01111 	Performance and Diagnostic Metrics (PDM) 	[RFC8250]
 * 			10000-11101 	Unassigned
 * 0x1E 	00 	0 	11110 	RFC3692-style Experiment [1] 				[RFC4727]
 * 0x3E 	00 	1 	11110 	RFC3692-style Experiment [1] 				[RFC4727]
 * 0x5E 	01 	0 	11110 	RFC3692-style Experiment [1] 				[RFC4727]
 * 0x7E 	01 	1 	11110 	RFC3692-style Experiment [1] 				[RFC4727]
 * 0x9E 	10 	0 	11110 	RFC3692-style Experiment [1] 				[RFC4727]
 * 0xBE 	10 	1 	11110 	RFC3692-style Experiment [1] 				[RFC4727]
 * 0xDE 	11 	0 	11110 	RFC3692-style Experiment [1] 				[RFC4727]
 * 0xFE 	11 	1 	11110 	RFC3692-style Experiment [1] 				[RFC4727]
 * 			11111 	Unassigned */


/* 	 0                   1                   2                   3
 * 	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 	|Version| Traffic Class |           Flow Label                  |
 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 	|         Payload Length        |  Next Header  |   Hop Limit   |
 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 	|                                                               |
 * 	+                                                               +
 * 	|                                                               |
 * 	+                         Source Address                        +
 * 	|                                                               |
 * 	+                                                               +
 * 	|                                                               |
 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 	|                                                               |
 * 	+                                                               +
 * 	|                                                               |
 * 	+                      Destination Address                      +
 * 	|                                                               |
 * 	+                                                               +
 * 	|                                                               |
 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * 	Version				4-bit Internet Protocol version number = 6.
 *
 * 	Traffic Class		8-bit Traffic Class field.
 *
 * 	Flow Label			20-bit flow label.
 *
 * 	Payload Length		16-bit unsigned integer. Length of the IPv6 payload, i.e., the rest of the
 * 						packet following this IPv6 header, in octets. (Note that any extension
 * 						headers (see Section 4) present are considered part of the payload, i.e.,
 * 						included in the length count.)
 *
 * 	Next Header			8-bit selector. Identifies the type of header immediately following the IPv6
 * 						header. Uses the same values as the IPv4 Protocol field [IANA-PN].
 *
 * 	Hop Limit			8-bit unsigned integer. Decremented by 1 by each node that forwards the
 * 						packet. When forwarding, the packet is discarded if Hop Limit was zero when
 * 						received or is decremented to zero. A node that is the destination of a
 * 						packet should not discard a packet with Hop Limit equal to zero; it should
 * 						process the packet normally.
 *
 * 	Source Address		128-bit address of the originator of the packet. See [RFC4291].
 *
 * 	Destination Address	128-bit address of the intended recipient of the packet (possibly not the
 * 						ultimate recipient, if a Routing header is present). See [RFC4291] and
 * 						Section 4.4.
 *
 * IPv6 Extension Header
 *
 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 	|  Next Header  |  Hdr Ext Len  |                               |
 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 * 	|                                                               |
 * 	.                                                               .
 * 	.                            Options                            .
 * 	.                                                               .
 * 	|                                                               |
 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * 	Next Header			8-bit selector. Identifies the type of header immediately following the
 * 						Hop-by-Hop Options header. Uses the same values as the IPv4 Protocol field
 * 						[IANA-PN].
 *
 * 	Hdr Ext Len			8-bit unsigned integer. Length of the header in 8-octet units, not including
 * 						the first 8 octets. */
typedef struct {
	uint8_t data[16];
} IPAddress;

typedef struct {
	uint8_t data[8];
} LLAddress;


/* Fragmenting IPv6 packets: each complete fragment, except possibly the last one, is an integer
 * multiple of 8 octets long. Keep track of received fragments by a bitmask where 1 bit represents
 * 8 octets. */
typedef struct {
	Buffer   buffer;
	uint32_t fragid;
	uint8_t  fragments[IPV6_MTU/64];
} IPPacket;

typedef struct {
	Buffer   buffer;
	uint8_t* prev;		/* Previous next header field */
} IPExthdr;

typedef struct {
	Buffer buffer;
} IPOption;



/* Public Functions ------------------------------------------------------------------------------ */
bool        ipv6_addr_is_unspec    (const IPAddress*);
bool        ipv6_addr_is_loopback  (const IPAddress*);
bool        ipv6_addr_is_link_local(const IPAddress*);
bool        ipv6_addr_is_multicast (const IPAddress*);
int         ipv6_addr_compare      (const void*, const void*);

void*       ipv6_init              (IPPacket*, void*, unsigned, unsigned);
void*       ipv6_set_length        (IPPacket*, unsigned);
void        ipv6_parse             (IPPacket*);
void        ipv6_clear             (IPPacket*);
void        ipv6_finalize          (IPPacket*);
unsigned    ipv6_length            (const IPPacket*);
unsigned    ipv6_size              (const IPPacket*);
uint16_t    ipv6_checksum          (const void*, unsigned, uint16_t);
void*       ipv6_ptr_start         (const IPPacket*);

uint8_t     ipv6_version           (const IPPacket*);
uint8_t     ipv6_traffic_class     (const IPPacket*);
uint32_t    ipv6_flow_label        (const IPPacket*);
uint16_t    ipv6_payload_length    (const IPPacket*);
uint8_t     ipv6_next_header       (const IPPacket*);
uint8_t     ipv6_hop_limit         (const IPPacket*);
IPAddress*  ipv6_src               (const IPPacket*);
IPAddress*  ipv6_dest              (const IPPacket*);

bool        ipv6_set_traffic_class (IPPacket*, uint8_t);
bool        ipv6_set_flow_label    (IPPacket*, uint32_t);
bool        ipv6_set_next_header   (IPPacket*, uint8_t);
bool        ipv6_set_hop_limit     (IPPacket*, uint8_t);
bool        ipv6_set_src           (IPPacket*, const IPAddress*);
bool        ipv6_set_dest          (IPPacket*, const IPAddress*);


/* IPv6 Extension Headers ------------------------------------------------------------------------ */
bool        ipv6_eh_can_frag       (uint8_t);
bool        ipv6_eh_is_upper       (uint8_t);

IPPacket*   ipv6_eh_pkt            (const IPExthdr*);
IPExthdr    ipv6_eh_first          (IPPacket*);
IPExthdr    ipv6_eh_read_first     (Buffer*, uint8_t*, uint8_t*);
bool        ipv6_eh_next           (IPExthdr*);
bool        ipv6_eh_is_valid       (const IPExthdr*);
uint8_t     ipv6_eh_type           (const IPExthdr*);
uint16_t    ipv6_eh_length         (const IPExthdr*);
uint16_t    ipv6_eh_length_content (const IPExthdr*);

bool        ipv6_eh_prepend        (IPExthdr*, uint8_t, const void*, unsigned);
bool        ipv6_eh_append         (IPExthdr*, uint8_t, const void*, unsigned);
// Buffer*     ipv6_eh_buffer         (IPExthdr*);
Buffer*     ipv6_eh_reset_buffer   (IPExthdr*);
void        ipv6_eh_finalize       (IPExthdr*);


/* IPv6 TLV Options ------------------------------------------------------------------------------ */
IPOption    ipv6_opt_read          (const Buffer*, uint8_t*);
IPOption    ipv6_opt_first         (const IPExthdr*);
bool        ipv6_opt_next          (IPOption*);
bool        ipv6_opt_is_valid      (const IPOption*);
uint8_t     ipv6_opt_type          (const IPOption*);
uint8_t     ipv6_opt_length        (const IPOption*);

bool        ipv6_opt_append        (IPOption*, uint8_t, const void*, unsigned, unsigned, unsigned);
// Buffer*     ipv6_opt_buffer        (IPOption*);
Buffer*     ipv6_opt_reset_buffer  (IPOption*);
void        ipv6_opt_finalize      (IPOption*);


/* IPv6 Fragment Header -------------------------------------------------------------------------- */
/* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |  Next Header  |   Reserved    |      Fragment Offset    |Res|M|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                         Identification                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
bool        ipv6_frag_eh_prepend   (IPExthdr*, uint32_t, uint16_t);
bool        ipv6_frag_eh_append    (IPExthdr*, uint32_t, uint16_t);
bool        ipv6_frag_eh_finalize  (IPExthdr*, uint16_t);
uint16_t    ipv6_frag_eh_offset    (const IPExthdr*);
bool        ipv6_frag_eh_is_last   (const IPExthdr*);
uint32_t    ipv6_frag_eh_id        (const IPExthdr*);


#ifdef __cplusplus
}
#endif

#endif // IPV6_H
/******************************************* END OF FILE *******************************************/
