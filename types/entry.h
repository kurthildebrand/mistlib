/************************************************************************************************//**
 * @file		entry.h
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
#ifndef ENTRY_H
#define ENTRY_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include "range.h"


/* Public Types ---------------------------------------------------------------------------------- */
typedef struct {
	void*    ptr;
	unsigned idx;
} Entry;


/* Public Functions ------------------------------------------------------------------------------ */
inline Entry    make_entry(void*, unsigned);
inline Entry    make_range_entry(const Range*, unsigned);
inline void*    eptr(const Entry* e) { return e ? e->ptr : 0;   }
inline unsigned eidx(const Entry* e) { return e ? e->idx : -1u; }


/* make_entry ***********************************************************************************//**
 * @brief		Creates an entry with the specified pointer and index. */
inline Entry make_entry(void* ptr, unsigned idx)
{
	return (Entry){ .ptr = ptr, .idx = idx };
}


/* make_range_entry *****************************************************************************//**
 * @brief		Creates an entry from the indexed element in a range. */
inline Entry make_range_entry(const Range* r, unsigned idx)
{
	return make_entry(range_entry(r, idx), idx);
}


#ifdef __cplusplus
}
#endif

#endif // ENTRY_H
/******************************************* END OF FILE *******************************************/
