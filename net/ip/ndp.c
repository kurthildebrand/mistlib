/************************************************************************************************//**
 * @file		ndp.c
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
#include "ndp.h"


/* ICMPv6 NDP RS --------------------------------------------------------------------------------- */
extern bool       ndp_append_rs           (IPExthdr*);
extern IPOption   ndp_rs_opt_first        (const IPExthdr*);


/* ICMPv6 NDP RA --------------------------------------------------------------------------------- */
extern bool       ndp_append_ra           (IPExthdr*, const NdpRA*);
extern uint8_t    ndp_ra_hop_limit        (const IPExthdr*);
extern uint8_t    ndp_ra_flags            (const IPExthdr*);
extern uint16_t   ndp_ra_router_life      (const IPExthdr*);
extern uint32_t   ndp_ra_reachable_time   (const IPExthdr*);
extern uint32_t   ndp_ra_retrans_time     (const IPExthdr*);
extern IPOption   ndp_ra_opt_first        (const IPExthdr*);


/* ICMPv6 NDP NS --------------------------------------------------------------------------------- */
extern bool       ndp_append_ns           (IPExthdr*, const IPAddress*);
extern IPAddress* ndp_ns_target           (const IPExthdr*);
extern IPOption   ndp_ns_opt_first        (const IPExthdr*);


/* ICMPv6 NDP NA --------------------------------------------------------------------------------- */
extern bool       ndp_append_na           (IPExthdr*, uint8_t, const IPAddress*);
extern uint8_t    ndp_na_flags            (const IPExthdr*);
extern IPAddress* ndp_na_target           (const IPExthdr*);
extern IPOption   ndp_na_opt_first        (const IPExthdr*);


/* ICMPv6 NDP Redirect --------------------------------------------------------------------------- */
extern bool       ndp_append_redir        (IPExthdr*, const IPAddress*, const IPAddress*);
extern IPAddress* ndp_redir_target        (const IPExthdr*);
extern IPAddress* ndp_redir_dest          (const IPExthdr*);
extern IPOption   ndp_redir_opt_first     (const IPExthdr*);


/* ICMPv6 NDP Source/Target Link-Layer Address Option -------------------------------------------- */
extern bool       ndp_opt_append_sllao    (IPOption*, const void*, unsigned);
extern bool       ndp_opt_append_tllao    (IPOption*, const void*, unsigned);
extern Buffer*    ndp_opt_llao            (IPOption*);


/* ICMPv6 NDP Prefix Information Option ---------------------------------------------------------- */
extern bool       ndp_opt_append_prefix   (IPOption*, const NdpPrefix*);
extern uint8_t    ndp_opt_prefix_length   (const IPOption*);
extern uint8_t    ndp_opt_prefix_flags    (const IPOption*);
extern uint32_t   ndp_opt_prefix_valid    (const IPOption*);
extern uint32_t   ndp_opt_prefix_preferred(const IPOption*);
extern IPAddress* ndp_opt_prefix_addr     (const IPOption*);


/* ICMPv6 NDP Redirected Header Option ----------------------------------------------------------- */
extern bool       ndp_opt_append_redir    (IPOption*, const void*, unsigned);
extern Buffer*    ndp_opt_redir_data      (IPOption*);


/* ICMPv6 NDP MTU Option ------------------------------------------------------------------------- */
extern bool       ndp_opt_append_mtu      (IPOption*, uint32_t);
extern uint32_t   ndp_opt_mtu             (const IPOption*);


/******************************************* END OF FILE *******************************************/
