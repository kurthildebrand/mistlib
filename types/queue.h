/************************************************************************************************//**
 * @file		queue.h
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
#ifndef QUEUE_H
#define QUEUE_H

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

#include "key.h"


/* Public Types ---------------------------------------------------------------------------------- */
typedef struct {
	        Key      key;
	        unsigned size;
	_Atomic unsigned read;
	_Atomic unsigned write;
	void* _Atomic* entries;		/* Pointer to atomic void pointer */
} Queue;


/* Public Functions ------------------------------------------------------------------------------ */
inline bool     queue_init (Queue*, void* _Atomic*, unsigned);
inline void     queue_clear(Queue* q)                { q->read = q->write;                     }
inline Key      queue_key  (const Queue* q)          { return q->key;                          }
inline unsigned queue_size (const Queue* q)          { return q->size;                         }
inline unsigned queue_count(const Queue* q)          { return q->write - q->read;              }
inline unsigned queue_free (const Queue* q)          { return queue_size(q) - queue_count(q);  }
inline bool     queue_empty(const Queue* q)          { return q->write == q->read;             }
inline bool     queue_full (const Queue* q)          { return queue_count(q) == queue_size(q); }
inline void*    queue_entry(const Queue*, unsigned);
inline void*    queue_next (const Queue* q)          { return queue_entry(q, 0);               }

       bool     queue_push (Queue*, const void*);
       bool     queue_peek (const Queue*, void*);
       bool     queue_pop  (Queue*);
       bool     queue_get  (Queue*, void*);


/* queue_init ***********************************************************************************//**
 * @brief		Initializes the queue. The queue stores pointers to entries.
 * @param[in]	q: the queue to initialize.
 * @param[in]	entries: pointer to an array of atomic void pointers.
 * @param[in]	size: the capacity of the queue. Must be a power of 2.
 * @retval		true if the queue was initialized successfully.
 * @retval		false if the size of the queue was not a power of 2. */
inline bool queue_init(Queue* q, void* _Atomic* entries, unsigned size)
{
	if(entries && size && !(size & (size-1)))
	{
		q->read    = 0;
		q->write   = 0;
		q->size    = size;
		q->entries = entries;
		return true;
	}
	else
	{
		return false;
	}
}


/* queue_entry **********************************************************************************//**
 * @brief		Returns the pointer at the specified index. The index is offset from the oldest entry
 * 				in the queue. That is, index 0 returns the next entry to be removed from the queue.
 * @param[in]	q: the queue from which to retrieve an entry.
 * @param[in]	idx: the index of the entry.
 * @return		Pointer to the entry at the specified index. Null if the index is out of bounds. */
inline void* queue_entry(const Queue* q, unsigned idx)
{
	if(idx < queue_count(q))
	{
		return q->entries[(q->read + idx) & (queue_size(q)-1)];
	}
	else
	{
		return 0;
	}
}


#ifdef __cplusplus
}
#endif

#endif // QUEUE_H
/******************************************* END OF FILE *******************************************/
