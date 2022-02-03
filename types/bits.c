/************************************************************************************************//**
 * @file		bits.c
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
#include <string.h>

#include "bits.h"
#include "calc.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern void     bits_init      (Bits*, void*, unsigned);
extern Bits     make_bits      (void*, unsigned);
extern unsigned bits_count     (const Bits*);
extern bool     bits_value     (const Bits*, unsigned);
extern unsigned bits_start     (void);
extern unsigned bits_end       (const Bits*);

extern unsigned bits_next_zero (Bits*, unsigned);
extern unsigned bits_prev_zero (Bits*, unsigned);
extern unsigned bits_next_one  (Bits*, unsigned);
extern unsigned bits_prev_one  (Bits*, unsigned);

extern bool     bits_set_many  (Bits*, unsigned, unsigned);
extern bool     bits_clear_many(Bits*, unsigned, unsigned);
extern bool     bits_write     (Bits*, bool, unsigned);
extern bool     bits_set       (Bits*, unsigned);
extern bool     bits_clear     (Bits*, unsigned);
extern bool     bits_flip      (Bits*, unsigned);


/* bits_zeros ***********************************************************************************//**
 * @brief		Returns the number of 0's in the bit array. */
unsigned bits_zeros(const Bits* b)
{
	const uint8_t* ptr = (const uint8_t*)(b->ptr);
	unsigned count = 0;
	unsigned i;

	/* Count bits 4 bytes at a time */
	for(i = 0; i < bits_count(b); i += 32)
	{
		uint32_t temp = 0;

		/* Check if 4 or more bytes remain. If so, just copy 4 bytes into temp. Just counting bits
		 * does not require worrying about endianness. */
		if(bits_count(b) - i >= 32)
		{
			memmove(&temp, &ptr[i/8], 4);
		}
		/* Less than 32 bits remain. Copy bytes manually and mask off unwanted bits. */
		else
		{
			uint32_t mask = (UINT32_MAX << (bits_count(b) - i));
			if(bits_count(b) - i > 0)  { temp |= (ptr[i/8 + 0] << 0);  }
			if(bits_count(b) - i > 8)  { temp |= (ptr[i/8 + 1] << 8);  }
			if(bits_count(b) - i > 16) { temp |= (ptr[i/8 + 2] << 16); }
			if(bits_count(b) - i > 24) { temp |= (ptr[i/8 + 3] << 24); }
			temp |= mask;
		}

		// temp = calc_popcount_u32(~temp);
		// // temp = ~temp;
		// // temp = ((temp >> 1)  & 0x55555555) + (temp & 0x55555555);
		// // temp = ((temp >> 2)  & 0x33333333) + (temp & 0x33333333);
		// // temp = ((temp >> 4)  & 0x0F0F0F0F) + (temp & 0x0F0F0F0F);
		// // temp = ((temp >> 8)  & 0x00FF00FF) + (temp & 0x00FF00FF);
		// // temp = ((temp >> 16) & 0x0000FFFF) + (temp & 0x0000FFFF);
		// count += temp;

		count += calc_popcount_u32(~temp);
	}

	return count;
}


/* bits_ones ************************************************************************************//**
 * @brief		Returns the number of 1's in the bit array. */
unsigned bits_ones(const Bits* b)
{
	const uint8_t* ptr = (const uint8_t*)(b->ptr);
	unsigned count = 0;
	unsigned i;

	/* Count bits 4 bytes at a time */
	for(i = 0; i < bits_count(b); i += 32)
	{
		uint32_t temp = 0;

		/* Check if 4 or more bytes remain. If so, just copy 4 bytes into temp. Just counting bits
		 * does not require worrying about endianness. */
		if(bits_count(b) - i >= 32)
		{
			memmove(&temp, &ptr[i/8], 4);
		}
		/* Less than 32 bits remain. Copy bytes manually and mask off unwanted bits. */
		else
		{
			uint32_t mask = (UINT32_MAX << (bits_count(b) - i));
			if(bits_count(b) - i > 0)  { temp |= (ptr[i/8 + 0] << 0);  }
			if(bits_count(b) - i > 8)  { temp |= (ptr[i/8 + 1] << 8);  }
			if(bits_count(b) - i > 16) { temp |= (ptr[i/8 + 2] << 16); }
			if(bits_count(b) - i > 24) { temp |= (ptr[i/8 + 3] << 24); }
			temp &= ~mask;
		}

		// temp = calc_popcount_u32(temp);
		// // temp = ((temp >> 1)  & 0x55555555) + (temp & 0x55555555);
		// // temp = ((temp >> 2)  & 0x33333333) + (temp & 0x33333333);
		// // temp = ((temp >> 4)  & 0x0F0F0F0F) + (temp & 0x0F0F0F0F);
		// // temp = ((temp >> 8)  & 0x00FF00FF) + (temp & 0x00FF00FF);
		// // temp = ((temp >> 16) & 0x0000FFFF) + (temp & 0x0000FFFF);
		// count += temp;

		count += calc_popcount_u32(temp);
	}

	return count;
}


/* bits_next_value ******************************************************************************//**
 * @brief		Returns the position of the next value from the specified index.
 * @param[in]	b: the bit array to search.
 * @param[in]	idx: the index to start the search. The bit at the specified index is included in the
 * 				search. Bits are searched by incrementing the index.
 * @param[in]	value: the value of the search bit: false to find a 0 bit, true to find a 1 bit. */
unsigned bits_next_value(Bits* b, unsigned idx, bool value)
{
	uint8_t* ptr = (uint8_t*)(b->ptr);

	while(idx < bits_end(b))
	{
		if(value && (ptr[idx/8] & (1 << (idx % 8))) != 0)
		{
			return idx;
		}
		else if(!value && (ptr[idx/8] & (1 << (idx % 8))) == 0)
		{
			return idx;
		}

		idx++;
	}

	return -1u;
}


/* bits_prev_value ******************************************************************************//**
 * @brief		Returns the position of the previous value from the specified index.
 * @param[in]	b: the bit array to search.
 * @param[in]	idx: the index to start the search. The bit at the specified index is included in the
 * 				search. Bits are searched by decrementing the index.
 * @param[in]	value: the value of the search bit: false to find a 0 bit, true to find a 1 bit. */
unsigned bits_prev_value(Bits* b, unsigned idx, bool value)
{
	uint8_t* ptr = (uint8_t*)(b->ptr);

	if(idx >= bits_end(b))
	{
		idx = bits_end(b) - 1;
	}

	while(idx != -1u)
	{
		if(value && (ptr[idx/8] & (1 << (idx % 8))) != 0)
		{
			return idx;
		}
		else if(!value && (ptr[idx/8] & (1 << (idx % 8))) == 0)
		{
			return idx;
		}

		idx--;
	}

	return -1u;
}


/* bits_set_all *********************************************************************************//**
 * @brief		Sets all the bits in the bit array. */
void bits_set_all(Bits* b)
{
	uint8_t* ptr = (uint8_t*)(b->ptr);

	/* Set all but the last byte */
	if(bits_count(b) >= 8)
	{
		memset(ptr, 0xFF, bits_count(b) / 8);
	}

	/* Set the last byte */
	if(bits_count(b) % 8 != 0)
	{
		uint8_t  mask = UINT8_MAX << (bits_count(b) % 8);
		unsigned last = (bits_count(b) + 7) / 8 - 1;
		ptr[last] |= ~(mask);
	}
}


/* bits_clear_all *******************************************************************************//**
 * @brief		Clears all the bits in the bit array. */
void bits_clear_all(Bits* b)
{
	uint8_t* ptr = (uint8_t*)(b->ptr);

	/* Clear all but the last byte */
	if(bits_count(b) >= 8)
	{
		memset(ptr, 0, bits_count(b) / 8);
	}

	/* Clear the last byte */
	if(bits_count(b) % 8 != 0)
	{
		uint8_t  mask = UINT8_MAX << (bits_count(b) % 8);
		unsigned last = (bits_count(b) + 7) / 8 - 1;
		ptr[last] &= mask;
	}
}


/* bits_write_many ******************************************************************************//**
 * @brief		Writes the value (0/1) to 'count' number of bits starting at the specified index.
 * @param[in]	b: the bit array containing the bits to change.
 * @param[in]	value: true sets the bit to 1. false sets the bit to 0.
 * @param[in]	start: index of the first bit to write.
 * @param[in]	count: the number of bits to write.
 * @retval		true if the bits were written successfully.
 * @retval		false if the bits were not written. */
bool bits_write_many(Bits* b, bool value, unsigned start, unsigned count)
{
//	uint8_t* ptr = (uint8_t*)(b->ptr);
//
//	if(start >= bits_end(b) || count == 0 || count > bits_count(b) || start + count > bits_end(b))
//	{
//		return false;
//	}
//	else
//	{
//		while(count-- > 0)
//		{
//			if(value)
//			{
//				ptr[start/8] |= (1 << (start % 8));
//			}
//			else
//			{
//				ptr[start/8] &= ~(1 << (start % 8));
//			}
//
//			start++;
//		}
//
//		return true;
//	}



	if(start >= bits_end(b) || count == 0 || count > bits_count(b) || start + count > bits_end(b))
	{
		return false;
	}

	unsigned end = start + count;
	uint8_t* ptr = (uint8_t*)(b->ptr);
	uint8_t startmask =   UINT8_MAX << ( start % 8);		/* Mask msb's down to start */
	uint8_t endmask   = ~(UINT8_MAX << ((end   % 8)));		/* Mask lsb's up to end */
	uint8_t vmask     = value ? 0xFF : 0x00;				/* Write 0's or 1's */

	/* Check if first and last bits are within the same byte */
	if(start / 8 == (end-1) / 8 && endmask)
	{
		ptr[start/8] = (ptr[start/8] & ~(startmask & endmask)) | (startmask & endmask & vmask);
	}
	else
	{
		/* Write first byte */
		ptr[start/8] = (ptr[start / 8] & ~startmask) | (startmask & vmask);

		/* Write middle bytes */
		for(start = ((start + 8) / 8) * 8; start < (end / 8) * 8; start += 8)
		{
			ptr[start/8] = vmask;
		}

		/* Write end byte */
		if(endmask)
		{
			ptr[end/8] = (ptr[end/8] & ~endmask) | (endmask & vmask);
		}
	}

	return true;
}


/* bits_flip_many *******************************************************************************//**
 * @brief		Inverts 'count' number of bits starting at the specified index.
 * @param[in]	b: the bit array containing the bits to flip.
 * @param[in]	start: the index of the first bit to flip.
 * @param[in]	count: the number of bits to flip.
 * @retval		true if the bits were flipped successfully.
 * @retval		false if the bits were not flipped. */
bool bits_flip_many(Bits* b, unsigned start, unsigned count)
{
//	uint8_t* ptr = (uint8_t*)(b->ptr);
//
//	if(start >= bits_end(b) || count == 0 || count > bits_count(b) || start + count > bits_end(b))
//	{
//		return false;
//	}
//	else
//	{
//		while(count-- > 0)
//		{
//			ptr[start/8] ^= (1 << (start % 8));
//
//			start++;
//		}
//
//		return true;
//	}



	if(start >= bits_end(b) || count == 0 || count > bits_count(b) || start + count > bits_end(b))
	{
		return false;
	}

	unsigned end = start + count;
	uint8_t* ptr = (uint8_t*)(b->ptr);
	uint8_t startmask =   UINT8_MAX << ( start % 8);		/* Mask msb's down to start */
	uint8_t endmask   = ~(UINT8_MAX << ((end   % 8)));		/* Mask lsb's up to end */

	/* Check if first and last bits are within the same byte */
	if(start / 8 == (end-1) / 8 && endmask)
	{
		ptr[start/8] ^= (startmask & endmask);
	}
	else
	{
		/* Flip first byte */
		ptr[start/8] ^= startmask;

		/* Flip middle bytes */
		for(start = ((start + 8) / 8) * 8; start < (end / 8) * 8; start += 8)
		{
			ptr[start/8] ^= 0xFF;
		}

		/* Write end byte */
		if(endmask)
		{
			ptr[end/8] ^= endmask;
		}
	}

	return true;
}


/******************************************* END OF FILE *******************************************/
