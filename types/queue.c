/************************************************************************************************//**
 * @file		queue.c
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
#include "queue.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern bool     queue_init (Queue*, void* _Atomic*, unsigned);
extern void     queue_clear(Queue*);
extern Key      queue_key  (const Queue*);
extern unsigned queue_size (const Queue*);
extern unsigned queue_count(const Queue*);
extern unsigned queue_free (const Queue*);
extern bool     queue_empty(const Queue*);
extern bool     queue_full (const Queue*);
extern void*    queue_entry(const Queue*, unsigned);
extern void*    queue_next (const Queue*);


/* queue_push ***********************************************************************************//**
 * @brief		Appends a pointer to the queue. */
bool queue_push(Queue* q, const void* ptr)
{
	unsigned write = q->write;

	while(write - q->read != queue_size(q))
	{
		if(atomic_compare_exchange_weak(&q->write, &write, write + 1))
		{
			q->entries[write & (queue_size(q)-1)] = (void*)ptr;
			return true;
		}
	}

	return false;
}


/* queue_peek ***********************************************************************************//**
 * @brief		Copies the oldest pointer in the queue into the out parameter. */
bool queue_peek(const Queue* q, void* out)
{
	unsigned read  = q->read;
	unsigned write = q->write;

	if(write != read)
	{
		memmove(out, &q->entries[read & (queue_size(q)-1)], sizeof(q->entries[0]));
		return true;
	}
	else
	{
		return false;
	}
}


/* queue_pop ************************************************************************************//**
 * @brief		Removes the oldest entry in the queue. */
bool queue_pop(Queue* q)
{
	unsigned read = q->read;

	if(read != q->write)
	{
		// q->entries[read & (queue_size(q)-1)] = 0;
		atomic_init(&q->entries[read & (queue_size(q)-1)], 0);
		atomic_fetch_add(&q->read, 1);
		return true;
	}
	else
	{
		return false;
	}
}


/* queue_get ************************************************************************************//**
 * @brief		Retrieves and removes a pointer from the queue.
 * @param[in]	q: the queue from which to remove an element.
 * @param[out]	out: parameter to store a pointer to the removed entry.
 * @retval		true if the oldest element was retrieved and removed from the queue.
 * @retval		false is the queue is empty. */
bool queue_get(Queue* q, void* out)
{
	unsigned read = q->read;

	while(read != q->write)
	{
		void* temp = q->entries[read];

		if(atomic_compare_exchange_weak(&q->read, &read, read + 1))
		{
			memmove(out, &temp, sizeof(q->entries[0]));
			// q->entries[read & (queue_size(q)-1)] = 0;
			atomic_init(&q->entries[read & (queue_size(q)-1)], 0);
			return true;
		}
	}

	return false;
}


/******************************************* END OF FILE *******************************************/
