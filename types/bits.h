/************************************************************************************************//**
 * @file		bits.h
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
#ifndef BITS_H
#define BITS_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <stdbool.h>
#include <stdint.h>


/* Public Macros --------------------------------------------------------------------------------- */
#define BIT_ARRAY_SIZE(n)	(((n) + 7) / 8)


/* Public Types ---------------------------------------------------------------------------------- */
typedef struct {
	void*    ptr;
	unsigned count;
} Bits;


/* Public Functions ------------------------------------------------------------------------------ */
inline void     bits_init      (Bits*, void*, unsigned);
inline Bits     make_bits      (void*, unsigned);
inline unsigned bits_count     (const Bits* b) { return b->count; }
inline bool     bits_value     (const Bits*, unsigned);
inline unsigned bits_start     (void)          { return 0; }
inline unsigned bits_end       (const Bits* b) { return b->count; }
       unsigned bits_zeros     (const Bits*);
       unsigned bits_ones      (const Bits*);
       unsigned bits_next_value(Bits*, unsigned, bool);
       unsigned bits_prev_value(Bits*, unsigned, bool);
inline unsigned bits_next_zero (Bits* b, unsigned idx) { return bits_next_value(b, idx, 0); }
inline unsigned bits_prev_zero (Bits* b, unsigned idx) { return bits_prev_value(b, idx, 0); }
inline unsigned bits_next_one  (Bits* b, unsigned idx) { return bits_next_value(b, idx, 1); }
inline unsigned bits_prev_one  (Bits* b, unsigned idx) { return bits_prev_value(b, idx, 1); }

       void     bits_set_all   (Bits*);
       void     bits_clear_all (Bits*);

       bool     bits_write_many(Bits*, bool, unsigned, unsigned);
inline bool     bits_set_many  (Bits*, unsigned, unsigned);
inline bool     bits_clear_many(Bits*, unsigned, unsigned);
       bool     bits_flip_many (Bits*, unsigned, unsigned);

inline bool     bits_write     (Bits* b, bool w, unsigned i) { return bits_write_many(b, w, i, 1); }
inline bool     bits_set       (Bits* b, unsigned i)         { return bits_set_many  (b, i, 1);    }
inline bool     bits_clear     (Bits* b, unsigned i)         { return bits_clear_many(b, i, 1);    }
inline bool     bits_flip      (Bits* b, unsigned i)         { return bits_flip_many (b, i, 1);    }


/* bits_init ************************************************************************************//**
 * @brief		Initializes the bit array.
 * @warning		Expects ptr to be a byte array with unused bits set to 0.
 * @param[in]	b: the bit array to initialize.
 * @param[in]	ptr: the first byte of the bit array.
 * @param[in]	count: the number of bits in the bit array. */
inline void bits_init(Bits* b, void* ptr, unsigned count)
{
	b->ptr   = ptr;
	b->count = ptr ? count : 0;
}


/* make_bits ***********************************************************************************//**
 * @brief		Returns a constructed bit array.
 * @param[in]	ptr: the first byte of the bit array.
 * @param[in]	count: the number of bits in the bit array. */
inline Bits make_bits(void* ptr, unsigned count)
{
	return (Bits){ .ptr = ptr, .count = ptr ? count : 0 };
}


/* bits_value ***********************************************************************************//**
 * @brief		Returns true if the bit at the specified position is set. Returns false if the bit is
 * 				not set or the index is out of range. */
inline bool bits_value(const Bits* b, unsigned idx)
{
	if(idx < bits_count(b))
	{
		uint8_t* ptr = (uint8_t*)(b->ptr);
		return (ptr[idx / 8] & (1 << (idx % 8))) != 0;
	}
	else
	{
		return false;
	}
}


/* bits_set_many ********************************************************************************//**
 * @brief		Sets 'count' many bits starting at the specified index.
 * @param[in]	b: the bit array containing the bits to set.
 * @param[in]	start: the first bit to set.
 * @param[in]	count: the number of bits to set.
 * @retval		true if the bits were set successfully.
 * @retval		false if the bits were not set. */
inline bool bits_set_many(Bits* b, unsigned start, unsigned count)
{
	return bits_write_many(b, 1, start, count);
}


/* bits_clear_many ******************************************************************************//**
 * @brief		Clears 'count' many bits starting at the specified index.
 * @param[in]	b: the bit array containing the bits to clear.
 * @param[in]	start: the first bit to clear.
 * @param[in]	count: the number of bits to clear.
 * @retval		true if the bits were cleared successfully.
 * @retval		false if the bits were not cleared. */
inline bool bits_clear_many(Bits* b, unsigned start, unsigned count)
{
	return bits_write_many(b, 0, start, count);
}


#ifdef __cplusplus
}
#endif

#endif // BITS_H
/******************************************* END OF FILE *******************************************/
