/************************************************************************************************//**
 * @file		lowpan.c
 *
 * @copyright	Copyright 2022 Kurt Hildebrand.
 * @license		Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
 *				file except in compliance with the License. You may obtain a copy of the License at
 *
 *				http://www.apache.org/licenses/LICENSE-2.0
 *
 *				Unless required by applicable law or agreed to in writing, software distributed under
 *				the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
 *				ANY KIND, either express or implied. See the License for the specific language
 *				governing permissions and limitations under the License.
 *
 ***************************************************************************************************/
#include <stdatomic.h>

#include "bits.h"
#include "byteorder.h"
#include "calc.h"
#include "ipv6.h"
#include "lowpan.h"


/* Private Functions ----------------------------------------------------------------------------- */
static Ieee154_Frame* lowpan_frame        (const Lowpan*);

static Lowpan   lowpan_first        (Ieee154_Frame*);
static bool     lowpan_next         (Lowpan*);
static bool     lowpan_is_valid     (const Lowpan*);
static uint8_t  lowpan_type         (const Lowpan*);
static uint8_t  lowpan_read_type    (const Buffer*, uint8_t*);
static uint16_t lowpan_read_length  (const Buffer*, uint8_t*, uint8_t);

static bool     lowpan_append_header(Lowpan*, uint8_t, const void*, unsigned);

static uint16_t lowpan_iphc_type              (const Lowpan*);
static bool     lowpan_is_iphc                (const Lowpan*);
static IPExthdr lowpan_iphc_eh_first          (Lowpan*);
static bool     lowpan_append_iphc_header     (Lowpan*, const IPAddress*, const IPAddress*);

static unsigned lowpan_iphc_flen_cid          (uint16_t);
static unsigned lowpan_iphc_flen_tcfl         (uint16_t);
static unsigned lowpan_iphc_flen_nh           (void);
static unsigned lowpan_iphc_flen_hlim         (uint16_t);
static unsigned lowpan_iphc_flen_src          (uint16_t);
static unsigned lowpan_iphc_flen_dest         (uint16_t);

static void     lowpan_iphc_set_type          (Lowpan*, uint16_t);
static void     lowpan_iphc_push_addr_mode    (Lowpan*, const IPAddress*, const IPAddress*);
static uint8_t  lowpan_iphc_set_src_addr_mode (Lowpan*, const IPAddress*);
static uint8_t  lowpan_iphc_set_dest_addr_mode(Lowpan*, const IPAddress*);
static void     lowpan_iphc_push_cid          (Lowpan*, uint8_t, uint8_t);
static void     lowpan_iphc_push_tcfl         (Lowpan*, const IPPacket*);
static void     lowpan_iphc_push_next_header  (Lowpan*, const IPPacket*);
static void     lowpan_iphc_push_hop_limit    (Lowpan*, const IPPacket*);
static void     lowpan_iphc_push_src          (Lowpan*, const IPAddress*);
static void     lowpan_iphc_push_dest         (Lowpan*, const IPAddress*);

static uint16_t lowpan_iphc_pop_type          (Lowpan*);
static uint8_t  lowpan_iphc_pop_cid           (Lowpan*, uint16_t);
static void     lowpan_iphc_pop_tcfl          (IPPacket*, Lowpan*, uint16_t);
static void     lowpan_iphc_pop_nh            (IPPacket*, Lowpan*, uint16_t);
static void     lowpan_iphc_pop_hlim          (IPPacket*, Lowpan*, uint16_t);
static void     lowpan_iphc_pop_src           (IPAddress*, Lowpan*, uint16_t, uint8_t);
static bool     lowpan_iphc_pop_dest          (IPAddress*, Lowpan*, uint16_t, uint8_t);


/* Private Variables ----------------------------------------------------------------------------- */
static IPAddress lowpan_ctx_data[16];
static _Atomic uint32_t lowpan_ctx_bitmask;


// =============================================================================================== //
// 6LOWPAN Context                                                                                 //
// =============================================================================================== //
/* lowpan_ctx_init ******************************************************************************//**
 * @brief		Initializes the lowpan address context. */
void lowpan_ctx_init(void)
{
	// IPAddress link_local = (IPAddress){ .data = {
	// 	0xFE, 0x80, 0x00, 0x00,
	// 	0x00, 0x00, 0x00, 0x00,
	// 	0x00, 0x00, 0x00, 0x00,
	// 	0x00, 0x00, 0x00, 0x00,
	// }};

	IPAddress link_local = { 0 };
	link_local.data[0]   = 0xFE;
	link_local.data[1]   = 0x80;

	lowpan_ctx_bitmask = 0;

	lowpan_ctx_put(0, &link_local);
}


/* lowpan_ctx_clear *****************************************************************************//**
 * @brief		Clears the lowpan address context. */
void lowpan_ctx_clear(void)
{
	lowpan_ctx_init();
}


/* lowpan_ctx_count *****************************************************************************//**
 * @brief		Returns the current number of entries in the lowpan address context. */
unsigned lowpan_ctx_count(void)
{
	return calc_popcount_u32(lowpan_ctx_bitmask);
}


/* lowpan_ctx_put *******************************************************************************//**
 * @brief		Inserts a new address into the lowpan address context. */
bool lowpan_ctx_put(Key id, const IPAddress* in)
{
	if(id > 16)
	{
		return false;
	}

	if(lowpan_ctx_bitmask & (1 << id))
	{
		return false;
	}

	lowpan_ctx_bitmask |= (1 << id);
	memmove(&lowpan_ctx_data[id], in, sizeof(*in));
	return true;
}


/* lowpan_ctx_get *******************************************************************************//**
 * @brief		Retrieves an address with the given id out of the lowpan address context. */
bool lowpan_ctx_get(Key id, IPAddress* out)
{
	if(id > 16)
	{
		return false;
	}

	if((lowpan_ctx_bitmask & (1 << id)) == 0)
	{
		return false;
	}

	memmove(out, &lowpan_ctx_data[id], sizeof(*out));
	return true;
}


/* lowpan_ctx_search_id *************************************************************************//**
 * @brief		Returns a pointer to the lowpan context with the given id. */
const IPAddress* lowpan_ctx_search_id(Key id)
{
	if(id > 16)
	{
		return 0;
	}
	else if((lowpan_ctx_bitmask & (1 << id)) == 0)
	{
		return 0;
	}
	else
	{
		return &lowpan_ctx_data[id];
	}
}


/* lowpan_ctx_search_addr ***********************************************************************//**
 * @brief		Returns a pointer to the lowpan context with the given address.
 * @param[in]	addr: the IPv6 address to find in the lowpan context.
 * @param[in]	start: the byte offset to start the search.
 * @param[in]	len: the number of bytes to search. */
Key lowpan_ctx_search_addr(const IPAddress* addr, unsigned start, unsigned len)
{
	unsigned i;

	if(start       >= sizeof(IPAddress) ||
	   len         >  sizeof(IPAddress) ||
	   start + len >  sizeof(IPAddress))
	{
		return -1u;
	}

	for(i = 0; i < 16; i++)
	{
		if((lowpan_ctx_bitmask & (1 << i)) == 0)
		{
			continue;
		}

		if(memcmp(&addr->data[start], &lowpan_ctx_data[i].data[start], len) == 0)
		{
			return i;
		}
	}

	return -1u;
}


/* lowpan_ctx_remove ****************************************************************************//**
 * @brief		Removes the lowpan address with the given id. */
bool lowpan_ctx_remove(Key id)
{
	if(id > 16)
	{
		return false;
	}
	else if((lowpan_ctx_bitmask & (1 << id)) == 0)
	{
		return false;
	}
	else
	{
		lowpan_ctx_bitmask &= ~(1 << id);
		return true;
	}
}





// =============================================================================================== //
// 6LOWPAN Compression / Decompression                                                             //
// =============================================================================================== //
/* lowpan_compress ******************************************************************************//**
 * @brief		Compresses an IPv6 packet into a lowpan frame. Fragments the packet If the full
 * 				compressed packet does not fit.
 * @warning		Expects the packet to be ready to transmit. Call ipv6_finalize before calling
 * 				lowpan_compress.
 * @return		Returns the total number of IPv6 packet bytes that have been fragmented.
 *				Returns 0 if there was an error. */
unsigned lowpan_compress(IPPacket* packet, Ieee154_Frame* frame)
{
	Bits frags = make_bits(&packet->fragments, (ipv6_length(packet) + 7) / 8);

	/* Check if all fragments have been sent */
	if(bits_next_zero(&frags, 0) >= bits_count(&frags))
	{
		return ipv6_length(packet);
	}

	Lowpan lowpan = lowpan_first(frame);
	lowpan_append_iphc_header   (&lowpan, ipv6_src(packet), ipv6_dest(packet));
	lowpan_iphc_push_tcfl       (&lowpan, packet);

	lowpan_iphc_push_next_header(&lowpan, packet);
	lowpan_iphc_push_hop_limit  (&lowpan, packet);
	lowpan_iphc_push_src        (&lowpan, ipv6_src(packet));
	lowpan_iphc_push_dest       (&lowpan, ipv6_dest(packet));

	/* First 40 bytes have been appended (40 bytes is the first 5 fragments) */
	bits_set_many(&frags, 0, 5);

	/* Append unfragmentable headers */
	unsigned frag   = 5;
	IPExthdr pkt_eh = ipv6_eh_first(packet);
	IPExthdr low_eh = lowpan_iphc_eh_first(&lowpan);

	/* Append unfragmentable extension headers */
	while(!ipv6_eh_can_frag(ipv6_eh_type(&pkt_eh)))
	{
		unsigned length = ipv6_eh_length(&pkt_eh);

		if(!buffer_push_mem(&low_eh.buffer, buffer_start(&pkt_eh.buffer), length))
		{
			return 0;
		}

		bits_set_many(&frags, frag, (length + 7) / 8);
		frag += (length + 7) / 8;
		ipv6_eh_next(&pkt_eh);
		ipv6_eh_next(&low_eh);
	}

	/* Find the next unsent fragment */
	frag = bits_next_zero(&frags, frag);

	if(frag < bits_end(&frags))
	{
		/* Check if the packet must be fragmented. The packet must be fragmented if the remaining
		 * length doesn't fit in the frame or if the packet has been partially sent (indicated by a 1
		 * in the fragment bitmap beyond the unfragmentable extension headers). */
		unsigned remaining = buffer_write(&packet->buffer) - buffer_start(&pkt_eh.buffer);

		if(remaining > ieee154_free(frame) || bits_next_one(&frags, frag) < bits_end(&frags))
		{
			ipv6_frag_eh_append(&low_eh, 0, frag * 8);	/* Todo: actual fragment id */
		}

		/* Begin reading from the location of the next unsent fragment in the packet itself */
		buffer_read_seek(&packet->buffer, frag * 8);

		/* Append as many contiguous, unsent fragments as possible to the frame */
		while(frag < bits_count(&frags) && bits_value(&frags, frag) == 0)
		{
			unsigned length = calc_min_uint(8, ipv6_length(packet) - (frag * 8));

			if(!buffer_push_mem(&lowpan.buffer, buffer_pop(&packet->buffer, length), length))
			{
				break;
			}

			bits_set(&frags, frag++);
		}

		ipv6_frag_eh_finalize(&low_eh, ipv6_length(packet));
	}

	/* Compute the number of sent bytes */
	unsigned sent = bits_ones(&frags) * 8;

	/* Each fragment bit represents 8 bytes expect for the last fragment. If the last fragment has
	 * been sent, then the number of sent bytes may be overestimated. */
	if(bits_value(&frags, bits_end(&frags)-1))
	{
		/* Subtract unused bytes in the last fragment. Example:
		 * packet length: 27 bytes
		 * fragment bitmask: 1111b
		 * fragment bitmask length: 4 bits (1 bit = 8 byte fragment)
		 * fragment bitmask range: start = bit  0, end = bit  4
		 *                         start = byte 0, end = byte 32
		 *
		 * the last frag bit overestimates the length of the fragment by: 32 - 27 = 5 bytes
		 *
		 * 		0          1          2          3
		 * 		01234567 89012345 67890123 45678901
		 * 		                              ^ 27 bytes long
		 * 		1        1        1        1 fragment bit mask */
		sent -= ((ipv6_length(packet) + 7) / 8) - (ipv6_length(packet) / 8);
	}

	return sent;
}


/* lowpan_decompress ****************************************************************************//**
 * @brief		Decompresses a lowpan frame to a full IPv6 packet. */
unsigned lowpan_decompress(IPPacket* packet, Ieee154_Frame* frame)
{
	Lowpan iphc_hdr = { 0 };
	Lowpan lowpan   = lowpan_first(frame);

	while(lowpan_is_valid(&lowpan) && (lowpan_type(&lowpan) & LOWPAN_NALP_MASK) != LOWPAN_NALP)
	{
		if(lowpan_is_iphc(&lowpan))
		{
			iphc_hdr = lowpan;
		}

		lowpan_next(&lowpan);
	}

	Bits frags = make_bits(packet->fragments, sizeof(packet->fragments) * 8);

	/* LOWPAN_IPHC
	 * 	     0                                       1
	 * 	     0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5
	 * 	   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 * 	0  | 0 | 1 | 1 | TF    |NH | HLIM  |CID|SAC| SAM   | M |DAC| DAM   |
	 * 	   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 * 	2  | SCI           | DCI           | ECN   | DSCP                  |
	 * 	   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 * 	4  | rsc           | Flow Label                                    |
	 * 	   +---+---+---+---+               +---+---+---+---+---+---+---+---+
	 * 	8  |                               | Next Header                   |
	 * 	   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 * 	10 | Hop Limit                     | Source Address                |
	 * 	   +---+---+---+---+---+---+---+---+                               |
	 * 	12 |                                                               |
	 * 	   +                                                               |
	 * 	14 |                                                               |
	 * 	   +                                                               |
	 *  16 |                                                               |
	 * 	   +                               +---+---+---+---+---+---+---+---+
	 * 	18 |                               | Destination Address           |
	 * 	   +---+---+---+---+---+---+---+---+                               |
	 * 	20 |                                                               |
	 * 	   |                                                               |
	 * 	22 |                                                               |
	 * 	   |                                                               |
	 * 	24 |                                                               |
	 * 	   |                               +---+---+---+---+---+---+---+---+
	 * 	26 |                               |
	 * 	   +---+---+---+---+---+---+---+---+ */
	uint16_t iphc = lowpan_iphc_pop_type(&iphc_hdr);
	uint8_t  cid  = lowpan_iphc_pop_cid (&iphc_hdr, iphc);
	lowpan_iphc_pop_tcfl(packet, &iphc_hdr, iphc);
	lowpan_iphc_pop_nh  (packet, &iphc_hdr, iphc);
	lowpan_iphc_pop_hlim(packet, &iphc_hdr, iphc);
	lowpan_iphc_pop_src (ipv6_src(packet),  &iphc_hdr, iphc, cid);
	lowpan_iphc_pop_dest(ipv6_dest(packet), &iphc_hdr, iphc, cid);

	/* Note: IPv6 Extension Header Compression is not supported. Extension headers are expected to be
	 * carried inline. */
	IPExthdr low_eh = lowpan_iphc_eh_first(&iphc_hdr);
	unsigned length = buffer_write(&frame->buffer) - buffer_start(&low_eh.buffer);

	buffer_push_mem(&packet->buffer, buffer_start(&low_eh.buffer), length);
	ipv6_finalize  (packet);
	bits_set_many  (&frags, 0, 5 + (ipv6_length(packet) + 7) / 8);

	return ipv6_length(packet);
}


/* lowpan_frame *********************************************************************************//**
 * @brief		Returns the IEEE 802.15.4 frame associated with the 6lowpan header. */
static Ieee154_Frame* lowpan_frame(const Lowpan* lowpan)
{
	return (Ieee154_Frame*)buffer_parent(&lowpan->buffer);
}


/* lowpan_first *********************************************************************************//**
 * @brief		Returns the first 6LOWPAN header in the IEEE 802.15.4 frame. */
static Lowpan lowpan_first(Ieee154_Frame* frame)
{
	Ieee154_IE ie = ieee154_ie_first(frame);

	while(!ieee154_ie_is_last(&ie))
	{
		ieee154_ie_next(&ie);
	}

	Lowpan lowpan;

	Buffer*  parent = ieee154_reset_buffer(frame);
	uint8_t* start  = buffer_read         (parent);
	uint8_t  type   = lowpan_read_type    (parent, start);
	uint16_t length = lowpan_read_length  (parent, start, type);

	buffer_slice(&lowpan.buffer, parent, start, length);

	return lowpan;
}


/* lowpan_next **********************************************************************************//**
 * @brief		Moves to the next header if possible. */
static bool lowpan_next(Lowpan* lowpan)
{
	if(!buffer_is_valid(&lowpan->buffer))
	{
		return false;
	}

	Buffer*  parent = buffer_parent     (&lowpan->buffer);
	uint8_t* start  = buffer_write      (parent);
	uint8_t  type   = lowpan_read_type  (parent, start);
	uint16_t length = lowpan_read_length(parent, start, type);

	buffer_slice(&lowpan->buffer, parent, start, length);

	return true;
}


/* lowpan_is_valid ******************************************************************************//**
 * @brief		Returns true if the header is a valid 6LOWPAN header. */
static bool lowpan_is_valid(const Lowpan* lowpan)
{
	return buffer_parent(&lowpan->buffer) != 0 &&
	       buffer_length(&lowpan->buffer) != 0;
}


/* lowpan_type **********************************************************************************//**
 * @brief		Reads a 6LOWPAN header's ctrl field. */
static uint8_t lowpan_type(const Lowpan* lowpan)
{
	return lowpan_read_type(&lowpan->buffer, buffer_start(&lowpan->buffer));
}


/* lowpan_read_type *****************************************************************************//**
 * @brief		Reads a 6LOWPAN header's ctrl field from the underlying buffer. */
static uint8_t lowpan_read_type(const Buffer* parent, uint8_t* start)
{
	void* ptr = buffer_peek_at(parent, start, 1);

	if(!ptr)
	{
		return LOWPAN_NALP;
	}
	else
	{
		return be_get_u8(ptr);
	}
}


/* lowpan_read_length ***************************************************************************//**
 * @brief		Returns the length of 6LOWPAN header. */
static uint16_t lowpan_read_length(const Buffer* parent, uint8_t* start, uint8_t type)
{
	if(type == LOWPAN_NALP)
	{
		return 0;
	}

	if((type & LOWPAN_IPHC_MASK) == LOWPAN_IPHC)
	{
		if(buffer_start(parent) <= start && start <= buffer_write(parent))
		{
			return buffer_write(parent) - start;
		}
	}

	return 0;
}


/* lowpan_append_header *************************************************************************//**
 * @brief		Appends a 6LOWPAN header after the current header. Moves the lowpan header to the
 * 				newly appended header if successful. Reserves len bytes in addition to the header
 * 				type and if the 'in' parameter is not null, copies len bytes after the header type.
 * 				The final length of the 6LOWPAN header after the call to lowpan_append_header is
 * 				1 + len bytes.
 * @param[in]	lowpan: the header to append to.
 * @param[in]	type: the appended header's type.
 * @param[in]	in: the content of the header.
 * @param[in]	len: the number of bytes for the content of the header.
 * @retval		true if the new header was appended successfully.
 * @retval		false if the frame is full. */
static bool lowpan_append_header(Lowpan* lowpan, uint8_t type, const void* in, unsigned len)
{
	uint8_t* start = buffer_write(&lowpan->buffer);

	if(!buffer_reserve_at(&lowpan->buffer, start, 1 + len))
	{
		return false;
	}

	buffer_slice(&lowpan->buffer, buffer_parent(&lowpan->buffer), start, 1 + len);
	buffer_replace_offset(&lowpan->buffer, &type, 0, 1);
	buffer_replace_offset(&lowpan->buffer, in,    1, len);
	return true;
}





// ----------------------------------------------------------------------------------------------- //
// LOWPAN IPHC                                                                                     //
// ----------------------------------------------------------------------------------------------- //
/* lowpan_is_iphc *******************************************************************************//**
 * @brief		Returns true if the lowpan header is an IPHC header. */
static bool lowpan_is_iphc(const Lowpan* lowpan)
{
	return (lowpan_type(lowpan) & LOWPAN_IPHC_MASK) == LOWPAN_IPHC;
}


/* lowpan_iphc_type *****************************************************************************//**
* @brief		Returns the first 16 bits of the IPHC header. */
static uint16_t lowpan_iphc_type(const Lowpan* lowpan)
{
	return be_get_u16(buffer_peek_offset(&lowpan->buffer, 0, 2));
}


/* lowpan_iphc_eh_first *************************************************************************//**
 * @brief		Returns the first IPv6 Extension Header in the lowpan packet. */
static IPExthdr lowpan_iphc_eh_first(Lowpan* lowpan)
{
	/* LOWPAN_IPHC
	 * 	     0                                       1
	 * 	     0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5
	 * 	   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 * 	0  | 0 | 1 | 1 | TF    |NH | HLIM  |CID|SAC| SAM   | M |DAC| DAM   |
	 * 	   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 * 	2  | SCI           | DCI           | ECN   | DSCP                  |
	 * 	   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 * 	4  | rsc           | Flow Label                                    |
	 * 	   +---+---+---+---+               +---+---+---+---+---+---+---+---+
	 * 	8  |                               | Next Header                   |
	 * 	   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 * 	10 | Hop Limit                     | Source Address                |
	 * 	   +---+---+---+---+---+---+---+---+                               |
	 * 	12 |                                                               |
	 * 	   +                                                               |
	 * 	14 |                                                               |
	 * 	   +                                                               |
	 *  16 |                                                               |
	 * 	   +                               +---+---+---+---+---+---+---+---+
	 * 	18 |                               | Destination Address           |
	 * 	   +---+---+---+---+---+---+---+---+                               |
	 * 	20 |                                                               |
	 * 	   |                                                               |
	 * 	22 |                                                               |
	 * 	   |                                                               |
	 * 	24 |                                                               |
	 * 	   |                               +---+---+---+---+---+---+---+---+
	 * 	26 |                               |
	 * 	   +---+---+---+---+---+---+---+---+ */
	uint16_t iphc  = lowpan_iphc_type(lowpan);

	uint8_t* prev =
		buffer_start(&lowpan->buffer) + 2 +
		lowpan_iphc_flen_cid (iphc) +
		lowpan_iphc_flen_tcfl(iphc);

	uint8_t* start = prev +
		lowpan_iphc_flen_nh  ()     +
		lowpan_iphc_flen_hlim(iphc) +
		lowpan_iphc_flen_src (iphc) +
		lowpan_iphc_flen_dest(iphc);

	return ipv6_eh_read_first(&lowpan->buffer, prev, start);
}


/* lowpan_append_iphc_header ********************************************************************//**
 * @brief		Appends a IPHC header. */
static bool lowpan_append_iphc_header(Lowpan* lowpan, const IPAddress* src, const IPAddress* dest)
{
	/* Total size of the default iphc header is 2 bytes. Remove one byte for the header type. */
	if(!lowpan_append_header(lowpan, LOWPAN_IPHC, 0, 1))
	{
		return false;
	}

	/*   0                                       1
		*   0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5
		* +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
		* | 0 | 1 | 1 |  TF   |NH | HLIM  |CID|SAC|  SAM  | M |DAC|  DAM  |
		* +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+ */
	lowpan_iphc_set_type(lowpan, (LOWPAN_IPHC << 8) |
		(LOWPAN_IPHC_TF_NONE | LOWPAN_IPHC_NH_INLINE | LOWPAN_IPHC_HLIM_INLINE));

	lowpan_iphc_push_addr_mode(lowpan, src, dest);
	return true;
}


/* lowpan_iphc_flen_cid *************************************************************************//**
 * @brief		Returns the length of the CID extension field given an IPHC type. */
static unsigned lowpan_iphc_flen_cid(uint16_t iphc)
{
	if((iphc & LOWPAN_IPHC_CID_MASK) == LOWPAN_IPHC_CID_NONE)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


/* lowpan_iphc_flen_tcfl ************************************************************************//**
 * @brief		Returns the length of the traffic class / flow label extension field from the IPHC
 * 				type. */
static unsigned lowpan_iphc_flen_tcfl(uint16_t iphc)
{
	/* 	 0                   1                   2                   3
	 * 	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 	|ECN|   DSCP    |  rsv  |             Flow Label                |
	 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 	 Figure 4: TF = 00: Traffic Class and Flow Label carried in-line
	 *
	 * 	 0                   1                   2
	 * 	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
	 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 	|ECN|rsv|             Flow Label                |
	 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 	Figure 5: TF = 01: Flow Label carried in-line
	 *
	 * 	 0 1 2 3 4 5 6 7
	 * 	+-+-+-+-+-+-+-+-+
	 * 	|ECN|   DSCP    |
	 * 	+-+-+-+-+-+-+-+-+
	 * 	Figure 6: TF = 10: Traffic Class carried in-line */
	switch(iphc & LOWPAN_IPHC_TF_MASK) {
	case LOWPAN_IPHC_TF_TC_FL: return 4;
	case LOWPAN_IPHC_TF_FL:    return 3;
	case LOWPAN_IPHC_TF_TC:    return 1;
	default: return 0;
	}
}



/* lowpan_iphc_flen_nh **************************************************************************//**
 * @brief		Returns the length of the next header field. */
static unsigned lowpan_iphc_flen_nh(void)
{
	return 1;
}


/* lowpan_iphc_flen_hlim ************************************************************************//**
 * @brief		Returns the expected length of the hop limit extension field from the IPHC type. */
static unsigned lowpan_iphc_flen_hlim(uint16_t iphc)
{
	if((iphc & LOWPAN_IPHC_HLIM_MASK) == LOWPAN_IPHC_HLIM_INLINE)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/* lowpan_iphc_flen_src *************************************************************************//**
 * @brief		Returns the expected length of the source address field from the IPHC type. */
static unsigned lowpan_iphc_flen_src(uint16_t iphc)
{
	switch(iphc & (LOWPAN_IPHC_SAC_MASK | LOWPAN_IPHC_SAM_MASK)) {
	/* SAC=0: Stateless (SL) Source Addressing Mode */
	case LOWPAN_IPHC_SAC_STATELESS | LOWPAN_IPHC_SAM_SL_128:    return 16;
	case LOWPAN_IPHC_SAC_STATELESS | LOWPAN_IPHC_SAM_SL_64:     return 8;
	case LOWPAN_IPHC_SAC_STATELESS | LOWPAN_IPHC_SAM_SL_16:     return 2;
	case LOWPAN_IPHC_SAC_STATELESS | LOWPAN_IPHC_SAM_SL_0:      return 0;

	/* SAC=1: Stateful (SF) Source Addressing Mode */
	case LOWPAN_IPHC_SAC_STATEFUL | LOWPAN_IPHC_SAM_SF_UNSPEC:  return 0;
	case LOWPAN_IPHC_SAC_STATEFUL | LOWPAN_IPHC_SAM_SF_64:      return 8;
	case LOWPAN_IPHC_SAC_STATEFUL | LOWPAN_IPHC_SAM_SF_16:      return 2;
	case LOWPAN_IPHC_SAC_STATEFUL | LOWPAN_IPHC_SAM_SF_0:       return 0;
	default : return 0;
	}
}


/* lowpan_iphc_flen_dest ************************************************************************//**
 * @brief		Returns the expected length of the destination address field from the IPHC type. */
static unsigned lowpan_iphc_flen_dest(uint16_t iphc)
{
	switch(iphc & (LOWPAN_IPHC_M_MASK | LOWPAN_IPHC_DAC_MASK | LOWPAN_IPHC_DAM_MASK)) {
	/* M=0, DAC=0: Not multicast, stateless (SL) destination addressing */
	case LOWPAN_IPHC_M_NOT_MULTICAST | LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_DAM_SL_128:
		return 16;
	case LOWPAN_IPHC_M_NOT_MULTICAST | LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_DAM_SL_64:
		return 8;
	case LOWPAN_IPHC_M_NOT_MULTICAST | LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_DAM_SL_16:
		return 2;
	case LOWPAN_IPHC_M_NOT_MULTICAST | LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_DAM_SL_0:
		return 0;

	/* M=0, DAC=1: Not multicast, stateful (SF) destination addressing */
	case LOWPAN_IPHC_M_NOT_MULTICAST | LOWPAN_IPHC_DAC_STATEFUL | LOWPAN_IPHC_DAM_SF_64:
		return 8;
	case LOWPAN_IPHC_M_NOT_MULTICAST | LOWPAN_IPHC_DAC_STATEFUL | LOWPAN_IPHC_DAM_SF_16:
		return 2;
	case LOWPAN_IPHC_M_NOT_MULTICAST | LOWPAN_IPHC_DAC_STATEFUL | LOWPAN_IPHC_DAM_SF_0:
		return 0;

	/* M=1, DAC=0: Multicast, stateless (SL) destination addressing */
	case LOWPAN_IPHC_M_MULTICAST | LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_DAM_MSL_128:
		return 16;
	case LOWPAN_IPHC_M_MULTICAST | LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_DAM_MSL_48:
		return 6;
	case LOWPAN_IPHC_M_MULTICAST | LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_DAM_MSL_32:
		return 4;
	case LOWPAN_IPHC_M_MULTICAST | LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_DAM_MSL_8:
		return 1;

	/* M=1, DAC=1: Multicast (M), stateful (SF) destination addressing */
	case LOWPAN_IPHC_M_MULTICAST | LOWPAN_IPHC_DAC_STATEFUL | LOWPAN_IPHC_DAM_MSF_48:
		return 6;

	default : return 0;
	}
}


/* lowpan_iphc_set_type *************************************************************************//**
 * @brief		Sets the IPHC header's type.
 * @warning		Assumes that lowpan is a valid IPHC header. */
static void lowpan_iphc_set_type(Lowpan* lowpan, uint16_t iphc)
{
	iphc = be_u16(iphc);
	buffer_replace_at(&lowpan->buffer, &iphc, buffer_start(&lowpan->buffer), 2);
}


/* lowpan_iphc_push_addr_mode *******************************************************************//**
 * @brief		Sets the IPHC lowpan header's addressing mode.
 * @desc		Potentially sets SAC, SAM, DAC, DAM. */
static void lowpan_iphc_push_addr_mode(Lowpan* lowpan, const IPAddress* src, const IPAddress* dest)
{
	uint8_t sci = lowpan_iphc_set_src_addr_mode (lowpan, src);
	uint8_t dci = lowpan_iphc_set_dest_addr_mode(lowpan, dest);
	lowpan_iphc_push_cid(lowpan, sci, dci);
}


/* lowpan_iphc_set_set_src_addr_mode ************************************************************//**
 * @brief		Sets the source addressing mode bits in the iphc header. Returns the SCI
 * 				corresponding to addressing mode. */
static uint8_t lowpan_iphc_set_src_addr_mode(Lowpan* lowpan, const IPAddress* src)
{
	Key ctx = -1;

	uint16_t iphc = lowpan_iphc_type(lowpan) & ~(LOWPAN_IPHC_SAC_MASK | LOWPAN_IPHC_SAM_MASK);

	/* Is address unspecified ::0? */
	if(src->data[0]  == 0 && src->data[1]  == 0 && src->data[2]  == 0 && src->data[3]  == 0 &&
	   src->data[4]  == 0 && src->data[5]  == 0 && src->data[6]  == 0 && src->data[7]  == 0 &&
	   src->data[8]  == 0 && src->data[9]  == 0 && src->data[10] == 0 && src->data[11] == 0 &&
	   src->data[12] == 0 && src->data[13] == 0 && src->data[14] == 0 && src->data[15] == 0)
	{
		iphc |= LOWPAN_IPHC_SAC_STATEFUL | LOWPAN_IPHC_SAM_SF_UNSPEC;
		goto done;
	}

	/* Search context prefix, which is /64. I.E. the first 8 bytes. */
	ctx = lowpan_ctx_search_addr(src, 0, 8);

	if(0 > ctx || ctx > 16)
	{
		iphc |= LOWPAN_IPHC_SAC_STATELESS | LOWPAN_IPHC_SAM_SL_128;
		goto done;
	}

	/* Is the address link local? */
	if(ctx == 0)
	{
		iphc |= LOWPAN_IPHC_SAC_STATELESS;
	}
	else
	{
		iphc |= LOWPAN_IPHC_SAC_STATEFUL;
	}

	/* Is the address in the form 0000:00ff:fe00:XXXX? */
	if(src->data[8]  == 0x00 && src->data[9]  == 0x00 && src->data[10] == 0x00 &&
	   src->data[11] == 0xFF && src->data[12] == 0xFE && src->data[13] == 0x00)
	{
		iphc |= LOWPAN_IPHC_SAM_16;
	}
	else
	{
		iphc |= LOWPAN_IPHC_SAM_64;
	}

	/* Can the source address be elided? Does the encapsulating frame contain the source address? */
	uint16_t frame_src_len = ieee154_length_src_addr(lowpan_frame(lowpan));

	if(frame_src_len && memcmp(
		&src->data[16-frame_src_len],
		ieee154_src_addr(lowpan_frame(lowpan)),
		frame_src_len) == 0)
	{
		/* The source address can be elided */
		iphc = (iphc & ~LOWPAN_IPHC_SAM_MASK) | LOWPAN_IPHC_SAM_0;
	}

	done:
		lowpan_iphc_set_type(lowpan, iphc);

		if(0 > ctx || ctx > 16)
		{
			return 0;
		}
		else
		{
			return ctx;
		}
}



/* lowpan_iphc_set_dest_addr_mode ***************************************************************//**
 * @brief		Returns the addressing bits for the source address. */
static uint8_t lowpan_iphc_set_dest_addr_mode(Lowpan* lowpan, const IPAddress* dest)
{
	Key ctx = -1u;
	uint16_t iphc = lowpan_iphc_type(lowpan) &
		~(LOWPAN_IPHC_M_MASK | LOWPAN_IPHC_DAC_MASK | LOWPAN_IPHC_DAM_MASK);

	/* Is the destination a multicast address? */
	if(ipv6_addr_is_multicast(dest))
	{
		iphc |= LOWPAN_IPHC_M_MULTICAST;

		/* Find context in the form ffXX:XXLL:PPPP:PPPP:PPPP:PPPP:XXXX:XXXX */
		ctx = lowpan_ctx_search_addr(dest, 3, 9);

		if(0 < ctx && ctx < 16)
		{
			iphc |= LOWPAN_IPHC_DAC_STATEFUL | LOWPAN_IPHC_DAM_MSF_48;
		}
		/* Does the address take the form ff02::00XX? */
		else if(dest->data[1]  == 0x02 && dest->data[2]  == 0x00 &&
		        dest->data[3]  == 0x00 && dest->data[4]  == 0x00 &&
		        dest->data[5]  == 0x00 && dest->data[6]  == 0x00 &&
		        dest->data[7]  == 0x00 && dest->data[8]  == 0x00 &&
		        dest->data[9]  == 0x00 && dest->data[10] == 0x00 &&
		        dest->data[11] == 0x00 && dest->data[12] == 0x00 &&
		        dest->data[13] == 0x00 && dest->data[14] == 0x00)
		{
			iphc |= LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_DAM_MSL_8;
		}
		/* Does the address take the form ffXX::00XX:XXXX? */
		else if(dest->data[2]  == 0x00 && dest->data[3]  == 0x00 &&
		        dest->data[4]  == 0x00 && dest->data[5]  == 0x00 &&
		        dest->data[6]  == 0x00 && dest->data[7]  == 0x00 &&
		        dest->data[8]  == 0x00 && dest->data[9]  == 0x00 &&
		        dest->data[10] == 0x00 && dest->data[11] == 0x00 &&
		        dest->data[12] == 0x00)
		{
			iphc |= LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_DAM_MSL_32;
		}
		/* Does the address take the form ffXX::00XX:XXXX:XXXX? */
		else if(dest->data[2]  == 0x00 && dest->data[3]  == 0x00 &&
		        dest->data[4]  == 0x00 && dest->data[5]  == 0x00 &&
		        dest->data[6]  == 0x00 && dest->data[7]  == 0x00 &&
		        dest->data[8]  == 0x00 && dest->data[9]  == 0x00 &&
		        dest->data[10] == 0x00)
		{
			iphc |= LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_DAM_MSL_48;
		}
		/* Must carry full 128-bit address inline */
		else
		{
			iphc |= LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_DAM_MSL_128;
		}
	}
	/* Address is not a multicast address */
	else
	{
		iphc |= LOWPAN_IPHC_M_NOT_MULTICAST;

		ctx = lowpan_ctx_search_addr(dest, 0, 8);

		if(0 > ctx || ctx > 16)
		{
			iphc |= LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_DAM_SL_128;
		}
		else
		{
			/* Is the address link local? */
			if(ctx == 0)
			{
				iphc |= LOWPAN_IPHC_DAC_STATELESS;
			}
			else
			{
				iphc |= LOWPAN_IPHC_DAC_STATEFUL;
			}

			/* Is the IID in the form 0000:00ff:fe00:XXXX? */
			if(dest->data[8]  == 0x00 && dest->data[9]  == 0x00 &&
			   dest->data[10] == 0x00 && dest->data[11] == 0xFF &&
			   dest->data[12] == 0xFE && dest->data[13] == 0x00)
			{
				iphc |= LOWPAN_IPHC_DAM_16;
			}
			else
			{
				iphc |= LOWPAN_IPHC_DAM_64;
			}

			/* Can the destination address be elided? Does the encapsulating frame contain the
			 * destination address? */
			uint16_t frame_dest_len = ieee154_length_dest_addr(lowpan_frame(lowpan));

			if(frame_dest_len && memcmp(
				&dest->data[16-frame_dest_len],
				ieee154_dest_addr(lowpan_frame(lowpan)),
				frame_dest_len) == 0)
			{
				/* The destination address can be elided */
				iphc = (iphc & ~(LOWPAN_IPHC_DAM_MASK)) | LOWPAN_IPHC_DAM_0;
			}
		}
	}

	lowpan_iphc_set_type(lowpan, iphc);

	if(0 > ctx || ctx > 16)
	{
		return 0;
	}
	else
	{
		return ctx;
	}
}


/* lowpan_iphc_push_cid *************************************************************************//**
 * @brief		Writes source and destination context identifier into the lowpan header. */
static void lowpan_iphc_push_cid(Lowpan* lowpan, uint8_t sci, uint8_t dci)
{
	uint8_t cid = (sci << LOWPAN_IPHC_CID_SCI_SHIFT) | (dci << LOWPAN_IPHC_CID_DCI_SHIFT);

	if(cid != 0)
	{
		uint16_t iphc = lowpan_iphc_type(lowpan);
		lowpan_iphc_set_type(lowpan, iphc | LOWPAN_IPHC_CID_EXT);
		buffer_push_mem(&lowpan->buffer, &cid, 1);
	}
}


/* lowpan_iphc_push_tcfl ************************************************************************//**
 * @brief		Writes traffic class / flow label into the lowpan header. */
static void lowpan_iphc_push_tcfl(Lowpan* lowpan, const IPPacket* packet)
{
	/* 6LOWPAN Traffic Class/Flow Label fields:
	 *
	 * 		 0                   1                   2                   3
	 * 		 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 		|ECN|   DSCP    |  rsv  |             Flow Label                |
	 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 		 Figure 4: TF = 00: Traffic Class and Flow Label carried in-line
	 *
	 * 		 0                   1                   2
	 * 		 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
	 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 		|ECN|rsv|             Flow Label                |
	 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 		Figure 5: TF = 01: Flow Label carried in-line
	 *
	 * 		 0 1 2 3 4 5 6 7
	 * 		+-+-+-+-+-+-+-+-+
	 * 		|ECN|   DSCP    |
	 * 		+-+-+-+-+-+-+-+-+
	 * 		Figure 6: TF = 10: Traffic Class carried in-line
	 *
	 * IPv6 Traffic Class:
	 *
	 * 		 0              |    1          |        2      |            3
	 * 		 0 1 2 3 4 5 6 7|8 9 0 1 2 3 4 5|6 7 8 9 0 1 2 3|4 5 6 7 8 9 0 1
	 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 		|Version| Traffic Class |       |   Flow Label  |               |
	 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 *  	        | DSCP  |   |ECN|       |               |
	 *  	        +-------|-------+       |               |
	 */
	uint8_t  buf[4];
	uint16_t iphc = lowpan_iphc_type(lowpan) & ~(LOWPAN_IPHC_TF_MASK);
	uint16_t tc   = ipv6_traffic_class(packet);
	uint8_t  ecn  = (tc >> 0) & 0x3;
	uint8_t  dscp = (tc >> 2) & 0x3F;
	uint32_t flow = ipv6_flow_label(packet);

	/* TF = 00 if flowlabel & dscp (ecn set regardless)
	 * TF = 01 if flowlabel        (ecn set regardless)
	 * TF = 10 if ecn | dscp       (flowlabel not set)
	 * TF = 11 else                (nothing needs to be set) */
	if(flow != 0) {
		if(dscp != 0) {
			iphc |= LOWPAN_IPHC_TF_TC_FL;
			buf[0] = (ecn  << 6)  | dscp;
			buf[1] = (flow >> 16) & 0x0F;
			buf[2] = (flow >> 8)  & 0xFF;
			buf[3] = (flow >> 0)  & 0xFF;
			buffer_push_mem(&lowpan->buffer, buf, 4);
		} else {
			iphc |= LOWPAN_IPHC_TF_FL;
			buf[0] = (ecn  << 6) | ((flow >> 16) & 0x0F);
			buf[1] = (flow >> 8) & 0xFF;
			buf[2] = (flow >> 0) & 0xFF;
			buffer_push_mem(&lowpan->buffer, buf, 3);
		}
	} else if(ecn != 0 || dscp != 0) {
		iphc |= LOWPAN_IPHC_TF_TC;
		buf[0] = (ecn << 6) | dscp;
		buffer_push_mem(&lowpan->buffer, buf, 1);
	} else {
		iphc |= LOWPAN_IPHC_TF_NONE;
	}

	lowpan_iphc_set_type(lowpan, iphc);
}


/* lowpan_iphc_push_next_header *****************************************************************//**
 * @brief		Writes the next header field. Next header will always be set as
 * 				LOWPAN_IPHC_NH_INLINE. */
static void lowpan_iphc_push_next_header(Lowpan* lowpan, const IPPacket* packet)
{
	/* TODO: verify that LOWPAN_IPHC_NH_INLINE is set when compressing */
	uint8_t nh = ipv6_next_header(packet);
	buffer_push_mem(&lowpan->buffer, &nh, 1);
}


/* lowpan_iphc_push_hop_limit *******************************************************************//**
 * @brief		Writes the hop limit field. Hop limit will always be set as
 * 				LOWPAN_IPHC_HLIM_INLINE. */
static void lowpan_iphc_push_hop_limit(Lowpan* lowpan, const IPPacket* packet)
{
	/* TODO: verify that LOWPAN_IPHC_HLIM_INLINE is set when compressing */
	uint8_t hlim = ipv6_hop_limit(packet);
	buffer_push_mem(&lowpan->buffer, &hlim, 1);
}


/* lowpan_iphc_push_src *************************************************************************//**
 * @brief		Writes the compressed source address to the frame. */
static void lowpan_iphc_push_src(Lowpan* lowpan, const IPAddress* src)
{
	uint16_t iphc = lowpan_iphc_type(lowpan);
	unsigned flen = lowpan_iphc_flen_src(iphc);
	buffer_push_mem(&lowpan->buffer, &src->data[16-flen], flen);
}


/* lowpan_iphc_push_dest ************************************************************************//**
 * @brief		Writes the compressed destination address to the frame. */
static void lowpan_iphc_push_dest(Lowpan* lowpan, const IPAddress* dest)
{
	uint16_t iphc = lowpan_iphc_type(lowpan);
	unsigned flen = lowpan_iphc_flen_dest(iphc);

	/* Copy the first few bytes if multicast address. However, no need to handle ff02::00XX the
	 * address bytes are contiguous (this case is handled like non-multicast addresses). */
	if((iphc & LOWPAN_IPHC_M_MASK) == LOWPAN_IPHC_M_MULTICAST)
	{
		/* Is address in the form ffXX:XXLL:PPPP:PPPP:PPPP:PPPP:XXXX:XXXX? */
		if((iphc & LOWPAN_IPHC_DAC_MASK) == LOWPAN_IPHC_DAC_STATEFUL &&
		  ((iphc & LOWPAN_IPHC_DAM_MASK) == LOWPAN_IPHC_DAM_MSF_48))
		{
			buffer_push_mem(&lowpan->buffer, &dest->data[1], 2);
			flen -= 2;
		}
		/* Is address in the form ffXX::00XX:XXXX or ffXX::00XX:XXXX:XXXX? */
		else if((iphc & LOWPAN_IPHC_DAC_MASK) == LOWPAN_IPHC_DAC_STATELESS &&
		       ((iphc & LOWPAN_IPHC_DAM_MASK) == LOWPAN_IPHC_DAM_MSL_32 ||
		        (iphc & LOWPAN_IPHC_DAM_MASK) == LOWPAN_IPHC_DAM_MSL_48))
		{
			buffer_push_mem(&lowpan->buffer, &dest->data[1], 1);
			flen -= 1;
		}
	}

	buffer_push_mem(&lowpan->buffer, &dest->data[16-flen], flen);
}


/* lowpan_iphc_pop_type *************************************************************************//**
 * @brief		*/
static uint16_t lowpan_iphc_pop_type(Lowpan* lowpan)
{
	return be_get_u16(buffer_pop_u16(&lowpan->buffer));
}


/* lowpan_iphc_pop_cid **************************************************************************//**
 * @brief		Returns the context identifier if set. */
static uint8_t lowpan_iphc_pop_cid(Lowpan* lowpan, uint16_t iphc)
{
	/* TODO: need to do further bitmasks to get SCI and DCI */
	if((iphc & LOWPAN_IPHC_CID_MASK) == LOWPAN_IPHC_CID_NONE) {
		return 0;
	} else {
		return be_get_u8(buffer_pop_u8(&lowpan->buffer));
	}
}


/* lowpan_iphc_pop_tcfl *************************************************************************//**
 * @brief		Reads the explicit congestion notification, differentiated services code point, and
 * 				flow label from the lowpan header and writes them into the ipv6 header. */
static void lowpan_iphc_pop_tcfl(IPPacket* packet, Lowpan* lowpan, uint16_t iphc)
{
	/* 6LOWPAN Traffic Class/Flow Label fields:
	 *
	 * 		 0                   1                   2                   3
	 * 		 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 		|ECN|   DSCP    |  rsv  |             Flow Label                |
	 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 		 Figure 4: TF = 00: Traffic Class and Flow Label carried in-line
	 *
	 * 		 0                   1                   2
	 * 		 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
	 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 		|ECN|rsv|             Flow Label                |
	 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 		Figure 5: TF = 01: Flow Label carried in-line
	 *
	 * 		 0 1 2 3 4 5 6 7
	 * 		+-+-+-+-+-+-+-+-+
	 * 		|ECN|   DSCP    |
	 * 		+-+-+-+-+-+-+-+-+
	 * 		Figure 6: TF = 10: Traffic Class carried in-line
	 *
	 * IPv6 Traffic Class:
	 *
	 * 		 0                   1                   2                   3
	 * 		 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 		|Version| Traffic Class |           Flow Label                  |
	 * 		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 *  	        | DSCP      |ECN|
	 *  	        +---------------+
	 */
	uint8_t  buf[4] = { 0 };
	uint8_t  ecn    = 0;
	uint8_t  dscp   = 0;
	uint32_t flow   = 0;

	if((iphc & LOWPAN_IPHC_TF_MASK) == LOWPAN_IPHC_TF_TC_FL)
	{
		buffer_pop_mem(&lowpan->buffer, buf, 4);
		ecn  = buf[0] >> 6;
		dscp = buf[0] << 2;
		flow |= (uint32_t)(buf[1] & 0xF) << 16;
		flow |= (uint32_t)(buf[2])       << 8;
		flow |= (uint32_t)(buf[3])       << 0;
	}
	else if((iphc & LOWPAN_IPHC_TF_MASK) == LOWPAN_IPHC_TF_FL)
	{
		buffer_pop_mem(&lowpan->buffer, buf, 3);
		ecn = buf[0] >> 6;
		flow |= (uint32_t)(buf[0] & 0xF) << 16;
		flow |= (uint32_t)(buf[1])       << 8;
		flow |= (uint32_t)(buf[2])       << 0;
	}
	else if((iphc & LOWPAN_IPHC_TF_MASK) == LOWPAN_IPHC_TF_TC)
	{
		buffer_pop_mem(&lowpan->buffer, buf, 1);
		ecn  = buf[0] >> 6;
		dscp = buf[0] << 2;
	}

	ipv6_set_traffic_class(packet, ecn | dscp);
	ipv6_set_flow_label   (packet, flow);
}


/* lowpan_iphc_pop_nh ***************************************************************************//**
 * @brief		Reads the next header field from the lowpan header and writes it into the ipv6
 * 				header. */
static void lowpan_iphc_pop_nh(IPPacket* packet, Lowpan* lowpan, uint16_t iphc)
{
	if((iphc & LOWPAN_IPHC_NH_MASK) == LOWPAN_IPHC_NH_INLINE)
	{
		ipv6_set_next_header(packet, be_get_u8(buffer_pop_u8(&lowpan->buffer)));
	}
	/* TODO */
//	else
//	{
//		LOG_ERR("compressed next header field unsupported");
//	}
}


/* lowpan_iphc_pop_hlim *************************************************************************//**
 * @brief		Reads the hop limit field from the lowpan header and writes it into the ipv6
 * 				header. */
static void lowpan_iphc_pop_hlim(IPPacket* packet, Lowpan* lowpan, uint16_t iphc)
{
	if((iphc & LOWPAN_IPHC_HLIM_MASK) == LOWPAN_IPHC_HLIM_1) {
		ipv6_set_hop_limit(packet, 1);
	} else if((iphc & LOWPAN_IPHC_HLIM_MASK) == LOWPAN_IPHC_HLIM_64) {
		ipv6_set_hop_limit(packet, 64);
	} else if((iphc & LOWPAN_IPHC_HLIM_MASK) == LOWPAN_IPHC_HLIM_255) {
		ipv6_set_hop_limit(packet, 255);
	} else {
		ipv6_set_hop_limit(packet, be_get_u8(buffer_pop_u8(&lowpan->buffer)));
	}
}


/* lowpan_iphc_pop_src **************************************************************************//**
 * @brief		Decompresses the source address from the lowpan header. */
static void lowpan_iphc_pop_src(IPAddress* src, Lowpan* lowpan, uint16_t iphc, uint8_t cid)
{
	/* Get the context. Defaults to link local context (contex id == 0). */
	uint16_t frame_src_len;
	uint8_t sci = (cid & LOWPAN_IPHC_CID_SCI_MASK) >> LOWPAN_IPHC_CID_SCI_SHIFT;
	const IPAddress* addr = lowpan_ctx_search_id(sci);

	memmove(src, addr->data, sizeof(IPAddress));

	switch(iphc & (LOWPAN_IPHC_SAC_MASK | LOWPAN_IPHC_SAM_MASK)) {
	/* Full 128-bit address inline */
	case LOWPAN_IPHC_SAC_STATELESS | LOWPAN_IPHC_SAM_SL_128:
		buffer_pop_mem(&lowpan->buffer, &src->data[0], 16);
		break;

	/* The unspecified address (::) */
	case LOWPAN_IPHC_SAC_STATEFUL | LOWPAN_IPHC_SAM_SF_UNSPEC:
		memset(&src->data[0], 0, 16);
		break;

	/* 64 bits link local or from context, 64-bit inline */
	case LOWPAN_IPHC_SAC_STATELESS | LOWPAN_IPHC_SAM_SL_64:
	case LOWPAN_IPHC_SAC_STATEFUL  | LOWPAN_IPHC_SAM_SF_64:
		buffer_pop_mem(&lowpan->buffer, &src->data[8], 8);
		break;

	/* 64 bits link local or from context, 0000:00ff:fe00:XXXX */
	case LOWPAN_IPHC_SAC_STATELESS | LOWPAN_IPHC_SAM_SL_16:
	case LOWPAN_IPHC_SAC_STATEFUL  | LOWPAN_IPHC_SAM_SF_16:
		src->data[11] = 0xFF;
		src->data[12] = 0xFE;
		buffer_pop_mem(&lowpan->buffer, &src->data[14], 2);
		break;

	/* 64 bits link local or from context, 64-bit from encapsulating frame */
	case LOWPAN_IPHC_SAC_STATELESS | LOWPAN_IPHC_SAM_SL_0:
	case LOWPAN_IPHC_SAC_STATEFUL  | LOWPAN_IPHC_SAM_SF_0:
		frame_src_len = ieee154_length_src_addr(lowpan_frame(lowpan));
		memmove(&src->data[16-frame_src_len], ieee154_src_addr(lowpan_frame(lowpan)), frame_src_len);
		if(frame_src_len == 2)
		{
			/* IID in the form of: 0000:00ff:fe00:XXXX */
			src->data[11] = 0xFF;
			src->data[12] = 0xFE;
		}
		else if(frame_src_len != 8)
		{
			return; // false;
		}
		break;

	default: return; // false;
	}

	return; // true;
}


/* lowpan_iphc_pop_dest ************************************************************************//**
 * @brief		Decompresses the destination address from the lowpan header. */
static bool lowpan_iphc_pop_dest(IPAddress* dest, Lowpan* lowpan, uint16_t iphc, uint8_t cid)
{
	/* Get the context. Defaults to link local context (contex id == 0). */
	uint16_t frame_dest_len;
	uint8_t dci = (cid & LOWPAN_IPHC_CID_DCI_MASK) >> LOWPAN_IPHC_CID_DCI_SHIFT;
	const IPAddress* addr = lowpan_ctx_search_id(dci);

	memmove(&dest->data[0], addr->data, sizeof(IPAddress));

	switch(iphc & (LOWPAN_IPHC_DAC_MASK | LOWPAN_IPHC_M_MASK | LOWPAN_IPHC_DAM_MASK)) {
	/* Full 128-bit address inline */
	case LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_M_NOT_MULTICAST | LOWPAN_IPHC_DAM_SL_128:
	case LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_M_MULTICAST     | LOWPAN_IPHC_DAM_MSL_128:
		buffer_pop_mem(&lowpan->buffer, &dest->data[0], 16);
		break;

	/* 64-bit link local or from context, 64-bit inline */
	case LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_M_NOT_MULTICAST | LOWPAN_IPHC_DAM_SL_64:
	case LOWPAN_IPHC_DAC_STATEFUL  | LOWPAN_IPHC_M_NOT_MULTICAST | LOWPAN_IPHC_DAM_SF_64:
		buffer_pop_mem(&lowpan->buffer, &dest->data[8], 8);
		break;

	/* 64-bit link local or from context, 0000:00ff:fe00:XXXX */
	case LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_M_NOT_MULTICAST | LOWPAN_IPHC_DAM_SL_16:
	case LOWPAN_IPHC_DAC_STATEFUL  | LOWPAN_IPHC_M_NOT_MULTICAST | LOWPAN_IPHC_DAM_SF_16:
		dest->data[11] = 0xFF;
		dest->data[12] = 0xFE;
		buffer_pop_mem(&lowpan->buffer, &dest->data[14], 2);
		break;

	/* 64-bit link local or from context, 64-bit encapsulating */
	case LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_M_NOT_MULTICAST | LOWPAN_IPHC_DAM_SL_0:
	case LOWPAN_IPHC_DAC_STATEFUL  | LOWPAN_IPHC_M_NOT_MULTICAST | LOWPAN_IPHC_DAM_SF_0:
		frame_dest_len = ieee154_length_dest_addr(lowpan_frame(lowpan));
		memmove(&dest->data[16-frame_dest_len],
			ieee154_dest_addr(lowpan_frame(lowpan)), frame_dest_len);
		if(frame_dest_len == 2)
		{
			/* IID in the form of: 0000:00ff:fe00:XXXX */
			dest->data[11] = 0xFF;
			dest->data[12] = 0xFE;
		}
		else if(frame_dest_len != 8)
		{
			return false;
		}
		break;

	/* ffXX::00XX:XXXX:XXXX */
	case LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_M_MULTICAST | LOWPAN_IPHC_DAM_MSL_48:
		dest->data[0] = 0xFF;
		dest->data[1] = be_get_u8(buffer_pop_u8(&lowpan->buffer));
		buffer_pop_mem(&lowpan->buffer, &dest->data[11], 5);
		break;

	/* ffXX::00XX:XXXX */
	case LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_M_MULTICAST | LOWPAN_IPHC_DAM_MSL_32:
		dest->data[0] = 0xFF;
		dest->data[1] = be_get_u8(buffer_pop_u8(&lowpan->buffer));
		buffer_pop_mem(&lowpan->buffer, &dest->data[13], 3);
		break;

	/* ff02::00XX */
	case LOWPAN_IPHC_DAC_STATELESS | LOWPAN_IPHC_M_MULTICAST | LOWPAN_IPHC_DAM_MSL_8:
		dest->data[0] = 0xFF;
		dest->data[1] = 0x02;
		dest->data[15] = be_get_u8(buffer_pop_u8(&lowpan->buffer));
		break;

	/* ffXX:XXLL:PPPP:PPPP:PPPP:PPPP:XXXX:XXXX. L and P are taken from the specified context. Context
	 * is already copied to the output parameter. @note: context is a full 128-bit IPv6 address. It
	 * is intended that it overlays exactly with the above multicast address. That is, the context is
	 * not shifted so that the prefix is located at index 0. */
	case LOWPAN_IPHC_DAC_STATEFUL | LOWPAN_IPHC_M_MULTICAST | LOWPAN_IPHC_DAM_MSF_48:
		dest->data[0] = 0xFF;
		dest->data[1] = be_get_u8(buffer_pop_u8(&lowpan->buffer));
		dest->data[2] = be_get_u8(buffer_pop_u8(&lowpan->buffer));
		dest->data[3] = addr->data[3] > 64 ? 64 : addr->data[3];
		buffer_pop_mem(&lowpan->buffer, &dest->data[12], 4);
		break;

	default: return false;
	}

	return true;
}


/******************************************* END OF FILE *******************************************/
