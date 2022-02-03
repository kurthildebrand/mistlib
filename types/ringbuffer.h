/************************************************************************************************//**
 * @file		ringbuffer.h
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
#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher for inline support.
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <stdbool.h>

#include "key.h"
#include "range.h"


/* Public Types ---------------------------------------------------------------------------------- */
typedef struct {
	Key key;
	Range range;
	unsigned write;
	unsigned read;
} RingBuffer;


/* Public Functions ------------------------------------------------------------------------------ */
inline bool     rb_init      (RingBuffer*, void*, unsigned, unsigned);
inline bool     rb_init_range(RingBuffer*, Range);
inline Range*   rb_range     (const RingBuffer*);
inline void     rb_clear     (RingBuffer* rb)       { rb->write = 0; rb->read = 0;        }
inline Key      rb_key       (const RingBuffer* rb) { return rb->key;                     }
inline unsigned rb_size      (const RingBuffer* rb) { return range_count(&rb->range);     }
inline unsigned rb_elemsize  (const RingBuffer* rb) { return range_elemsize(&rb->range);  }
inline unsigned rb_count     (const RingBuffer* rb) { return rb->write - rb->read;        }
inline unsigned rb_free      (const RingBuffer* rb) { return rb_size(rb) - rb_count(rb);  }
inline bool     rb_empty     (const RingBuffer* rb) { return rb->write == rb->read;       }
inline bool     rb_full      (const RingBuffer* rb) { return rb_count(rb) == rb_size(rb); }
inline void*    rb_entry     (const RingBuffer*, unsigned);
inline void*    rb_next      (const RingBuffer* rb) { return rb_entry(rb, 0);             }

inline bool     rb_push_many (RingBuffer*, const void*, unsigned);
inline bool     rb_pop_many  (RingBuffer*, unsigned);

inline void*    rb_reserve   (RingBuffer*);
inline bool     rb_push      (RingBuffer*, const void*);
inline bool     rb_peek      (const RingBuffer*, void*);
inline bool     rb_peek_at   (const RingBuffer*, void*, unsigned);
inline bool     rb_pop       (RingBuffer*);
inline bool     rb_get       (RingBuffer*, void*);


/* rb_init **************************************************************************************//**
 * @brief		Initializes a ring buffer to point to the data buffer with size number of elements.
 * @warning		The size of the ring buffer must be a power of two.
 * @param[in]	rb: the ring buffer to initialize.
 * @param[in]	data: the buffer to hold ring buffer entries.
 * @param[in]	size: the capacity of the ring buffer. The size must be a power of two.
 * @param[in]	elemsize: the size of an entry in the queue.
 * @retval		true if the ring buffer was initialized successfully.
 * @retval		false if the size of the ring buffer was not a power of 2. */
inline bool rb_init(RingBuffer* rb, void* data, unsigned size, unsigned elemsize)
{
	/* Guard against empty ring buffers. Also, ensure that ring buffers have a power of 2 size.
	 * Note: if the size is a power of 2, only one bit will be set. Therefore, size bitwise and with
	 * size-1 will be zero if and only if the size is a power of 2. E.G. 0x100 & 0xFF = 0. Zero is
	 * explicitly checked because size & (size-1) incorrectly validates 0 as a power of 2. */
	if(size && !(size & (size-1)))
	{
		range_init(&rb->range, data, size, elemsize);
		rb->read     = 0;
		rb->write    = 0;
		return true;
	}
	else
	{
		return false;
	}
}


/* rb_init_range ********************************************************************************//**
 * @brief		Initializes an empty ring buffer with elements contained in the specified range.
 * @warning		The size of the ring buffer must be a power of two.
 * @param[in]	rb: the ring buffer to initialize.
 * @param[in]	r: the range containing the ring buffer's entries. The range must have a power of
 *				2 size.
 * @retval		true if the ring buffer was initialized successfully.
 * @retval		false if the size of the ring buffer was not a power of 2. */
inline bool rb_init_range(RingBuffer* rb, Range r)
{
	/* Guard against empty ring buffers. Also, ensure that ring buffers have a power of 2 size.
	 * Note: if the size is a power of 2, only one bit will be set. Therefore, size bitwise and with
	 * size-1 will be zero if and only if the size is a power of 2. E.G. 0x100 & 0xFF = 0. Zero is
	 * explicitly checked because size & (size-1) incorrectly validates 0 as a power of 2. */
	if(range_count(&r) && !(range_count(&r) & (range_count(&r)-1)))
	{
		rb->range = r;
		rb->read  = 0;
		rb->write = 0;
		return true;
	}
	else
	{
		return false;
	}
}


/* rb_range *************************************************************************************//**
 * @brief		Returns a range for the ring buffers.
 * @warning		This is an internal function and should not be called by the user. */
inline Range* rb_range(const RingBuffer* rb)
{
	return (Range*)&rb->range;
}


/* rb_entry *************************************************************************************//**
 * @brief		Returns a pointer to the entry at the specified index. The index is offset from the
 * 				oldest entry in the queue. That is, index 0 returns a pointer to the next entry to be
 * 				removed from the ring buffer.
 * @param[in]	rb: the ring buffer from which to retrieve an entry.
 * @param[in]	idx: the index of the entry.
 * @return		Pointer to the entry at the specified index. Null if the index is out of bounds. */
inline void* rb_entry(const RingBuffer* rb, unsigned idx)
{
	if(idx < rb_count(rb))
	{
		Range* r = rb_range(rb);

		return range_at(r, range_start(r) + ((rb->read + idx) & (rb_size(rb) - 1)));
	}
	else
	{
		return 0;
	}
}


/* rb_push_many *********************************************************************************//**
 * @brief		Copies the specified number of elements into the ring buffer if there is enough
 * 				space.
 * @param[in]	rb: the ring buffer into which new elements are placed.
 * @param[in]	in: the array containing the new elements.
 * @param[in]	count: the number of new elements to push.
 * @retval		true if the new elements were pushed successfully.
 * @retval		false if there was not enough space in the ring buffer. */
inline bool rb_push_many(RingBuffer* rb, const void* in, unsigned count)
{
	if(rb_free(rb) < count)
	{
		return false;
	}

	/* Copy from s (source) -> r (ring buffer). Compute the number of entries remaining till the end
	 * of the ringbuffer's array. */
	      Range* r = rb_range(rb);
	const Range  s = make_const_range(in, count, rb_elemsize(rb));
	unsigned block = rb_size(rb) - (rb->write & (rb_size(rb)-1));

	if(count <= block)
	{
		memmove(
			range_at(r, rb->write & (rb_size(rb)-1)),
			range_at(&s, 0),
			count * rb_elemsize(rb));
	}
	else
	{
		memmove(
			range_at(r, rb->write & (rb_size(rb)-1)),
			range_at(&s, 0),
			block * rb_elemsize(rb));

		memmove(
			range_at(r, (rb->write + block) & (rb_size(rb)-1)),
			range_at(&s, block),
			(count - block) * rb_elemsize(rb));
	}

	rb->write += count;
	return true;
}


/* rb_pop_many **********************************************************************************//**
 * @brief		Removes the specified number of elements from the ring buffer if the ring buffer
 * 				contains at least that many elements.
 * @param[in]	rb: the ring buffer from which to remove elements.
 * @param[in]	count: the number of elements to remove.
 * @retval		true if the requested number of elements were removed successfully.
 * @retval		false if the ring buffer contains fewer than the specified number of elements. In
 * 				this case, the ring buffer will be left unmodified. */
inline bool rb_pop_many(RingBuffer* rb, unsigned count)
{
	if(rb_count(rb) >= count)
	{
		rb->read += count;
		return true;
	}
	else
	{
		return false;
	}
}


/* rb_reserve ***********************************************************************************//**
 * @brief		Reserves an entry from the ring buffer and returns a pointer to the entry.
 * @warning		This function is not thread safe (single producer, single consumer).
 * @param[in]	rb: the ring buffer to operate on.
 * @param[in]	in: the new element to push onto ring buffer.
 * @retval		true if the new element was pushed onto the ring buffer.
 * @retval		false if the ring buffer is full. */
inline void* rb_reserve(RingBuffer* rb)
{
	if(!rb_full(rb))
	{
		Range* r = rb_range(rb);

		return range_at(r, range_start(r) + (rb->write++ & (rb_size(rb) - 1)));
	}
	else
	{
		return 0;
	}
}


/* rb_push **************************************************************************************//**
 * @brief		Copies a new element into the ring buffer if its not full.
 * @param[in]	rb: the ring buffer to place a new element on.
 * @param[in]	in: the new element to place.
 * @retval		true if the operation succeeded.
 * @retval		false if the operation failed. In this case the queue remains unchanged. */
inline bool rb_push(RingBuffer* rb, const void* in)
{
	if(!rb_full(rb))
	{
		Range* r = rb_range(rb);

		memmove(range_at(r, rb->write & (rb_size(rb) - 1)), in, rb_elemsize(rb));

		rb->write++;

		return true;
	}
	else
	{
		return false;
	}
}


/* rb_peek **************************************************************************************//**
 * @brief		Retrieves the next element to be removed from the ring buffer.
 * @param[in]	rb: the ring buffer to retrieve an entry from.
 * @param[out]	out: buffer to store a copy of the oldest element.
 * @retval		true if the operation succeeded.
 * @retval		false if the operation failed. */
inline bool rb_peek(const RingBuffer* rb, void* out)
{
	return rb_peek_at(rb, out, 0);
}


/* rb_peek_at ***********************************************************************************//**
 * @brief		Retireves the specified element indexed from the oldest entry from the ring buffer if
 * 				possible.
 * @param[in]	rb: the ring buffer to retrieve an entry from.
 * @param[out]	out: buffer to store a copy of the specified element.
 * @param[in]	idx: the index of the element to retrieve.
 * @retval		true if the element was retrieved successfully.
 * @retval		false if the index was out of bounds. */
inline bool rb_peek_at(const RingBuffer* rb, void* out, unsigned idx)
{
	const void* ptr = rb_entry(rb, idx);

	if(ptr)
	{
		memmove(out, ptr, rb_elemsize(rb));
		return true;
	}
	else
	{
		return false;
	}
}


/* rb_pop ***************************************************************************************//**
 * @brief		Removes the next element from the ring buffer.
 * @param[in]	rb: the ring buffer to remove an element from.
 * @retval		true if the oldest element was removed from the ring buffer.
 * @retval		false if the ring buffer is empty. */
inline bool rb_pop(RingBuffer* rb)
{
	if(!rb_empty(rb))
	{
		rb->read++;
		return true;
	}
	else
	{
		return false;
	}
}


/* rb_get ***************************************************************************************//**
 * @brief		Retrieves and removes an element from the ring buffer.
 * @param[in]	rb: the ring buffer to remove an element from.
 * @param[out]	out: buffer to store a copy of the next element.
 * @retval		true if the oldest element was retrieved and removed from the ring buffer.
 * @retval		false if the ring buffer is empty. */
inline bool rb_get(RingBuffer* rb, void* out)
{
	return rb_peek(rb, out) && rb_pop(rb);
}


#ifdef __cplusplus
}
#endif

#endif // RINGBUFFER_H
/******************************************* END OF FILE *******************************************/
