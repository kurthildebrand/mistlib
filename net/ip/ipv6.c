/************************************************************************************************//**
 * @file		ipv6.c
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
#include <string.h>

#include "byteorder.h"
#include "ipv6.h"


/* Private Functions ----------------------------------------------------------------------------- */
static void     ipv6_set_version       (IPPacket*);
static void     ipv6_update_length     (Buffer*);
static uint8_t  ipv6_eh_read_next_hdr  (const Buffer*, const uint8_t*);
static uint16_t ipv6_eh_read_length    (const Buffer*, const uint8_t*, uint8_t);
static void     ipv6_eh_set_type       (IPExthdr*, uint8_t);
static void     ipv6_eh_set_length     (IPExthdr*, unsigned);
static uint8_t  ipv6_opt_length_content(const IPOption*);
static uint8_t  ipv6_opt_read_type     (const Buffer*, const uint8_t*);
static uint8_t  ipv6_opt_read_length   (const Buffer*, uint8_t*, uint8_t);
static void     ipv6_opt_pad           (IPOption*, uint8_t*, unsigned);


/* Private Variables ----------------------------------------------------------------------------- */
static const IPAddress ipv6_unspec_addr = {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }};


/* ipv6_addr_is_unspec **************************************************************************//**
 * @brief		Returns true if the IPv6 address is the unspecified address ::. */
bool ipv6_addr_is_unspec(const IPAddress* addr)
{
	return memcmp(addr->data, ipv6_unspec_addr.data, 16) == 0;
}


/* ipv6_addr_is_loopback ************************************************************************//**
 * @brief		Returns true if the IPv6 address is the loopback address ::1. */
bool ipv6_addr_is_loopback(const IPAddress* addr)
{
	return memcmp(addr->data, ipv6_unspec_addr.data, 15) == 0 && addr->data[15] == 1;
}


/* ipv6_addr_is_link_local **********************************************************************//**
 * @brief		Returns true if the IPv6 address is link local FE80::/10. */
bool ipv6_addr_is_link_local(const IPAddress* addr)
{
	return addr->data[0] == 0xFE && (addr->data[1] & 0xC0) == 0x80;
}


/* ipv6_addr_is_multicast ***********************************************************************//**
 * @brief		Returns true if the IPv6 address is a multicast address FF00::/8. */
bool ipv6_addr_is_multicast(const IPAddress* addr)
{
	return addr->data[0] == 0xFF;
}


/* ipv6_addr_compare ****************************************************************************//**
 * @brief		Compares two IPv6 addresses. The magnitude of the returned value indicates the match
 * 				length in bits. The sign of the returned value indicates if a <, =, > than b. */
int ipv6_addr_compare(const void* a, const void* b)
{
	const uint8_t* first  = a;
	const uint8_t* second = b;

	unsigned i, j;

	for(i = 0; i < 16; i++)
	{
		if(first[i] != second[i])
		{
			for(j = 0; j < 8; j++)
			{
				if((first[i] & (1 << (7-j))) != (second[i] & (1 << (7-j))))
				{
					return (128 - (i * 8) - j) * (first[i] < second[i] ? -1 : +1);
				}
			}
		}
	}

	return 0;
}


/* ipv6_init ************************************************************************************//**
 * @brief		Initializes a packet's data. */
void* ipv6_init(IPPacket* pkt, void* data, unsigned count, unsigned size)
{
	return buffer_init(&pkt->buffer, data, count, size);
}


/* ipv6_set_length ******************************************************************************//**
 * @brief		Initializes the length of the packet and returns a pointer to the start of the
 * 				packet's buffer. Call ipv6_parse  after copying data into the reserved bytes. */
void* ipv6_set_length(IPPacket* pkt, unsigned length)
{
	return buffer_set_length(&pkt->buffer, length);
}


/* ipv6_parse ***********************************************************************************//**
 * @brief		*/
void ipv6_parse(IPPacket* pkt)
{
	(void)(pkt);
}


/* ipv6_clear ***********************************************************************************//**
 * @brief		Clears any existing data in the packet and initializes an empty packet with version
 * 				set and the next header set to NO NEXT HEADER. */
void ipv6_clear(IPPacket* pkt)
{
	buffer_clear(&pkt->buffer);
	buffer_set_length(&pkt->buffer, 40);
	ipv6_set_version(pkt);
	ipv6_set_next_header(pkt, IPV6_NO_NEXT_HEADER);
}


/* ipv6_finalize ********************************************************************************//**
 * @brief		Performs final tasks after writing to an IPv6 packet.
 * @desc		1.	Updates the length of the packet. */
void ipv6_finalize(IPPacket* packet)
{
	ipv6_update_length(&packet->buffer);
}


/* ipv6_length **********************************************************************************//**
 * @brief		Returns the length of the whole IPv6 packet in bytes. */
unsigned ipv6_length(const IPPacket* packet)
{
	return buffer_length(&packet->buffer);
}


/* ipv6_size ************************************************************************************//**
 * @brief		Returns the total capacity of the IPv6 packet's underlying buffer in bytes. */
unsigned ipv6_size(const IPPacket* packet)
{
	return buffer_size(&packet->buffer);
}


/* ipv6_checksum ********************************************************************************//**
 * @brief		Computes the IPv6 checksum on the specified data. */
uint16_t ipv6_checksum(const void* data, unsigned len, uint16_t sum)
{
	const uint8_t* ptr = data;
	const uint8_t* end = ptr + len;

	while(ptr < end)
	{
		uint16_t temp = 0;
		temp += (ptr < end ? (*ptr++) << 8 : 0);	/* Read 1st byte */
		temp += (ptr < end ? (*ptr++) << 0 : 0);	/* Read 2nd byte */
		sum  += temp;
		sum  += sum < temp;	/* 1's complement addition: add 1 if carry occurs */
	}

	return sum;
}


/* ipv6_ptr_start *******************************************************************************//**
 * @brief		Returns a pointer to the start of the IPv6 packet's buffer. */
void* ipv6_ptr_start(const IPPacket* packet)
{
	return buffer_start(&packet->buffer);
}


/* ipv6_version *********************************************************************************//**
 * @brief		Returns the packet's version number. */
uint8_t ipv6_version(const IPPacket* packet)
{
	uint8_t version = 0;
	buffer_read_offset(&packet->buffer, &version, 0, 1);
	return (version >> 4) & 0x0F;
}


/* ipv6_traffic_class ***************************************************************************//**
 * @brief		Returns the packet's traffic class. */
uint8_t ipv6_traffic_class(const IPPacket* packet)
{
	/* 	 0                   1                   2                   3
	 * 	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 	|Version| Traffic Class |           Flow Label                  |
	 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 *   <------0------> <------1------> <------2------> <------3------> */
	uint8_t buf[2];
	buffer_read_offset(&packet->buffer, buf, 0, 2);
	return ((buf[0] & 0x0F) << 4) | ((buf[1] & 0xF0) >> 4);
}


/* ipv6_flow_label ******************************************************************************//**
 * @brief		Returns the packet's flow label. */
uint32_t ipv6_flow_label(const IPPacket* packet)
{
	/* 	 0                   1                   2                   3
	 * 	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 	|Version| Traffic Class |           Flow Label                  |
	 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 *   <------0------> <------1------> <------2------> <------3------> */
	uint8_t buf[3] = { 0 };
	buffer_read_offset(&packet->buffer, buf, 1, 3);
	return ((buf[0] & 0x0F) << 16) | (buf[1] << 8) | (buf[2] << 0);
}


/* ipv6_payload_length **************************************************************************//**
 * @brief		Get the payload length from the packet. */
uint16_t ipv6_payload_length(const IPPacket* packet)
{
	/* Read the length from the packet. Bypass buffer functions as the buffer bounds may not have
	 * been set yet. */
	return ntoh_get_u16(buffer_peek_offset(&packet->buffer, 4, 2));
}


/* ipv6_next_header *****************************************************************************//**
 * @brief		Returns the next header tag in the packet. */
uint8_t ipv6_next_header(const IPPacket* packet)
{
	uint8_t next_header = 0;
	buffer_read_offset(&packet->buffer, &next_header, 6, 1);
	return next_header;
}


/* ipv6_hop_limit *******************************************************************************//**
 * @brief		Returns the packet's hop limit. */
uint8_t ipv6_hop_limit(const IPPacket* packet)
{
	uint8_t hop_limit = 0;
	buffer_read_offset(&packet->buffer, &hop_limit, 7, 1);
	return hop_limit;
}


/* ipv6_src *************************************************************************************//**
 * @brief		Returns a pointer to the packet's source address. */
IPAddress* ipv6_src(const IPPacket* packet)
{
	return buffer_peek_offset(&packet->buffer, 8, 8);
}


/* ipv6_dest ************************************************************************************//**
 * @brief		Returns a pointer to the packet's destination address. */
IPAddress* ipv6_dest(const IPPacket* packet)
{
	return buffer_peek_offset(&packet->buffer, 24, 8);
}


/* ipv6_set_version *****************************************************************************//**
 * @brief		Sets the packet's version number to 6 (for IPv6). */
static void ipv6_set_version(IPPacket* packet)
{
	/* 	 0                   1                   2                   3
	 * 	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 	|Version| Traffic Class |           Flow Label                  |
	 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 *   <------0------> <------1------> <------2------> <------3------> */
	uint8_t version = 0;
	buffer_read_offset(&packet->buffer, &version, 0, 1);
	version = (6 << 4) | (version & 0x0F);
	buffer_replace_offset(&packet->buffer, &version, 0, 1);
}


/* ipv6_set_traffic_class ***********************************************************************//**
 * @brief		Sets the packet's traffic class. */
bool ipv6_set_traffic_class(IPPacket* packet, uint8_t traffic_class)
{
	/* 	 0                   1                   2                   3
	 * 	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 	|Version| Traffic Class |           Flow Label                  |
	 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 *   <------0------> <------1------> <------2------> <------3------> */
	uint8_t buf[2];
	buffer_read_offset(&packet->buffer, buf, 0, 2);
	buf[0] = (buf[0] & 0xF0) | ((traffic_class >> 4) & 0x0F);
	buf[1] = ((traffic_class & 0x0F) << 4) | (buf[1] & 0x0F);
	buffer_replace_offset(&packet->buffer, buf, 0, 2);
	return true;
}


/*ipv6_set_flow_label ***************************************************************************//**
 * @brief		Sets the packet's flow label. */
bool ipv6_set_flow_label(IPPacket* packet, uint32_t flow_label)
{
	/* 	 0                   1                   2                   3
	 * 	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * 	|Version| Traffic Class |           Flow Label                  |
	 * 	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 *   <------0------> <------1------> <------2------> <------3------> */
	uint8_t buf[3];
	buffer_read_offset(&packet->buffer, buf, 1, 3);
	buf[0] = (buf[0] & 0xF0) | ((flow_label >> 16) & 0x0F);
	buf[1] = (flow_label >> 8) & 0xFF;
	buf[2] = (flow_label >> 0) & 0xFF;
	buffer_replace_offset(&packet->buffer, buf, 1, 3);
	return true;
}


/* ipv6_update_length ***************************************************************************//**
 * @brief		Updates the IPv6 packet's payload length based on the buffer_length of the packet's
 * 				underlying buffer. */
static void ipv6_update_length(Buffer* packet)
{
	/* Payload length is the length of the rest of the packet following the IPv6 header (which is
	 * always 40 bytes long). Note that extension headers are considered part of the payload. */
	uint16_t length = hton_u16(buffer_length(packet) - 40);
	buffer_replace_offset(packet, &length, 4, 2);
}


/* ipv6_set_next_header *************************************************************************//**
 * @brief		Sets the packet's next header. Todo: make static? */
bool ipv6_set_next_header(IPPacket* packet, uint8_t next_header)
{
	return buffer_replace_offset(&packet->buffer, &next_header, 6, 1);
}


/* ipv6_set_hop_limit ***************************************************************************//**
 * @brief		Sets the packet's hop limit. */
bool ipv6_set_hop_limit(IPPacket* packet, uint8_t hop_limit)
{
	return buffer_replace_offset(&packet->buffer, &hop_limit, 7, 1);
}


/* ipv6_set_src *********************************************************************************//**
 * @brief		 Sets the packet's source address (128 bits). */
bool ipv6_set_src(IPPacket* packet, const IPAddress* addr)
{
	return buffer_replace_offset(&packet->buffer, addr, 8, 16);
}


/* ipv6_set_dest ********************************************************************************//**
 * @brief		Sets the packet's destination address (128 bits).*/
bool ipv6_set_dest(IPPacket* packet, const IPAddress* addr)
{
	return buffer_replace_offset(&packet->buffer, addr, 24, 16);
}





// ----------------------------------------------------------------------------------------------- //
// IPv6 Extension Headers                                                                          //
// ----------------------------------------------------------------------------------------------- //
/* ipv6_eh_can_frag *****************************************************************************//**
 * @brief		Returns true if the header type can be fragmented. */
bool ipv6_eh_can_frag(uint8_t type)
{
	return type != IPV6_HBH && type != IPV6_ROUTING;
}


/* ipv6_eh_is_upper *****************************************************************************//**
 * @brief		Returns true if the type is an upper layer extension header. */
bool ipv6_eh_is_upper(uint8_t type)
{
	return (type != IPV6_HBH)                 &&
	       (type != IPV6_ROUTING)             &&
	       (type != IPV6_FRAG_HEADER)         &&
	       (type != IPV6_ENCAP_SEC_PROTOCOL)  &&
	       (type != IPV6_AUTH_HEADER)         &&
	       (type != IPV6_NO_NEXT_HEADER)      &&
	       (type != IPV6_DEST_OPTS_HEADER)    &&
	       (type != IPV6_MOBILITY)            &&
	       (type != IPV6_HOST_IDENT_PROTOCOL) &&
	       (type != IPV6_SHIM6_PROTOCOL)      &&
	       (type != IPV6_RES1)                &&
	       (type != IPV6_RES2);
}


/* ipv6_eh_pkt **********************************************************************************//**
 * @brief		Returns IPv6 extension header's underlying packet. */
IPPacket* ipv6_eh_pkt(const IPExthdr* eh)
{
	return (IPPacket*)buffer_top_parent(&eh->buffer);
}


/* ipv6_eh_first ********************************************************************************//**
 * @brief		Returns the first extension header in the packet. */
IPExthdr ipv6_eh_first(IPPacket* packet)
{
	IPExthdr eh;

	uint8_t* prev  = buffer_offset(&packet->buffer, 6);
	uint8_t* start = buffer_offset(&packet->buffer, 40);
	uint8_t  type  = ipv6_eh_read_next_hdr(&packet->buffer, prev);
	unsigned len   = ipv6_eh_read_length(&packet->buffer, start, type);

	eh.prev = prev;
	buffer_slice(&eh.buffer, &packet->buffer, start, len);
	ipv6_eh_reset_buffer(&eh);
	return eh;
}


/* ipv6_eh_next *********************************************************************************//**
 * @brief		Returns the next extension header in the packet if it exists. */
bool ipv6_eh_next(IPExthdr* eh)
{
	if(!ipv6_eh_is_valid(eh))
	{
		return false;
	}
	else if(ipv6_eh_is_upper(ipv6_eh_type(eh)))
	{
		eh->prev = buffer_start(&eh->buffer);
		buffer_slice(&eh->buffer, buffer_parent(&eh->buffer), buffer_end(&eh->buffer), 0);
		return false;
	}
	else
	{
		/* Type and Length of the current extension header */
		uint8_t  oldtype  = ipv6_eh_type(eh);
		uint16_t oldlen   = ipv6_eh_read_length(&eh->buffer, buffer_start(&eh->buffer), oldtype);

		/* Move to the next extenion header */
		eh->prev = buffer_start(&eh->buffer);
		uint8_t* newstart = buffer_start(&eh->buffer) + oldlen;
		uint8_t  newtype  = ipv6_eh_read_next_hdr(&eh->buffer, buffer_start(&eh->buffer));
		uint16_t newlen   = ipv6_eh_read_length(buffer_parent(&eh->buffer), newstart, newtype);

		buffer_slice(&eh->buffer, buffer_parent(&eh->buffer), newstart, newlen);
		buffer_read_seek(&eh->buffer, 2);
		return true;
	}
}


/* ipv6_eh_is_valid *****************************************************************************//**
 * @brief		Returns true if the header is valid. */
bool ipv6_eh_is_valid(const IPExthdr* eh)
{
	return buffer_is_valid(&eh->buffer) && buffer_start(&eh->buffer) < buffer_write(&eh->buffer);
}


/* ipv6_eh_type *********************************************************************************//**
 * @brief		Returns the header's type.
 * @warning		Expects a valid extension header. */
uint8_t ipv6_eh_type(const IPExthdr* eh)
{
	return ipv6_eh_read_next_hdr(buffer_parent(&eh->buffer), eh->prev);
}


/* ipv6_eh_length *******************************************************************************//**
 * @brief		Returns the total number of bytes of the extension header. */
uint16_t ipv6_eh_length(const IPExthdr* eh)
{
	return buffer_length(&eh->buffer);
}


/* ipv6_eh_next_hdr *****************************************************************************//**
 * @brief		Reads the next header field from the extension header pointed to by start. */
static uint8_t ipv6_eh_read_next_hdr(const Buffer* b, const uint8_t* start)
{
	uint8_t nexthdr = IPV6_INVALID;
	buffer_read_at(b, &nexthdr, start, 1);
	return nexthdr;
}


/* ipv6_eh_read_length **************************************************************************//**
 * @brief		Reads the length of the extension header pointed to by start. */
static uint16_t ipv6_eh_read_length(const Buffer* b, const uint8_t* start, uint8_t type)
{
	// uint8_t length = 0;

	if(!buffer_is_valid(b) || type == IPV6_NO_NEXT_HEADER || type == IPV6_INVALID)
	{
		return 0;
	}
	else if(ipv6_eh_is_upper(type))
	{
		return (uint16_t)(buffer_write(b) - start);
	}
	else if(type == IPV6_FRAG_HEADER)
	{
		return 8;
	}
	else
	{
		/* Note: start is the start of the EH */
		// buffer_read_at(b, &length, start + 1, 1);

		return 8 + 8 * ntoh_get_u8(buffer_peek_at(b, start + 1, 1));
	}

	// return 8 + 8 * length;

}


/* ipv6_eh_prepend ******************************************************************************//**
 * @brief		Inserts an extension header before the specified header. */
bool ipv6_eh_prepend(IPExthdr* eh, uint8_t type, const void* in, unsigned len)
{
	if(!buffer_is_valid(&eh->buffer) || !buffer_parent(&eh->buffer))
	{
		return false;
	}

	if(ipv6_eh_is_upper(type))
	{
		/* Only prepend if there is no upper layer header (IPV6_NO_NEXT_HEADER) */
		if(ipv6_eh_type(eh) != IPV6_NO_NEXT_HEADER)
		{
			return false;
		}
		else if(buffer_tailroom(&eh->buffer) < len)
		{
			return false;
		}

		ipv6_eh_set_type(eh, type);
		buffer_slice    (&eh->buffer, buffer_parent(&eh->buffer), buffer_start(&eh->buffer), 0);
		buffer_push_mem (&eh->buffer, in, len);
		return true;
	}
	else
	{
		/* Inserting a new extension header requires 2 extra bytes (1 for next header field + 1 for
		 * extension header length). */
		if(buffer_tailroom(&eh->buffer) < len + 2)
		{
			return false;
		}

		/* Save the old header's type */
		uint8_t next = ipv6_eh_type(eh);

		ipv6_eh_set_type     (eh, type);
		buffer_slice         (&eh->buffer, buffer_parent(&eh->buffer), buffer_start(&eh->buffer), 0);
		buffer_reserve       (&eh->buffer, len + 2);
		buffer_replace_offset(&eh->buffer, &next, 0, 1);
		buffer_replace_offset(&eh->buffer, in,    2, len);
		buffer_read_seek     (&eh->buffer, 2);
		return true;
	}
}


/* ipv6_append_exthdr ***************************************************************************//**
 * @brief		Inserts an extension header after the specified header. */
bool ipv6_eh_append(IPExthdr* eh, uint8_t type, const void* in, unsigned len)
{
	if(ipv6_eh_is_upper(ipv6_eh_type(eh)))
	{
		return false;
	}

	ipv6_eh_next(eh);

	return ipv6_eh_prepend(eh, type, in, len);


//	return !ipv6_eh_is_upper(ipv6_eh_type(eh)) &&
//	        ipv6_eh_next(eh), ipv6_eh_prepend(eh, type, in, len);
}


/* ipv6_eh_reset_buffer *************************************************************************//**
 * @brief		Returns the extension header's buffer. The buffer's read and write pointers are reset
 * 				so that bytes can be read from the start of and written to the end of the extension
 * 				header's content.
 * @warning		Extension headers may be padded.
 * @warning		Call ipv6_eh_finalize after writing bytes to the EH. */
Buffer* ipv6_eh_reset_buffer(IPExthdr* eh)
{
	if(ipv6_eh_is_upper(ipv6_eh_type(eh)))
	{
		buffer_read_seek(&eh->buffer, 0);
		return &eh->buffer;
	}
	else
	{
		buffer_read_seek(&eh->buffer, 2);
		return &eh->buffer;
	}
}


/* ipv6_eh_finalize *****************************************************************************//**
 * @brief		Performs final tasks after writing to an IPv6 extension header.
 * @desc		1.	Updates the extension header's length.
 * 				2.	Pads the extension header. */
void ipv6_eh_finalize(IPExthdr* eh)
{
	if(!ipv6_eh_is_upper(ipv6_eh_type(eh)))
	{
		/* Pad the EH */
		unsigned end = buffer_length(&eh->buffer);
		unsigned pad = (end + 7) / 8 * 8;
		buffer_reserve(&eh->buffer, pad - end);

		/* Set the length of the EH */
		ipv6_eh_set_length(eh, buffer_length(&eh->buffer));
	}
}


/* ipv6_eh_set_type *****************************************************************************//**
 * @brief		Sets this header's type by writing the previous extension header's next header
 * 				field. */
static void ipv6_eh_set_type(IPExthdr* eh, uint8_t type)
{
	buffer_replace_at(buffer_parent(&eh->buffer), &type, eh->prev, 1);
}


/* ipv6_eh_set_length ***************************************************************************//**
 * @brief		Sets the extension header's length.
 * @param[in]	header: the header to set the length.
 * @param[in]	len: the total number of bytes currently in the header. */
static void ipv6_eh_set_length(IPExthdr* eh, unsigned len)
{
	/* Length of the header in 8-octet units, not including the first 8 octets */
	uint8_t hlen = (len + 7) / 8 - 1;

	/* Header extension length field is located at byte offset 1 from the start of this header */
	buffer_replace_offset(&eh->buffer, &hlen, 1, 1);
}





// ----------------------------------------------------------------------------------------------- //
// IPv6 Options                                                                                    //
// ----------------------------------------------------------------------------------------------- //
/* ipv6_opt_eh **********************************************************************************//**
 * @brief		Returns the TLV option's parent extension header.
 * @warning		Assumes options are not nested (option containing option). */
IPExthdr* ipv6_opt_eh(IPOption* opt)
{
	return (IPExthdr*)buffer_parent(&opt->buffer);
}


/* ipv6_opt_read ********************************************************************************//**
 * @brief		*/
IPOption ipv6_opt_read(const Buffer* b, uint8_t* start)
{
	IPOption opt;
	uint8_t  type = ipv6_opt_read_type  (b, start);
	uint8_t  len  = ipv6_opt_read_length(b, start, type);
	buffer_slice    (&opt.buffer, b, start, len);
	buffer_read_seek(&opt.buffer, 2);
	return opt;
}


/* ipv6_opt_first *******************************************************************************//**
 * @brief		Returns the first TLV option in the header. */
IPOption ipv6_opt_first(const IPExthdr* eh)
{
	if(!ipv6_eh_is_valid(eh) || ipv6_eh_is_upper(ipv6_eh_type(eh)))
	{
		return (IPOption){ .buffer = make_buffer_slice(&eh->buffer, 0, 0), };
	}
	else
	{
		return ipv6_opt_read(&eh->buffer, buffer_offset(&eh->buffer, 2));
	}
}


/* ipv6_opt_next ********************************************************************************//**
 * @brief		Returns the next option in the header if it exists. */
bool ipv6_opt_next(IPOption* opt)
{
	if(!ipv6_opt_is_valid(opt))
	{
		return false;
	}
	else
	{
		Buffer*  parent = buffer_parent       (&opt->buffer);
		uint8_t* start  = buffer_write        (&opt->buffer);
		uint8_t  type   = ipv6_opt_read_type  (parent, start);
		uint8_t  len    = ipv6_opt_read_length(parent, start, type);
		buffer_slice    (&opt->buffer, parent, start, len);
		buffer_read_seek(&opt->buffer, 2);
		return true;
	}
}


/* ipv6_opt_is_valid ****************************************************************************//**
 * @brief		Returns true if the option is valid. */
bool ipv6_opt_is_valid(const IPOption* opt)
{
	return buffer_is_valid(&opt->buffer) && buffer_start(&opt->buffer) < buffer_write(&opt->buffer);
}


/* ipv6_opt_type ********************************************************************************//**
 * @brief		Returns the option's type. */
uint8_t ipv6_opt_type(const IPOption* opt)
{
	if(!ipv6_opt_is_valid(opt)) {
		return IPV6_OPT_TYPE_INVALID;
	} else {
		return ipv6_opt_read_type(&opt->buffer, buffer_start(&opt->buffer));
	}
}


/* ipv6_opt_length ******************************************************************************//**
 * @brief		Returns the total length of the option (type, length, and content). */
uint8_t ipv6_opt_length(const IPOption* opt)
{
	return buffer_length(&opt->buffer);
}


/* ipv6_opt_length_content **********************************************************************//**
 * @brief		Returns the length of the option's content. */
static uint8_t ipv6_opt_length_content(const IPOption* opt)
{
	uint8_t len = ipv6_opt_length(opt);

	if(len < 2) {
		return 0;
	} else {
		return len - 2;
	}
}


/* ipv6_opt_read_type ***************************************************************************//**
 * @brief		Reads the type field from the option pointed to by start. */
static uint8_t ipv6_opt_read_type(const Buffer* b, const uint8_t* start)
{
	uint8_t type = IPV6_OPT_TYPE_INVALID;
	buffer_read_at(b, &type, start, 1);
	return type;
}


/* ipv6_opt_read_length *************************************************************************//**
 * @brief		Reads the length from the option pointed to by start. */
static uint8_t ipv6_opt_read_length(const Buffer* parent, uint8_t* start, uint8_t type)
{
	if(!buffer_is_valid(parent) || type == IPV6_OPT_TYPE_INVALID)
	{
		return 0;
	}

	IPExthdr* eh = (IPExthdr*)parent;

	if(ipv6_eh_is_upper(ipv6_eh_type(eh)))
	{
		return 8 * ntoh_get_u8(buffer_peek_at(parent, start + 1, 1));
	}
	else if(type == IPV6_OPT_TYPE_PAD1)
	{
		return 1;
	}
	else
	{
		return 2 + ntoh_get_u8(buffer_peek_at(parent, start + 1, 1));
	}
}


/* ipv6_opt_append ******************************************************************************//**
 * @brief		Inserts an IPv6 option after the specified option.
 * @param[in]	align: 1, 2, 4, 8. */
bool ipv6_opt_append(IPOption* o, uint8_t type, const void* in, unsigned len, unsigned m, unsigned b)
{
	/* Todo: change align to mx+b */
	/* Todo: init eh buffer slice to set max size */
	/* Todo: init opt buffer slice to set max size */

	// IPExthdr* eh = (IPExthdr*)buffer_parent(&o->buffer);

	if(!buffer_is_valid(&o->buffer))
	{
		return false;
	}
	/* Check that there is enough space in the packet */
	else if(buffer_tailroom(&o->buffer) < len)
	{
		return false;
	}
	// /* Check that type is not PAD1 or PADN. Padding is handled automatically. */
	// else if(!ipv6_eh_is_upper(ipv6_eh_type(eh)) &&
	//        (type == IPV6_OPT_TYPE_PAD1 || type == IPV6_OPT_TYPE_PADN))
	// {
	// 	return false;
	// }
	/* Check that alignment is 1, 2, 4, 8 */
	else if(m != 1 && m != 2 && m != 4 && m != 8)
	{
		return false;
	}

	b %= m;

	Buffer*  eh  = buffer_parent(&o->buffer);
	uint8_t* end = buffer_write(&o->buffer);
	unsigned pad = (m - (buffer_offsetof(eh, end) - b) % m) % m;

	/* Compute the current padding from the end of the opt to the end of the EH. */
	unsigned current = buffer_write(buffer_parent(&o->buffer)) - buffer_write(&o->buffer);

	/* Reserve enough bytes for the padding and the payload. Need to add two bytes for the type and
	 * length fields of the new option as 'len' only refers to the content of the option. Padding
	 * refers to the total number of bytes that need to be added which includes any type and length
	 * fields. */
	if(current < pad + len + 2)
	{
		buffer_reserve(&o->buffer, pad + len + 2 - current);
	}

	ipv6_opt_pad(o, end, pad);

	/* Move option to the new location */
	buffer_slice(&o->buffer, eh, end + pad, len + 2);

	/* Set option. Note: length gets set by ipv6_opt_finalize. */
	buffer_replace_at(&o->buffer, &type, end + pad + 0, 1);
	buffer_replace_at(&o->buffer, in,    end + pad + 2, len);
	buffer_read_seek (&o->buffer, 2);
	return true;
}


/* ipv6_opt_reset_buffer ************************************************************************//**
 * @brief		Returns the IPv6 option's buffer. The buffer's read and write pointers are reset so
 * 				that bytes can be read from the start of and written to the end of the extension
 * 				header's content. */
Buffer* ipv6_opt_reset_buffer(IPOption* opt)
{
	buffer_read_seek(&opt->buffer, 2);
	return &opt->buffer;
}


/* ipv6_opt_finalize ****************************************************************************//**
 * @brief		Performs final tasks after writing to an IPv6 option.
 * @desc		1.	Updates the option's length.
 * 				2.	Pads the extension header to a multiple of 8.
 * 				3.	Pads the end of the option. */
void ipv6_opt_finalize(IPOption* opt)
{
	IPExthdr* eh  = (IPExthdr*)buffer_parent(&opt->buffer);
	uint8_t   len;

	/* Todo: doc */
	if(ipv6_eh_is_upper(ipv6_eh_type(eh)))
	{
		len = (ipv6_opt_length(opt) + 7) / 8;
	}
	else
	{
		len = ipv6_opt_length_content(opt);
	}

	/* Set option length */
	buffer_replace_offset(&opt->buffer, &len, 1, 1);

	/* Pad EH to multiple of 8. Note: leave opt's buffer unmodified by padding opt's parent instead.
	 * At return, buffer_write(&opt->buffer) should still point to the end of the opt's content, not
	 * the end of any potential padding. */
	unsigned end = buffer_offsetof(buffer_parent(&opt->buffer), buffer_write(&opt->buffer));
	unsigned pad = (end + 7) / 8 * 8;

	/* Compute the current padding from the end of the opt to the end of the EH. */
	unsigned current = buffer_write(buffer_parent(&opt->buffer)) - buffer_write(&opt->buffer);

	if(current < pad - end)
	{
		buffer_reserve(buffer_parent(&opt->buffer), pad - end - current);
	}

	/* Pad the OPT */
	ipv6_opt_pad(opt, buffer_write(&opt->buffer), pad - end);
}


/* ipv6_opt_pad *********************************************************************************//**
 * @brief		Sets padding option from [start, end). Expects that space has already been made (e.g.
 * 				with reserve) for the padding bytes. Does not modify opt's buffer. */
static void ipv6_opt_pad(IPOption* opt, uint8_t* start, unsigned len)
{
	IPExthdr* eh = (IPExthdr*)buffer_parent(&opt->buffer);

	/* Set the padding to zero. This implicitly sets PAD1 */
	memset(start, 0, len);

	/* If padding is 2 or more bytes, then set PADN and the length */
	if(!ipv6_eh_is_upper(ipv6_eh_type(eh)) && len >= 2)
	{
		/* PADN option requires two bytes: 1 for type, 1 for length */
		uint8_t padtype = IPV6_OPT_TYPE_PADN;
		uint8_t padlen  = len - 2;

		/* Write the bytes in the parent buffer to leave opt unmodified */
		buffer_replace_at(buffer_parent(&opt->buffer), &padtype, start + 0, 1);
		buffer_replace_at(buffer_parent(&opt->buffer), &padlen,  start + 1, 1);
	}
}





// ----------------------------------------------------------------------------------------------- //
// IPv6 Fragment Extension Header                                                                  //
// ----------------------------------------------------------------------------------------------- //
/* ipv6_frag_eh_prepend *************************************************************************//**
 * @brief		Prepends a fragment extension header.
 * @warning		Requires ipv6_finalize_frag_exthdr to be called after all bytes have been added to
 * 				the fragment.
 * @warning		Assumes that the fragment extension header is being inserted immediately after the
 * 				unfragmentable extension headers.
 * @param[in]	eh: the header to prepend the frag extension header onto.
 * @param[in]	id: the fragment's id.
 * @param[in]	offset: the fragment's offset in bytes, referenced from the start of the packet. For
 * 				example, if this is the first fragment, offset would be 40 assuming no unfragmentable
 * 				extension headers. */
bool ipv6_frag_eh_prepend(IPExthdr* eh, uint32_t id, uint16_t offset)
{
	if(offset % 8 != 0)
	{
		return false;
	}
	else if(!ipv6_eh_prepend(eh, IPV6_FRAG_HEADER, 0, 8))
	{
		return false;
	}
	else
	{
		/* Offset needs to be relative to the start of the fragmentable part of the original packet.
		 * Offset also needs to be in units of 8-octets. Assume that this frag extension header is
		 * being inserted immediately after the unfragmentable part. Therefore, eh->start is the
		 * index to insert the extension header.
		 *
		 * Offset is guaranteed to be a multiple of 8 because of the offset % 8 check above. Being a
		 * multiple of 8 means the 3 LSb are 0. */
		offset -= buffer_offsetof(buffer_parent(&eh->buffer), buffer_start(&eh->buffer));

		ntoh_set_u16(buffer_peek_offset(&eh->buffer, 2, 2), offset);
		ntoh_set_u32(buffer_peek_offset(&eh->buffer, 4, 4), id);
		return true;
	}
}


/* ipv6_frag_eh_append **************************************************************************//**
 * @brief		Appends a fragment extension header.
 * @warning		Requires ipv6_finalize_frag_exthdr to be called after all bytes have been added to
 * 				the fragment.
 * @warning		Assumes that the fragment extension header is being inserted immediately after the
 * 				unfragmentable extension headers.
 * @param[in]	eh: the header to prepend the frag extension header onto.
 * @param[in]	id: the fragment's id.
 * @param[in]	offset: the fragment's offset in bytes, referenced from the start of the packet. For
 * 				example, if this is the first fragment, offset would be 40 assuming no unfragmentable
 * 				extension headers. */
bool ipv6_frag_eh_append(IPExthdr* eh, uint32_t id, uint16_t offset)
{
	return ipv6_eh_next(eh) && ipv6_frag_eh_prepend(eh, id, offset);
}


/* ipv6_frag_eh_finalize ************************************************************************//**
 * @brief		Sets the more fragments bit in the frag extension header.
 * @param[in]	eh: the frag extension header to finalize.
 * @param[in]	total_length: the complete length of the reassembled packet. */
bool ipv6_frag_eh_finalize(IPExthdr* eh, uint16_t total_length)
{
	if(ipv6_eh_type(eh) != IPV6_FRAG_HEADER)
	{
		return false;
	}

	/* Computes if this is the last fragment by examining the length of the remaining bytes that need
	 * to be appended to the packet. The number of remaining bytes = total_length - offset + size of
	 * frag extension header (8). The available length in the packet is retrieved from the extension
	 * header's underlying buffer, which may be smaller than the IPv6 MTU. */
	uint16_t offset = ipv6_frag_eh_offset(eh);
	uint16_t length = buffer_write(buffer_parent(&eh->buffer)) - buffer_write(&eh->buffer);
	bool     islast = length >= total_length || offset + length >= total_length;
	ntoh_set_u16(buffer_peek_offset(&eh->buffer, 2, 2), offset | (!islast));

	return true;
}


/* ipv6_frag_eh_offset **************************************************************************//**
 * @brief		Returns this fragment's offset in bytes. */
uint16_t ipv6_frag_eh_offset(const IPExthdr* eh)
{
	return ntoh_get_u16(buffer_peek_offset(&eh->buffer, 2, 2)) & 0xFFF8;
}


/* ipv6_frag_eh_is_last *************************************************************************//**
 * @brief		Returns true if this is the last fragment. Otherwise returns false. */
bool ipv6_frag_eh_is_last(const IPExthdr* eh)
{
	return ntoh_get_u8(buffer_peek_offset(&eh->buffer, 3, 1)) & 1;
}


/* ipv6_frag_eh_id ******************************************************************************//**
 * @brief		Returns this fragment's id. */
uint32_t ipv6_frag_eh_id(const IPExthdr* eh)
{
	return ntoh_get_u32(buffer_peek_offset(&eh->buffer, 4, 1));
}


/******************************************* END OF FILE *******************************************/
