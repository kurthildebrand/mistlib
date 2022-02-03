/************************************************************************************************//**
 * @file		ndp.h
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
 * @desc		ref: NDP https://datatracker.ietf.org/doc/html/rfc4861
 * 				Router Solicitation: When an interface becomes enabled, hosts may send out Router
 * 				Solicitations that request routers to generate Router Advertisements immediately
 * 				rather than at their next scheduled time.
 *
 * 				Router Advertisement: Routers advertise their presence together with various link and
 * 				Internet parameters either periodically, or in response to a Router Solicitation
 * 				message. Router Advertisements contain prefixes that are used for determining whether
 * 				another address shares the same link (on-link determination) and/or address
 * 				configuration, a suggested hop limit value, etc.
 *
 * 				Neighbor Solicitation: Sent by a node to determine the link-layer address of a
 * 				neighbor, or to verify that a neighbor is still reachable via a cached link-layer
 * 				address. Neighbor Solicitations are also used for Duplicate Address Detection.
 *
 * 				Neighbor Advertisement: A response to a Neighbor Solicitation message. A node may
 * 				also send unsolicited Neighbor Advertisements to announce a link-layer address
 * 				change.
 *
 * 				Redirect: Used by routers to inform hosts of a better first hop for a destination.
 *
 * 					Message name                       ICMPv6 Type
 * 					----------------------------------------------
 * 					Router Solicitation                        133
 * 					Router Advertisement                       134
 * 					Neighbor Solicitation                      135
 * 					Neighbor Advertisement                     136
 * 					Redirect                                   137
 *
 * 					Option Name                               Type
 * 					----------------------------------------------
 * 					Source Link-Layer Address                    1
 * 					Target Link-Layer Address                    2
 * 					Prefix Information                           3
 * 					Redirected Header                            4
 * 					MTU                                          5
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Router Solicitation:
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                            Reserved                           |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|   Options ...
 * 					+-+-+-+-+-+-+-+-+-+-+-+-
 *
 * 					IP Fields:
 * 						Source Address
 * 							An IP address assigned to the sending interface, or the unspecified
 * 							address if no address is assigned to the sending interface.
 *
 * 						Destination Address
 * 							Typically the all-routers multicast address.
 *
 * 						Hop Limit
 * 							255
 *
 * 					ICMP Fields:
 * 						Type
 * 							133
 *
 * 						Code
 * 							0
 *
 * 						Checksum
 * 							The ICMP checksum.  See [ICMPv6].
 *
 * 						Reserved
 * 							This field is unused. It MUST be initialized to zero by the sender and
 * 							MUST be ignored by the receiver.
 *
 * 					Valid Options:
 * 						Source link-layer address
 * 							The link-layer address of the sender, if known. MUST NOT be included if
 * 							the Source Address is the unspecified address. Otherwise, it SHOULD be
 * 							included on link layers that have addresses.
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Router Advertisement:
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					| Cur Hop Limit |M|O|  Reserved |       Router Lifetime         |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                         Reachable Time                        |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                          Retrans Timer                        |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|   Options ...
 * 					+-+-+-+-+-+-+-+-+-+-+-+-
 *
 * 					IP Fields:
 * 						Source Address
 * 							MUST be the link-local address assigned to the interface from which this
 * 							message is sent.
 *
 * 						Destination Address
 * 							Typically the Source Address of an invoking Router Solicitation or the
 * 							all-nodes multicast address.
 *
 * 						Hop Limit
 * 							255
 *
 * 					ICMP Fields:
 * 						Type
 * 							134
 *
 * 						Code
 * 							0
 *
 * 						Checksum
 * 							The ICMP checksum. See [ICMPv6].
 *
 * 						Cur Hop Limit
 * 							8-bit unsigned integer. The default value that should be placed in the
 * 							Hop Count field of the IP header for outgoing IP packets. A value of zero
 * 							means unspecified (by this router).
 *
 * 						M
 * 							1-bit "Managed address configuration" flag. When set, it indicates that
 * 							addresses are available via Dynamic Host Configuration Protocol [DHCPv6].
 * 							If the M flag is set, the O flag is redundant and can be ignored because
 * 							DHCPv6 will return all available configuration information.
 *
 * 						O
 * 							1-bit "Other configuration" flag. When set, it indicates that other
 * 							configuration information is available via DHCPv6. Examples of such
 * 							information are DNS-related information or information on other servers
 * 							within the network.
 *
 * 							Note: If neither M nor O flags are set, this indicates that no
 * 							information is available via DHCPv6.
 *
 * 						Reserved
 * 							A 6-bit unused field. It MUST be initialized to zero by the sender and
 * 							MUST be ignored by the receiver.
 *
 * 						Router Lifetime
 * 							16-bit unsigned integer. The lifetime associated with the default router
 * 							in units of seconds. The field can contain values up to 65535 and
 * 							receivers should handle any value, while the sending rules in Section 6
 * 							limit the lifetime to 9000 seconds. A Lifetime of 0 indicates that the
 * 							router is not a default router and SHOULD NOT appear on the default
 * 							router list. The Router Lifetime applies only to the router's usefulness
 * 							as a default router; it does not apply to information contained in other
 * 							message fields or options. Options that need time limits for their
 * 							information include their own lifetime fields.
 *
 * 						Reachable Time 32-bit unsigned integer.
 * 							The time, in milliseconds, that a node assumes a neighbor is reachable
 * 							after having received a reachability confirmation. Used by the Neighbor
 * 							Unreachability Detection algorithm (see Section 7.3). A value of zero
 * 							means unspecified (by this router).
 *
 * 						Retrans Timer 32-bit unsigned integer.
 * 							The time, in milliseconds, between retransmitted Neighbor Solicitation
 * 							messages. Used by address resolution and the Neighbor Unreachability
 * 							Detection algorithm (see Sections 7.2 and 7.3). A value of zero means
 * 							unspecified (by this router).
 *
 * 					Possible options:
 * 						Source link-layer address
 * 							The link-layer address of the interface from which the Router
 * 							Advertisement is sent. Only used on link layers that have addresses. A
 * 							router MAY omit this option in order to enable inbound load sharing
 * 							across multiple link-layer addresses.
 *
 * 						MTU
 * 							SHOULD be sent on links that have a variable MTU (as specified in the
 * 							document that describes how to run IP over the particular link type).
 * 							MAY be sent on other links.
 *
 * 						Prefix Information
 * 							These options specify the prefixes that are on-link and/or are used for
 * 							stateless address autoconfiguration. A router SHOULD include all its
 * 							on-link prefixes (except the link-local prefix) so that multihomed hosts
 * 							have complete prefix information about on-link destinations for the links
 * 							to which they attach. If complete information is lacking, a host with
 * 							multiple interfaces may not be able to choose the correct outgoing
 * 							interface when sending traffic to its neighbors.
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Neighbor Solicitation:
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                           Reserved                            |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+                       Target Address                          +
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|   Options ...
 * 					+-+-+-+-+-+-+-+-+-+-+-+-
 *
 * 					IP Fields:
 * 						Source Address
 * 							Either an address assigned to the interface from which this message is
 * 							sent or (if Duplicate Address Detection is in progress [ADDRCONF]) the
 * 							unspecified address.
 *
 * 						Destination Address
 * 							Either the solicited-node multicast address corresponding to the target
 * 							address, or the target address.
 *
 * 						Hop Limit
 * 							255
 *
 * 					ICMP Fields:
 * 						Type
 * 							135
 *
 * 						Code
 * 						0
 *
 * 						Checksum
 * 							The ICMP checksum. See [ICMPv6].
 *
 * 						Reserved
 * 							This field is unused. It MUST be initialized to zero by the sender and
 * 							MUST be ignored by the receiver.
 *
 * 						Target Address
 * 							The IP address of the target of the solicitation. It MUST NOT be a
 * 							multicast address.
 *
 * 					Possible options:
 * 						Source link-layer address
 * 							The link-layer address for the sender. MUST NOT be included when the
 * 							source IP address is the unspecified address. Otherwise, on link layers
 * 							that have addresses this option MUST be included in multicast
 * 							solicitations and SHOULD be included in unicast solicitations.
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Neighbor Advertisement:
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|R|S|O|                     Reserved                            |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+                       Target Address                          +
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|   Options ...
 * 					+-+-+-+-+-+-+-+-+-+-+-+-
 *
 * 					IP Fields:
 * 						Source Address
 * 							An address assigned to the interface from which the advertisement is
 * 							sent.
 *
 * 						Destination Address
 * 							For solicited advertisements, the Source Address of an invoking Neighbor
 * 							Solicitation or, if the solicitation's Source Address is the unspecified
 * 							address, the all-nodes multicast address.
 *
 * 							For unsolicited advertisements typically the all-nodes multicast address.
 *
 * 						Hop Limit
 * 							255
 *
 * 					ICMP Fields:
 * 						Type
 * 							136
 *
 * 						Code
 * 							0
 *
 * 						Checksum
 * 							The ICMP checksum.  See [ICMPv6].
 *
 * 						R
 * 							Router flag. When set, the R-bit indicates that the sender is a router.
 * 							The R-bit is used by Neighbor Unreachability Detection to detect a router
 * 							that changes to a host.
 *
 * 						S
 * 							Solicited flag. When set, the S-bit indicates that the advertisement was
 * 							sent in response to a Neighbor Solicitation from the Destination address.
 * 							The S-bit is used as a reachability confirmation for Neighbor
 * 							Unreachability Detection. It MUST NOT be set in multicast advertisements
 * 							or in unsolicited unicast advertisements.
 *
 * 						O
 * 							Override flag. When set, the O-bit indicates that the advertisement
 * 							should override an existing cache entry and update the cached link-layer
 * 							address. When it is not set the advertisement will not update a cached
 * 							link-layer address though it will update an existing Neighbor Cache entry
 * 							for which no link-layer address is known. It SHOULD NOT be set in
 * 							solicited advertisements for anycast addresses and in solicited proxy
 * 							advertisements. It SHOULD be set in other solicited advertisements and in
 * 							unsolicited advertisements.
 *
 * 						Reserved
 * 							29-bit unused field. It MUST be initialized to zero by the sender and
 * 							MUST be ignored by the receiver.
 *
 * 						Target Address
 * 							For solicited advertisements, the Target Address field in the Neighbor
 * 							Solicitation message that prompted this advertisement. For an unsolicited
 * 							advertisement, the address whose link-layer address has changed. The
 * 							Target Address MUST NOT be a multicast address.
 *
 * 					Possible options:
 * 						Target link-layer address
 * 							The link-layer address for the target, i.e., the sender of the
 * 							advertisement. This option MUST be included on link layers that have
 * 							addresses when responding to multicast solicitations. When responding to
 * 							a unicast Neighbor Solicitation thisoption SHOULD be included.
 *
 * 							The option MUST be included for multicast solicitations in order to avoid
 * 							infinite Neighbor Solicitation "recursion" when the peer node does not
 * 							have a cache entry to return a Neighbor Advertisements message. When
 * 							responding to unicast solicitations, the option can be omitted since the
 * 							sender of the solicitation has the correct link-layer address; otherwise,
 * 							it would not be able to send the unicast solicitation in the first place.
 * 							However, including the link-layer address in this case adds little
 * 							overhead and eliminates a potential race condition where the sender
 * 							deletes the cached link-layer address prior to receiving a response to a
 * 							previous solicitation.
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Redirect Message:
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                           Reserved                            |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+                       Target Address                          +
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+                     Destination Address                       +
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|   Options ...
 * 					+-+-+-+-+-+-+-+-+-+-+-+-
 *
 * 					IP Fields:
 * 						Source Address
 * 							MUST be the link-local address assigned to the interface from which this
 * 							message is sent.
 *
 * 						Destination Address
 * 							The Source Address of the packet that triggered theredirect.
 *
 * 						Hop Limit
 * 							255
 *
 * 					ICMP Fields:
 * 						Type
 * 							137
 *
 * 						Code
 * 							0
 *
 * 						Checksum
 * 							The ICMP checksum. See [ICMPv6].
 *
 * 						Reserved
 * 							This field is unused. It MUST be initialized to zero by the sender and
 * 							MUST be ignored by the receiver.
 *
 * 						Target Address
 * 							An IP address that is a better first hop to use for the ICMP Destination
 * 							Address. When the target is the actual endpoint of communication, i.e.,
 * 							the destination is a neighbor, the Target Address field MUST contain the
 * 							same value as the ICMP Destination Address field. Otherwise, the target
 * 							is a better first-hop router and the Target Address MUST be the router's
 * 							link-local address so that hosts can uniquely identify routers.
 *
 * 						Destination Address
 * 							The IP address of the destination that is redirected to the target.
 *
 * 					Possible options:
 * 						Target link-layer address
 * 							The link-layer address for the target. It SHOULD be included (if known).
 * 							Note that on NBMA links, hosts may rely on the presence of the Target
 * 							Link-Layer Address option in Redirect messages as the means for
 * 							determining the link-layer addresses of neighbors. In such cases, the
 * 							option MUST be included in Redirect messages.
 *
 * 						Redirected Header
 * 							As much as possible of the IP packet that triggered the sending of the
 * 							Redirect without making the redirect packet exceed the minimum MTU
 * 							specified in [IPv6].
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Option Format:
 * 				-------------------------------------------------------------------------------------
 * 				Neighbor Discovery messages include zero or more options, some of which may appear
 * 				multiple times in the same message. Options should be padded when necessary to ensure
 * 				that they end on their natural 64-bit boundaries. All options are of the form:
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |    Length     |              ...              |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					~                              ...                              ~
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * 					Fields:
 * 						Type
 * 							8-bit identifier of the type of option. The options defined in this
 * 							document are:
 *
 * 							Option Name                               Type
 *
 * 							Source Link-Layer Address                    1
 * 							Target Link-Layer Address                    2
 * 							Prefix Information                           3
 * 							Redirected Header                            4
 * 							MTU                                          5
 *
 * 						Length
 * 							8-bit unsigned integer. The length of the option (including the type and
 * 							length fields) in units of 8 octets. The value 0 is invalid. Nodes MUST
 * 							silently discard an ND packet that contains an option with length zero.
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Source/Target Link-layer Address
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |    Length     |    Link-Layer Address ...
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * 					Fields:
 * 						Type
 * 							1 for Source Link-layer Address
 * 							2 for Target Link-layer Address
 *
 * 						Length
 * 							The length of the option (including the type and length fields) in units
 * 							of 8 octets. For example, the length for IEEE 802 addresses is 1
 * 							[IPv6-ETHER].
 *
 * 						Link-Layer Address
 * 							The variable length link-layer address. The content and format of this
 * 							field (including byte and bit ordering) is expected to be specified in
 * 							specific documents that describe how IPv6 operates over different link
 * 							layers. For instance, [IPv6-ETHER].
 *
 * 					Description
 * 						The Source Link-Layer Address option contains the link-layer address of the
 * 						sender of the packet. It is used in the Neighbor Solicitation, Router
 * 						Solicitation, and Router Advertisement packets.
 *
 * 						The Target Link-Layer Address option contains the link-layer address of the
 * 						target. It is used in Neighbor Advertisement and Redirect packets.
 *
 * 						These options MUST be silently ignored for other Neighbor Discovery messages.
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Prefix Information
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |    Length     | Prefix Length |L|A| Reserved1 |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                         Valid Lifetime                        |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                       Preferred Lifetime                      |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                           Reserved2                           |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+                            Prefix                             +
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * 					Fields:
 * 						Type
 * 							3
 *
 * 						Length
 * 							4
 *
 * 						Prefix Length
 * 							8-bit unsigned integer. The number of leading bits in the Prefix that are
 * 							valid. The value ranges from 0 to 128. The prefix length field provides
 * 							necessary information for on-link determination (when combined with the L
 * 							flag in the prefix information option). It also assists with address
 * 							autoconfiguration as specified in [ADDRCONF], for which there may be more
 * 							restrictions on the prefix length.
 *
 * 						L
 * 							1-bit on-link flag. When set, indicates that this prefix can be used for
 * 							on-link determination. When not set the advertisement makes no statement
 * 							about on-link or off-link properties of the prefix. In other words, if
 * 							the L flag is not set a host MUST NOT conclude that an address derived
 * 							from the prefix is off-link. That is, it MUST NOT update a previous
 * 							indication that the address is on-link.
 *
 * 						A
 * 							1-bit autonomous address-configuration flag. When set indicates that this
 * 							prefix can be used for stateless address configuration as specified in
 * 							[ADDRCONF].
 *
 * 						Reserved1
 * 							6-bit unused field. It MUST be initialized to zero by the sender and MUST
 * 							be ignored by the receiver.
 *
 * 						Valid Lifetime
 * 							32-bit unsigned integer. The length of time in seconds (relative to the
 * 							time the packet is sent) that the prefix is valid for the purpose of
 * 							on-link determination. A value of all one bits (0xffffffff) represents
 * 							infinity. The Valid Lifetime is also used by [ADDRCONF].
 *
 * 						Preferred Lifetime
 * 							32-bit unsigned integer. The length of time in seconds (relative to the
 * 							time the packet is sent) that addresses generated from the prefix via
 * 							stateless address autoconfiguration remain preferred [ADDRCONF]. A value
 * 							of all one bits (0xffffffff) represents infinity. See [ADDRCONF].
 *
 * 							Note that the value of this field MUST NOT exceed the Valid Lifetime
 * 							field to avoid preferring addresses that are no longer valid.
 *
 * 						Reserved2
 * 							This field is unused. It MUST be initialized to zero by the sender and
 * 							MUST be ignored by the receiver.
 *
 * 						Prefix
 * 							An IP address or a prefix of an IP address. The Prefix Length field
 * 							contains the number of valid leading bits in the prefix. The bits in the
 * 							prefix after the prefix length are reserved and MUST be initialized to
 * 							zero by the sender and ignored by the receiver. A router SHOULD NOT send
 * 							a prefix option for the link-local prefix and a host SHOULD ignore such a
 * 							prefix option.
 *
 * 					Description
 * 						The Prefix Information option provide hosts with on-link prefixes and
 * 						prefixes for Address Autoconfiguration. The Prefix Information option appears
 * 						in Router Advertisement packets and MUST be silently ignored for other
 * 						messages.
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				Redirected Header
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |    Length     |            Reserved           |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                           Reserved                            |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                                                               |
 * 					~                       IP header + data                        ~
 * 					|                                                               |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * 					Fields:
 * 						Type
 * 							4
 *
 * 						Length
 * 							The length of the option in units of 8 octets.
 *
 * 						Reserved
 * 							These fields are unused. They MUST be initialized to zero by the sender
 * 							and MUST be ignored by the receiver.
 *
 * 						IP header + data
 * 							The original packet truncated to ensure that the size of the redirect
 * 							message does not exceed the minimum MTU required to support IPv6 as
 * 							specified in [IPv6].
 *
 * 					Description
 * 						The Redirected Header option is used in Redirect messages and contains all or
 * 						part of the packet that is being redirected. This option MUST be silently
 * 						ignored for other Neighbor Discovery messages.
 *
 *
 * 				-------------------------------------------------------------------------------------
 * 				MTU
 * 				-------------------------------------------------------------------------------------
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |    Length     |           Reserved            |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                              MTU                              |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * 					Fields:
 * 						Type
 * 							5
 *
 * 						Length
 * 							1
 *
 * 						Reserved
 * 							This field is unused. It MUST be initialized to zero by the sender and
 * 							MUST be ignored by the receiver.
 *
 * 						MTU
 * 							32-bit unsigned integer. The recommended MTU for the link.
 *
 * 					Description
 * 						The MTU option is used in Router Advertisement messages to ensure that all
 * 						nodes on a link use the same MTU value in those cases where the link MTU is
 * 						not well known.
 *
 * 						This option MUST be silently ignored for other Neighbor Discovery messages.
 *
 * 						In configurations in which heterogeneous technologies are bridged together,
 * 						the maximum supported MTU may differ from one segment to another. If the
 * 						bridges do not generate ICMP Packet Too Big messages, communicating nodes
 * 						will be unable to use Path MTU to dynamically determine the appropriate MTU
 * 						on a per-neighbor basis. In such cases, routers can be configured to use the
 * 						MTU option to specify the maximum MTU value that is supported by all
 * 						segments.
 *
 ***************************************************************************************************/
#ifndef NDP_H
#define NDP_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include "icmp6.h"


/* Public Macros --------------------------------------------------------------------------------- */
#define ICMPV6_NDP_OPT_SLLAO               (1)
#define ICMPV6_NDP_OPT_TLLAO               (2)
#define ICMPV6_NDP_OPT_PREFIX_INFO         (3)
#define ICMPV6_NDP_OPT_REDIR_HDR           (4)
#define ICMPV6_NDP_OPT_MTU                 (5)

#define ICMPV6_NDP_RA_M                    (0x1 << 7)	/* Managed address config */
#define ICMPV6_NDP_RA_O                    (0x1 << 6)	/* Other config */
// #define ICMPV6_NDP_RA_H                    ()			/* Mobile IPv6 Home Agent [RFC3775] */
// #define ICMPV6_NDP_RA_PRF                  ()			/* Router Selection Preferences [RFC4191] */
// #define ICMPV6_NDP_RA_P                    ()			/* Neighbor Discovery Proxy Flag [RFC4389] */
#define ICMPV6_NDP_NA_R                    (0x1 << 7)	/* Router flag */
#define ICMPV6_NDP_NA_S                    (0x1 << 6)	/* Solicited flag */
#define ICMPV6_NDP_NA_O                    (0x1 << 5)	/* Override flag */
#define ICMPV6_NDP_PREFIX_L                (0x1 << 7)	/* On-Link flag */
#define ICMPV6_NDP_PREFIX_A                (0x1 << 6)	/* Auto address-config flag */


/* Public Types ---------------------------------------------------------------------------------- */
typedef struct {
	uint8_t  cur_hop_limit;
	uint8_t  flags;
	uint16_t router_lifetime;
	uint32_t reachable_time;
	uint32_t retrans_timer;
} NdpRA;

typedef struct {
	uint8_t  prefix_length;
	uint8_t  flags;
	uint32_t valid_lifetime;
	uint32_t preferred_lifetime;
	const IPAddress* prefix;
} NdpPrefix;


/* ICMPv6 NDP RS --------------------------------------------------------------------------------- */
inline bool       ndp_append_rs        (IPExthdr*);
inline IPOption   ndp_rs_opt_first     (const IPExthdr*);


/* ICMPv6 NDP RA --------------------------------------------------------------------------------- */
inline bool       ndp_append_ra           (IPExthdr*, const NdpRA*);
inline uint8_t    ndp_ra_hop_limit        (const IPExthdr*);
inline uint8_t    ndp_ra_flags            (const IPExthdr*);
inline uint16_t   ndp_ra_router_life      (const IPExthdr*);
inline uint32_t   ndp_ra_reachable_time   (const IPExthdr*);
inline uint32_t   ndp_ra_retrans_time     (const IPExthdr*);
inline IPOption   ndp_ra_opt_first        (const IPExthdr*);


/* ICMPv6 NDP NS --------------------------------------------------------------------------------- */
inline bool       ndp_append_ns           (IPExthdr*, const IPAddress*);
inline IPAddress* ndp_ns_target           (const IPExthdr*);
inline IPOption   ndp_ns_opt_first        (const IPExthdr*);


/* ICMPv6 NDP NA --------------------------------------------------------------------------------- */
inline bool       ndp_append_na           (IPExthdr*, uint8_t, const IPAddress*);
inline uint8_t    ndp_na_flags            (const IPExthdr*);
inline IPAddress* ndp_na_target           (const IPExthdr*);
inline IPOption   ndp_na_opt_first        (const IPExthdr*);


/* ICMPv6 NDP Redirect --------------------------------------------------------------------------- */
inline bool       ndp_append_redir        (IPExthdr*, const IPAddress*, const IPAddress*);
inline IPAddress* ndp_redir_target        (const IPExthdr*);
inline IPAddress* ndp_redir_dest          (const IPExthdr*);
inline IPOption   ndp_redir_opt_first     (const IPExthdr*);


/* ICMPv6 NDP Source/Target Link-Layer Address Option -------------------------------------------- */
inline bool       ndp_opt_append_sllao    (IPOption*, const void*, unsigned);
inline bool       ndp_opt_append_tllao    (IPOption*, const void*, unsigned);
inline Buffer*    ndp_opt_llao            (IPOption*);


/* ICMPv6 NDP Prefix Information Option ---------------------------------------------------------- */
inline bool       ndp_opt_append_prefix   (IPOption*, const NdpPrefix*);
inline uint8_t    ndp_opt_prefix_length   (const IPOption*);
inline uint8_t    ndp_opt_prefix_flags    (const IPOption*);
inline uint32_t   ndp_opt_prefix_valid    (const IPOption*);
inline uint32_t   ndp_opt_prefix_preferred(const IPOption*);
inline IPAddress* ndp_opt_prefix_addr     (const IPOption*);


/* ICMPv6 NDP Redirected Header Option ----------------------------------------------------------- */
inline bool       ndp_opt_append_redir    (IPOption*, const void*, unsigned);
inline Buffer*    ndp_opt_redir_data      (IPOption*);


/* ICMPv6 NDP MTU Option ------------------------------------------------------------------------- */
inline bool       ndp_opt_append_mtu      (IPOption*, uint32_t);
inline uint32_t   ndp_opt_mtu             (const IPOption*);





/* Router Solicitation **************************************************************************//**
 * @brief		Layout:
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                            Reserved                           |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|   Options ...
 * 					+-+-+-+-+-+-+-+-+-+-+-+-
 *
 * @desc		Valid Options:
 * 					Source link-layer address
 * 						The link-layer address of the sender, if known. MUST NOT be included if the
 * 						Source Address is the unspecified address. Otherwise, it SHOULD be included
 * 						on link layers that have addresses. */
inline bool ndp_append_rs(IPExthdr* eh)
{
	return icmpv6_append(eh, ICMPV6_RS, 0) &&
	       buffer_push_u32(&eh->buffer, hton_u32(0));
}
inline IPOption ndp_rs_opt_first(const IPExthdr* eh)
{
	return ipv6_opt_read(&eh->buffer, buffer_offset(&eh->buffer, 8));
}


/* Router Advertisement *************************************************************************//**
 * @brief		 Layout:
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					| Cur Hop Limit |M|O|  Reserved |       Router Lifetime         |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                         Reachable Time                        |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                          Retrans Timer                        |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|   Options ...
 * 					+-+-+-+-+-+-+-+-+-+-+-+-
 *
 * @desc		Possible options:
 * 					Source link-layer address
 * 						The link-layer address of the interface from which the Router Advertisement
 * 						is sent. Only used on link layers that have addresses. A router MAY omit this
 * 						option in order to enable inbound load sharing across multiple link-layer
 * 						addresses.
 *
 * 					MTU
 * 						SHOULD be sent on links that have a variable MTU (as specified in the
 * 						document that describes how to run IP over the particular link type). MAY be
 * 						sent on other links.
 *
 * 					Prefix Information
 * 						These options specify the prefixes that are on-link and/or are used for
 * 						stateless address autoconfiguration. A router SHOULD include all its on-link
 * 						prefixes (except the link-local prefix) so that multihomed hosts have
 * 						complete prefix information about on-link destinations for the links to which
 * 						they attach. If complete information is lacking, a host with multiple
 * 						interfaces may not be able to choose the correct outgoing interface when
 * 						sending traffic to its neighbors. */
inline bool ndp_append_ra(IPExthdr* eh, const NdpRA* fields)
{
	return icmpv6_append(eh, ICMPV6_RA, 0)                                &&
	       buffer_push_u8 (&eh->buffer, fields->cur_hop_limit)             &&
	       buffer_push_u8 (&eh->buffer, fields->flags & 0xC0)              &&
	       buffer_push_u16(&eh->buffer, hton_u16(fields->router_lifetime)) &&
	       buffer_push_u32(&eh->buffer, hton_u32(fields->reachable_time))  &&
	       buffer_push_u32(&eh->buffer, hton_u32(fields->retrans_timer));
}
inline uint8_t ndp_ra_hop_limit(const IPExthdr* eh)
{
	return ntoh_get_u8(buffer_peek_offset(&eh->buffer, 4, 1));
}
inline uint8_t ndp_ra_flags(const IPExthdr* eh)
{
	return ntoh_get_u8(buffer_peek_offset(&eh->buffer, 5, 1));
}
inline uint16_t ndp_ra_router_life(const IPExthdr* eh)
{
	return ntoh_get_u16(buffer_peek_offset(&eh->buffer, 6, 2));
}
inline uint32_t ndp_ra_reachable_time(const IPExthdr* eh)
{
	return ntoh_get_u32(buffer_peek_offset(&eh->buffer, 8, 4));
}
inline uint32_t ndp_ra_retrans_time(const IPExthdr* eh)
{
	return ntoh_get_u32(buffer_peek_offset(&eh->buffer, 12, 4));
}
inline IPOption ndp_ra_opt_first(const IPExthdr* eh)
{
	return ipv6_opt_read(&eh->buffer, buffer_offset(&eh->buffer, 16));
}


/* Neighbor Solicitation ************************************************************************//**
 * @brief 		Layout:
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                           Reserved                            |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+                       Target Address                          +
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|   Options ...
 * 					+-+-+-+-+-+-+-+-+-+-+-+-
 *
 * @desc		 Possible options:
 * 					Source link-layer address
 * 						The link-layer address for the sender. MUST NOT be included when the source
 * 						IP address is the unspecified address. Otherwise, on link layers that have
 * 						addresses this option MUST be included in multicast solicitations and SHOULD
 * 						be included in unicast solicitations. */
inline bool ndp_append_ns(IPExthdr* eh, const IPAddress* target_addr)
{
	return icmpv6_append(eh, ICMPV6_NS, 0)           &&
	       buffer_push_u32(&eh->buffer, hton_u32(0)) &&
	       buffer_push_mem(&eh->buffer, target_addr, 16);
}
inline IPAddress* ndp_ns_target(const IPExthdr* eh)
{
	return buffer_peek_offset(&eh->buffer, 8, 16);
}
inline IPOption ndp_ns_opt_first(const IPExthdr* eh)
{
	return ipv6_opt_read(&eh->buffer, buffer_offset(&eh->buffer, 24));
}


/* Neighbor Advertisement ***********************************************************************//**
 * @brief  		Layout:
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|R|S|O|                     Reserved                            |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+                       Target Address                          +
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|   Options ...
 * 					+-+-+-+-+-+-+-+-+-+-+-+-
 *
 * @desc		Possible options:
 * 					Target link-layer address
 * 						The link-layer address for the target, i.e., the sender of the advertisement.
 * 						This option MUST be included on link layers that have addresses when
 * 						responding to multicast solicitations. When responding to a unicast Neighbor
 * 						Solicitation thisoption SHOULD be included.
 *
 * 						The option MUST be included for multicast solicitations in order to avoid
 * 						infinite Neighbor Solicitation "recursion" when the peer node does not have a
 * 						cache entry to return a Neighbor Advertisements message. When responding to
 * 						unicast solicitations, the option can be omitted since the sender of the
 * 						solicitation has the correct link-layer address; otherwise, it would not be
 * 						able to send the unicast solicitation in the first place. However, including
 * 						the link-layer address in this case adds little overhead and eliminates a
 * 						potential race condition where the sender deletes the cached link-layer
 * 						address prior to receiving a response to a previous solicitation. */
inline bool ndp_append_na(IPExthdr* eh, uint8_t flags, const IPAddress* target_addr)
{
	return icmpv6_append(eh, ICMPV6_NA, 0)           &&
	       buffer_push_u8(&eh->buffer, flags & 0xE0) &&
	       buffer_push_u8(&eh->buffer, 0)            &&
	       buffer_push_u16(&eh->buffer, hton_u16(0)) &&
	       buffer_push_mem(&eh->buffer, target_addr, 16);
}
inline uint8_t ndp_na_flags(const IPExthdr* eh)
{
	return ntoh_get_u8(buffer_peek_offset(&eh->buffer, 4, 1)) & 0xC0;
}
inline IPAddress* ndp_na_target(const IPExthdr* eh)
{
	return buffer_peek_offset(&eh->buffer, 8, 16);
}
inline IPOption ndp_na_opt_first(const IPExthdr* eh)
{
	return ipv6_opt_read(&eh->buffer, buffer_offset(&eh->buffer, 24));
}


/* ndp_append_redir *****************************************************************************//**
 * @brief		Layout:
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |     Code      |          Checksum             |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                           Reserved                            |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+                       Target Address                          +
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+                     Destination Address                       +
 * 					|                                                               |
 * 					+                                                               +
 * 					|                                                               |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|   Options ...
 * 					+-+-+-+-+-+-+-+-+-+-+-+-
 *
 * @desc		Possible options:
 * 					Target link-layer address
 * 						The link-layer address for the target. It SHOULD be included (if known). Note
 * 						that on NBMA links, hosts may rely on the presence of the Target Link-Layer
 * 						Address option in Redirect messages as the means for determining the
 * 						link-layer addresses of neighbors. In such cases, the option MUST be included
 * 						in Redirect messages.
 *
 * 					Redirected Header
 * 						As much as possible of the IP packet that triggered the sending of the
 * 						Redirect without making the redirect packet exceed the minimum MTU specified
 * 						in [IPv6]. */
inline bool ndp_append_redir(IPExthdr* eh, const IPAddress* target, const IPAddress* dest)
{
	return icmpv6_append(eh, ICMPV6_REDIRECT, 0)     &&
	       buffer_push_u32(&eh->buffer, hton_u32(0)) &&
	       buffer_push_mem(&eh->buffer, target, 16)  &&
	       buffer_push_mem(&eh->buffer, dest, 16);
}
inline IPAddress* ndp_redir_target(const IPExthdr* eh)
{
	return buffer_peek_offset(&eh->buffer, 8, 16);
}
inline IPAddress* ndp_redir_dest(const IPExthdr* eh)
{
	return buffer_peek_offset(&eh->buffer, 24, 16);
}
inline IPOption ndp_redir_opt_first(const IPExthdr* eh)
{
	return ipv6_opt_read(&eh->buffer, buffer_offset(&eh->buffer, 40));
}





// ----------------------------------------------------------------------------------------------- //
// NDP Options                                                                                     //
// ----------------------------------------------------------------------------------------------- //
/* Source/Target Link-Layer Address Option ******************************************************//**
 * @brief		Layout:
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |    Length     |    Link-Layer Address ...
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
inline bool ndp_opt_append_sllao(IPOption* opt, const void* slladdr, unsigned len)
{
	return ipv6_opt_append(opt, ICMPV6_NDP_OPT_SLLAO, slladdr, len, 4, 0);
}
inline bool ndp_opt_append_tllao(IPOption* opt, const void* tlladdr, unsigned len)
{
	return ipv6_opt_append(opt, ICMPV6_NDP_OPT_TLLAO, tlladdr, len, 4, 0);
}
inline Buffer* ndp_opt_llao(IPOption* opt)
{
	buffer_read_seek(&opt->buffer, 2);
	return &opt->buffer;
}


/* Prefix Information Option ********************************************************************//**
 * @brief		Layout:
 *
 * 					 0                   1                   2                   3
 *	 				 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *	 				+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *	 				|     Type      |    Length     | Prefix Length |L|A| Reserved1 |
 *	 				+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *	 				|                         Valid Lifetime                        |
 *	 				+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *	 				|                       Preferred Lifetime                      |
 *	 				+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *	 				|                           Reserved2                           |
 *	 				+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *	 				|                                                               |
 *	 				+                                                               +
 *	 				|                                                               |
 *	 				+                            Prefix                             +
 *	 				|                                                               |
 *	 				+                                                               +
 *	 				|                                                               |
 *	 				+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
inline bool ndp_opt_append_prefix(IPOption* opt, const NdpPrefix* fields)
{
	return ipv6_opt_append(opt, ICMPV6_NDP_OPT_PREFIX_INFO, 0, 0, 4, 0)        &&
	       buffer_push_u8 (&opt->buffer, fields->prefix_length)                &&
	       buffer_push_u8 (&opt->buffer, fields->flags & 0xC0)                 &&
	       buffer_push_u32(&opt->buffer, hton_u32(fields->valid_lifetime))     &&
	       buffer_push_u32(&opt->buffer, hton_u32(fields->preferred_lifetime)) &&
	       buffer_push_u32(&opt->buffer, hton_u32(0))                          &&
	       buffer_push_mem(&opt->buffer, fields->prefix, 16);
}
inline uint8_t ndp_opt_prefix_length(const IPOption* opt)
{
	return ntoh_get_u8(buffer_peek_offset(&opt->buffer, 2, 1));
}
inline uint8_t ndp_opt_prefix_flags(const IPOption* opt)
{
	return ntoh_get_u8(buffer_peek_offset(&opt->buffer, 3, 1));
}
inline uint32_t ndp_opt_prefix_valid(const IPOption* opt)
{
	return ntoh_get_u32(buffer_peek_offset(&opt->buffer, 4, 4));
}
inline uint32_t ndp_opt_prefix_preferred(const IPOption* opt)
{
	return ntoh_get_u32(buffer_peek_offset(&opt->buffer, 8, 4));
}
inline IPAddress* ndp_opt_prefix_addr(const IPOption* opt)
{
	return buffer_peek_offset(&opt->buffer, 16, 16);
}


/* Redirected Header Option *********************************************************************//**
 * @brief		Layout:
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |    Length     |            Reserved           |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                           Reserved                            |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                                                               |
 * 					~                       IP header + data                        ~
 * 					|                                                               |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
inline bool ndp_opt_append_redir(IPOption* opt, const void* pkt, unsigned len)
{
	len = calc_min_uint(buffer_free(&opt->buffer), len);

	return ipv6_opt_append(opt, ICMPV6_NDP_OPT_REDIR_HDR, 0, 0, 4, 0) &&
	       buffer_push_u16(&opt->buffer, hton_u16(0)) &&
	       buffer_push_u32(&opt->buffer, hton_u32(0)) &&
	       buffer_push_mem(&opt->buffer, pkt, len);
}
inline Buffer* ndp_opt_redir_data(IPOption* opt)
{
	buffer_read_seek(&opt->buffer, 8);
	return &opt->buffer;
}


/* MTU Option ***********************************************************************************//**
 * @brief		Layout:
 *
 * 					 0                   1                   2                   3
 * 					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|     Type      |    Length     |           Reserved            |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 					|                              MTU                              |
 * 					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
inline bool ndp_opt_append_mtu(IPOption* opt, uint32_t mtu)
{
	return ipv6_opt_append(opt, ICMPV6_NDP_OPT_MTU, 0, 0, 4, 0) &&
	       buffer_push_u16(&opt->buffer, hton_u16(0)) &&
	       buffer_push_u32(&opt->buffer, hton_u32(mtu));
}
inline uint32_t ndp_opt_mtu(const IPOption* opt)
{
	return ntoh_get_u32(buffer_peek_offset(&opt->buffer, 4, 4));
}

#ifdef __cplusplus
}
#endif

#endif // NDP_H
/******************************************* END OF FILE *******************************************/
