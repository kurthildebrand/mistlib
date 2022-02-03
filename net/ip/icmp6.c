/************************************************************************************************//**
 * @file		icmp6.c
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
#include "icmp6.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern bool     ipv6_eh_is_icmpv6      (const IPExthdr*);
extern uint8_t  icmpv6_type            (const IPExthdr*);
extern uint8_t  icmpv6_code            (const IPExthdr*);
extern uint16_t icmpv6_checksum        (const IPExthdr*);
extern Buffer*  icmpv6_reset_buffer    (IPExthdr*);
extern bool     icmpv6_append          (IPExthdr*, uint8_t, uint8_t);
extern bool     icmpv6_append_error    (IPExthdr*, uint8_t, uint8_t, uint32_t, const void*,unsigned);
extern bool     icmpv6_append_echo_req (IPExthdr*, uint16_t, uint16_t, const void*, unsigned);
extern bool     icmpv6_append_echo_resp(IPExthdr*, const IPExthdr*);
extern uint16_t icmpv6_echo_id         (const IPExthdr*);
extern uint16_t icmpv6_echo_seqnum     (const IPExthdr*);
extern Buffer*  icmpv6_echo_data       (IPExthdr*);
extern uint32_t icmpv6_calc_checksum   (const IPExthdr*);
extern void     icmpv6_set_checksum    (IPExthdr*, uint16_t);
extern void     icmpv6_finalize        (IPExthdr*);


/******************************************* END OF FILE *******************************************/
