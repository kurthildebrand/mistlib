/************************************************************************************************//**
 * @file		range.c
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
#include "range.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern void     range_init      (Range*, void*, unsigned, unsigned);
extern void     range_const_init(Range*, const void*, unsigned, unsigned);
extern bool     range_slice     (Range*, const Range*, unsigned, unsigned);
extern Range    make_range      (void*, unsigned, unsigned);
extern Range    make_const_range(const void*, unsigned, unsigned);
extern Range    make_range_slice(const Range*, unsigned, unsigned);
extern unsigned range_start     (const Range*);
extern unsigned range_end       (const Range*);
extern unsigned range_count     (const Range*);
extern bool     range_empty     (const Range*);
extern unsigned range_elemsize  (const Range*);
extern void*    range_entry     (const Range*, unsigned);
extern void*    range_offset    (const Range*, unsigned);
extern void*    range_at        (const Range*, unsigned);
extern unsigned range_indexof   (const Range*, const void*);
extern unsigned range_offsetof  (const Range*, const void*);
extern bool     range_put       (Range*, const void*, unsigned);
extern bool     range_put_many  (Range*, const void*, unsigned, unsigned);
extern bool     range_get       (const Range*, void*, unsigned);
extern bool     range_get_many  (const Range*, void*, unsigned, unsigned);


/* range_swap ***********************************************************************************//**
 * @brief		Swaps two elements in a range.
 * @param[in]	r: the range to swap elements in.
 * @param[in]	first: the index of the first element.
 * @param[in]	second: the index of the second element.
 * @retval		true if the elements were swapped successfully.
 * @retval		false if one of the indices is out of bounds. */
bool range_swap(Range* r, unsigned first, unsigned second)
{
	unsigned i;
	unsigned char* f = range_entry(r, first);
	unsigned char* s = range_entry(r, second);

	if(!f || !s)
	{
		return false;
	}
	else if(f != s)
	{
		for(i = 0; i < range_elemsize(r); i++)
		{
			unsigned char temp;
			temp = f[i];
			f[i] = s[i];
			s[i] = temp;
		}
	}

	return true;
}


/******************************************* END OF FILE *******************************************/
