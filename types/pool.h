/************************************************************************************************//**
 * @file		pool.h
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
 * @brief		Fixed array of objects that can be allocated and deallocated dynamically.
 *
 ***************************************************************************************************/
#ifndef POOL_H
#define POOL_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "calc.h"
#include "key.h"
#include "range.h"


/* Public Types ---------------------------------------------------------------------------------- */
typedef struct {
	Key   key;
	Range entries;
	_Atomic uint32_t freemask;	/* Bitmask where a 1 bit indicates a free entry */
} Pool;


/* Public Functions ------------------------------------------------------------------------------ */
inline bool     pool_init           (Pool*, void*, unsigned, unsigned);
inline bool     pool_init_range     (Pool*, Range);
inline void     pool_clear          (Pool*);
inline unsigned pool_size           (const Pool* p) { return range_count(&p->entries);       }
inline unsigned pool_elemsize       (const Pool* p) { return range_elemsize(&p->entries);    }
inline unsigned pool_free           (const Pool* p) { return calc_popcount_u32(p->freemask); }
inline unsigned pool_count          (const Pool* p) { return pool_size(p) - pool_free(p);    }
inline bool     pool_empty          (const Pool* p) { return pool_count(p) == 0;             }
inline bool     pool_full           (const Pool* p) { return p->freemask == 0;               }
inline void*    pool_entry          (const Pool*, unsigned);
inline bool     pool_idx_is_reserved(const Pool*, unsigned);
inline bool     pool_ptr_is_reserved(const Pool*, void*);
inline void*    pool_reserve        (Pool*);
inline bool     pool_release        (Pool*, const void*);


/* pool_init ************************************************************************************//**
 * @brief		Initializes a pool.
 * @param[in]	p: the pool to initialize.
 * @param[in]	entries: pointer to the buffer holding the pool's entries.
 * @param[in]	size: the total number of possible entries.
 * @param[in]	elemsize: the size in bytes of one entry in the pool. */
inline bool pool_init(Pool* p, void* entries, unsigned size, unsigned elemsize)
{
	if(size == 0 || size > 32)
	{
		return false;
	}
	else
	{
		range_init(&p->entries, entries, size, elemsize);
		pool_clear(p);
		return true;
	}
}


/* pool_init_range ******************************************************************************//**
 * @brief		Initializes a pool from a range of entries.
 * @param[in]	p: the pool to initialize.
 * @param[in]	entries: the range of the pool's entries. */
inline bool pool_init_range(Pool* p, Range entries)
{
	if(range_count(&entries) == 0 || range_count(&entries) > 32)
	{
		return false;
	}
	else
	{
		p->entries = entries;
		pool_clear(p);
		return true;
	}
}


/* pool_cool ************************************************************************************//**
 * @brief       Deallocates all entries from the pool. */
inline void pool_clear(Pool* p)
{
	if(range_count(&p->entries) == 0)
	{
		p->freemask = 0;
	}
	else
	{
		p->freemask = (0x1 << range_count(&p->entries)) - 1;
	}
}


/* pool_entry ***********************************************************************************//**
 * @brief       Returns a pointer to the entry at the specified index. */
inline void* pool_entry(const Pool* p, unsigned i)
{
	return range_entry(&p->entries, i);
}


/* pool_idx_is_reserved *************************************************************************//**
 * @brief		Returns true if the slot at the specified offset is reserved. */
inline bool pool_idx_is_reserved(const Pool* p, unsigned idx)
{
	if(idx < range_count(&p->entries))
	{
		return (p->freemask & (0x1 << idx)) == 0;
	}
	else
	{
		return false;
	}
}


/* pool_ptr_is_reserved *************************************************************************//**
 * @brief		Returns true if the entry pointed to by ptr has been reserved. */
inline bool pool_ptr_is_reserved(const Pool* p, void* ptr)
{
	return pool_idx_is_reserved(p, range_offsetof(&p->entries, ptr));
}


/* pool_reserve *********************************************************************************//**
 * @brief		Allocates an entry from a pool. */
inline void* pool_reserve(Pool* p)
{
	uint32_t freemask;
	uint32_t resmask;

	do {
		freemask = p->freemask;

		if(freemask == 0)
		{
			return 0;
		}

		resmask = freemask & (-freemask);	/* Isolate the rightmost 1 bit */
	} while(!atomic_compare_exchange_weak(&p->freemask, &freemask, freemask & ~(resmask)));

	return range_offset(&p->entries, calc_log2(resmask));
}


/* pool_release *********************************************************************************//**
 * @brief		Deallocates an entry from a pool.
 * @param[in]	p: the pool containing the entry to deallocate.
 * @param[in]	ptr: address of the entry to deallocate.
 * @retval		true if the entry was deallocated successfully.
 * @retval		false if the entry is not contained in the pool or the entry has already been
 *				deallocated. */
inline bool pool_release(Pool* p, const void* ptr)
{
	uint32_t freemask;
	uint32_t offset = range_offsetof(&p->entries, ptr);

	if(offset >= 32)
	{
		return false;
	}

	do {
		freemask = p->freemask;

		if(freemask & (0x1 << offset))
		{
			break;
		}
	} while(!atomic_compare_exchange_weak(&p->freemask, &freemask, freemask | (0x1 << offset)));

	return true;
}


#ifdef __cplusplus
}
#endif

#endif // POOL_H
/******************************************* END OF FILE *******************************************/
