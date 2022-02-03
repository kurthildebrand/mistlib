/************************************************************************************************//**
 * @file		icmp6.h
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
 * @desc		ref: ICMPv6 https://datatracker.ietf.org/doc/html/rfc4443
 * 				ICMPv6 packets have the following general format:
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                                                               |
 * 					+                         Message Body                          +
 * 					|                                                               |
 *
 * 				The type field indicates the type of the message. Its value determines the format of
 * 				the remaining data.
 *
 * 				The code field depends on the message type. It is used to create an additional level
 * 				of message granularity.
 *
 * 				The checksum field is used to detect data corruption in the ICMPv6 message and parts
 * 				of the IPv6 header.
 *
 * 				ICMPv6 messages are grouped into two classes: error messages and informational
 * 				messages. Error messages are identified as such by a zero in the high-order bit of
 * 				their message Type field values. Thus, error messages have message types from 0 to
 * 				127; informational messages have message types from 128 to 255.
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Destination Unreachable Message
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                             Unused                            |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                    As much of invoking packet                 |
 * 					+                as possible without the ICMPv6 packet          +
 * 					|                exceeding the minimum IPv6 MTU [IPv6]          |
 *
 * 					IPv6 Fields:
 * 						Destination Address
 * 							Copied from the Source Address field of the invoking packet.
 *
 * 					ICMPv6 Fields:
 * 						Type
 * 							1
 *
 * 						Code
 * 							0 - No route to destination
 * 							1 - Communication with destination administratively prohibited
 * 							2 - Beyond scope of source address
 * 							3 - Address unreachable
 * 							4 - Port unreachable
 * 							5 - Source address failed ingress/egress policy
 * 							6 - Reject route to destination
 *
 * 						Unused
 * 							This field is unused for all code values. It must be initialized to zero
 * 							by the originator and ignored by the receiver.
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Packet Too Big Message
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                             MTU                               |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                    As much of invoking packet                 |
 * 					+               as possible without the ICMPv6 packet           +
 * 					|               exceeding the minimum IPv6 MTU [IPv6]           |
 *
 * 					IPv6 Fields:
 * 						Destination Address
 * 							Copied from the Source Address field of the invoking packet.
 *
 * 					ICMPv6 Fields:
 * 						Type
 * 							2
 *
 * 						Code
 * 							Set to 0 (zero) by the originator and ignored by the receiver.
 *
 * 						MTU
 * 							The Maximum Transmission Unit of the next-hop link.
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Time Exceeded Message
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                             Unused                            |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                    As much of invoking packet                 |
 * 					+               as possible without the ICMPv6 packet           +
 * 					|               exceeding the minimum IPv6 MTU [IPv6]           |
 *
 * 					IPv6 Fields:
 * 						Destination Address
 * 							Copied from the Source Address field of the invoking packet.
 *
 * 					ICMPv6 Fields:
 * 						Type
 * 							3
 *
 * 						Code
 * 							0 - Hop limit exceeded in transit
 * 							1 - Fragment reassembly time exceeded
 *
 * 						Unused
 * 							This field is unused for all code values. It must be initialized to
 * 							zero by the originator and ignored by the receiver.
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Parameter Problem Message
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                            Pointer                            |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                    As much of invoking packet                 |
 * 					+               as possible without the ICMPv6 packet           +
 * 					|               exceeding the minimum IPv6 MTU [IPv6]           |
 *
 * 					IPv6 Fields:
 * 						Destination Address
 * 							Copied from the Source Address field of the invoking packet.
 *
 * 					ICMPv6 Fields:
 * 						Type
 * 							4
 *
 * 						Code
 * 							0 - Erroneous header field encountered
 * 							1 - Unrecognized Next Header type encountered
 * 							2 - Unrecognized IPv6 option encountered
 *
 * 						Pointer
 * 							Identifies the octet offset within the invoking packet where the error
 * 							was detected.
 *
 * 							The pointer will point beyond the end of the ICMPv6 packet if the field
 * 							in error is beyond what can fit in the maximum size of an ICMPv6 error
 * 							message.
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Echo Request Message
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|           Identifier          |        Sequence Number        |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Data ...
 * 					+-+-+-+-+-
 *
 * 					IPv6 Fields:
 * 						Destination Address
 * 							Any legal IPv6 address.
 *
 * 					ICMPv6 Fields:
 * 						Type
 * 							128
 *
 * 						Code
 * 							0
 *
 * 						Identifier
 * 							An identifier to aid in matching Echo Replies to this Echo Request. May
 * 							be zero.
 *
 * 						Sequence Number
 * 							A sequence number to aid in matching Echo Replies to this Echo Request.
 * 							May be zero.
 *
 * 						Data
 * 							Zero or more octets of arbitrary data.
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Echo Reply Message
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|           Identifier          |        Sequence Number        |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Data ...
 * 					+-+-+-+-+-
 *
 * 					IPv6 Fields:
 * 						Destination Address
 * 							Copied from the Source Address field of the invoking Echo Request packet.
 *
 * 					ICMPv6 Fields:
 * 						Type
 * 							129
 *
 * 						Code
 * 							0
 *
 * 						Identifier
 * 							The identifier from the invoking Echo Request message.
 *
 * 						Sequence Number
 * 							The sequence number from the invoking Echo Request message.
 *
 * 						Data
 * 							The data from the invoking Echo Request message.
 *
 ***************************************************************************************************/
#ifndef ICMPV6_H
#define ICMPV6_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include "ipv6.h"


/* Public Macros --------------------------------------------------------------------------------- */
#define ICMPV6_DEST_UNREACH                (1)
#define ICMPV6_PACKET_TOO_BIG              (2)
#define ICMPV6_TIME_EXCEEDED               (3)
#define ICMPV6_PARAM_PROBLEM               (4)
#define ICMPV6_ERROR_PRIV1                 (100)
#define ICMPV6_ERROR_PRIV2                 (101)

#define ICMPV6_ECHO_REQUEST                (128)
#define ICMPV6_ECHO_REPLY                  (129)
#define ICMPV6_RS                          (133)	/* RFC 4861 */
#define ICMPV6_RA                          (134)	/* RFC 4861 */
#define ICMPV6_NS                          (135)	/* RFC 4861 */
#define ICMPV6_NA                          (136)	/* RFC 4861 */
#define ICMPV6_REDIRECT                    (137)	/* RFC 4861 */
#define ICMPV6_INFO_PRIV1                  (200)
#define ICMPV6_INFO_PRIV2                  (201)

/* ICMPv6 Error Message Codes */
#define ICMPV6_DEST_UNREACH_NO_ROUTE       (0)
#define ICMPV6_DEST_UNREACH_ADMIN          (1)
#define ICMPV6_DEST_UNREACH_SCOPE          (2)
#define ICMPV6_DEST_UNREACH_NO_ADDR        (3)
#define ICMPV6_DEST_UNREACH_NO_PORT        (4)
#define ICMPV6_DEST_UNREACH_SRC_ADDR       (5)
#define ICMPV6_DEST_UNREACH_REJECT_ROUTE   (6)

#define ICMPV6_TE_HOP_LIMIT_EXCEEDED       (0)
#define ICMPV6_TE_FRAG_REASSEMBLY_EXCEEDED (1)

#define ICMPV6_PARAM_PROBLEM_HEADER_FIELD  (0)
#define ICMPV6_PARAM_PROBLEM_NEXT_HEADER   (1)
#define ICMPV6_PARAM_PROBLEM_IPV6_OPT      (2)

#define ICMPV6_DEFAULT_CODE                (0)


/* Public Functions ------------------------------------------------------------------------------ */
inline bool     ipv6_eh_is_icmpv6      (const IPExthdr*);
inline uint8_t  icmpv6_type            (const IPExthdr*);
inline uint8_t  icmpv6_code            (const IPExthdr*);
inline uint16_t icmpv6_checksum        (const IPExthdr*);
inline Buffer*  icmpv6_reset_buffer    (IPExthdr*);

inline bool     icmpv6_append          (IPExthdr*, uint8_t, uint8_t);
inline bool     icmpv6_append_error    (IPExthdr*, uint8_t, uint8_t, uint32_t, const void*,unsigned);
inline uint32_t icmpv6_error_param     (const IPExthdr*);
inline bool     icmpv6_append_echo_req (IPExthdr*, uint16_t, uint16_t, const void*, unsigned);
inline bool     icmpv6_append_echo_resp(IPExthdr*, const IPExthdr*);
inline uint16_t icmpv6_echo_id         (const IPExthdr*);
inline uint16_t icmpv6_echo_seqnum     (const IPExthdr*);
inline Buffer*  icmpv6_echo_data       (IPExthdr*);
inline void     icmpv6_set_checksum    (IPExthdr*, uint16_t);
inline void     icmpv6_finalize        (IPExthdr*);



/* ipv6_eh_is_icmpv6 ****************************************************************************//**
 * @brief		Returns true if the packet is an ICMPv6 packet. */
inline bool ipv6_eh_is_icmpv6(const IPExthdr* eh)
{
	return ipv6_eh_type(eh) == IPV6_ICMPV6;
}


/* icmpv6_type **********************************************************************************//**
 * @brief		Returns the ICMPv6 type. */
inline uint8_t icmpv6_type(const IPExthdr* eh)
{
	return be_get_u8(buffer_peek_offset(&eh->buffer, 0, 1));
}


/* icmpv6_code **********************************************************************************//**
 * @brief		Returns the ICMPv6 code. */
inline uint8_t icmpv6_code(const IPExthdr* eh)
{
	return be_get_u8(buffer_peek_offset(&eh->buffer, 1, 1));
}


/* icmpv6_checksum ******************************************************************************//**
 * @brief		Returns the ICMPv6 checksum. */
inline uint16_t icmpv6_checksum(const IPExthdr* eh)
{
	return be_get_u16(buffer_peek_offset(&eh->buffer, 2, 2));
}


/* icmpv6_reset_buffer **************************************************************************//**
 * @brief		Returns the ICMPv6's buffer. The buffer's read and write pointers are reset so that
 * 				bytes can be read from the start of and written to the end of the ICMPv6's
 * 				content. */
inline Buffer* icmpv6_reset_buffer(IPExthdr* eh)
{
	return ipv6_eh_reset_buffer(eh);
}


/* icmpv6_append ********************************************************************************//**
 * @brief		Appends an ICMPv6 upper layer header after the specified extension header. */
inline bool icmpv6_append(IPExthdr* eh, uint8_t type, uint8_t code)
{
	return ipv6_eh_append(eh, IPV6_ICMPV6, 0, 0) &&
	       buffer_push_u8 (&eh->buffer, type)    &&
	       buffer_push_u8 (&eh->buffer, code)    &&
	       buffer_push_u16(&eh->buffer, 0);
}


/* icmpv6_append_error **************************************************************************//**
 * @brief		Appends an ICMPv6 error upper layer header after the specified extension header. */
inline bool icmpv6_append_error(
	IPExthdr*   eh,
	uint8_t     type,
	uint8_t     code,
	uint32_t    param,
	const void* data,
	unsigned    len)
{
	/*  0                   1                   2                   3
	 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |     Type      |     Code      |          Checksum             |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |                             Param                             |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |                    As much of invoking packet                 |
	 * +               as possible without the ICMPv6 packet           +
	 * |               exceeding the minimum IPv6 MTU [IPv6]           | */
	len = calc_min_uint(len, buffer_free(&eh->buffer));

	return icmpv6_append(eh, type, code) &&
	       buffer_push_u32(&eh->buffer, hton_u32(param)) &&
	       buffer_push_mem(&eh->buffer, data, len);
}


/* icmpv6_error_param ***************************************************************************//**
 * @brief  		Returns the ICMPv6 Error upper layer header's param field. */
inline uint32_t icmpv6_error_param(const IPExthdr* eh)
{
	return ntoh_get_u32(buffer_peek_offset(&eh->buffer, 4, 4));
}


/* icmpv6_append_echo_req ***********************************************************************//**
 * @brief		Appends an ICMPv6 echo request upper layer header after the specified extension
 * 				header. */
inline bool icmpv6_append_echo_req(
	IPExthdr*   eh,
	uint16_t    id,
	uint16_t    seqnum,
	const void* data,
	unsigned    len)
{
	/*  0                   1                   2                   3
	 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |     Type      |     Code      |          Checksum             |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |           Identifier          |        Sequence Number        |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |     Data ...
	 * +-+-+-+-+- */
	return icmpv6_append(eh, ICMPV6_ECHO_REQUEST, 0)      &&
	       buffer_push_u16(&eh->buffer, hton_u16(id))     &&
	       buffer_push_u16(&eh->buffer, hton_u16(seqnum)) &&
	       buffer_push_mem(&eh->buffer, data, len);
}


/* icmpv6_append_echo_resp **********************************************************************//**
 * @brief		Appends an ICMPv6 echo response upper layer header after the specified extension
 * 				header. */
inline bool icmpv6_append_echo_resp(IPExthdr* eh, const IPExthdr* orig)
{
	/*  0                   1                   2                   3
	 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |     Type      |     Code      |          Checksum             |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |           Identifier          |        Sequence Number        |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |     Data ...
	 * +-+-+-+-+- */
	return icmpv6_append(eh, ICMPV6_ECHO_REPLY, 0)                          &&
	       buffer_push_u16(&eh->buffer, hton_u16(icmpv6_echo_id(orig)))     &&
	       buffer_push_u16(&eh->buffer, hton_u16(icmpv6_echo_seqnum(orig))) &&
	       buffer_push_mem(&eh->buffer, buffer_start(&orig->buffer), buffer_length(&orig->buffer));
}


/* icmpv6_echo_id *******************************************************************************//**
 * @brief		Returns an echo request's/response's identifier. */
inline uint16_t icmpv6_echo_id(const IPExthdr* eh)
{
	return ntoh_get_u16(buffer_peek_offset(&eh->buffer, 4, 2));
}


/* icmpv6_echo_seqnum ***************************************************************************//**
 * @brief		Returns an echo request's/response's sequence number. */
inline uint16_t icmpv6_echo_seqnum(const IPExthdr* eh)
{
	return ntoh_get_u16(buffer_peek_offset(&eh->buffer, 6, 2));
}


/* icmpv6_echo_data *****************************************************************************//**
 * @brief		Returns an echo request's/response's data. */
inline Buffer* icmpv6_echo_data(IPExthdr* eh)
{
	buffer_read_seek(&eh->buffer, 8);
	return &eh->buffer;
}


/* icmpv6_calc_checksum *************************************************************************//**
 * @brief		Calculates the ICMPv6 packet's checksum. */
inline uint32_t icmpv6_calc_checksum(const IPExthdr* eh)
{
	/* Pseudo-header:
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |                                                               |
	 * +                                                               +
	 * |                                                               |
	 * +                         Source Address                        +
	 * |                                                               |
	 * +                                                               +
	 * |                                                               |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |                                                               |
	 * +                                                               +
	 * |                                                               |
	 * +                      Destination Address                      +
	 * |                                                               |
	 * +                                                               +
	 * |                                                               |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |                   Upper-Layer Packet Length                   |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |                      zero                     |  Next Header  |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
	const IPPacket* pkt = ipv6_eh_pkt(eh);

	uint16_t checksum = buffer_length(&eh->buffer) + IPV6_ICMPV6;
	checksum = ipv6_checksum(ipv6_src(pkt),  16, checksum);
	checksum = ipv6_checksum(ipv6_dest(pkt), 16, checksum);
	checksum = ipv6_checksum(buffer_start(&eh->buffer), buffer_length(&eh->buffer), checksum);

	return ~(checksum == 0 ? 0xFFFF : checksum);
}


/* icmpv6_set_checksum **************************************************************************//**
 * @brief		Sets the ICMPv6's checksum. */
inline void icmpv6_set_checksum(IPExthdr* eh, uint16_t checksum)
{
	/*  0                   1                   2                   3
	 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |     Type      |     Code      |          Checksum             |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |                                                               |
	 * +                         Message Body                          +
	 * |                                                               | */
	hton_set_u16(buffer_peek_offset(&eh->buffer, 2, 2), hton_u16(checksum));
}


/* icmpv6_finalize ******************************************************************************//**
 * @brief		Finalizes the ICMPv6 packet prior to sending. */
inline void icmpv6_finalize(IPExthdr* eh)
{
	ipv6_finalize(ipv6_eh_pkt(eh));
	icmpv6_set_checksum(eh, 0);	/* Ensure a 0 checksum */
	icmpv6_set_checksum(eh, hton_u16(icmpv6_calc_checksum(eh)));
}


#ifdef __cplusplus
}
#endif

#endif // ICMPV6_H
/******************************************* END OF FILE *******************************************/
