/************************************************************************************************//**
 * @file		array.c
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
#include "array.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern Array    make_array      (void*, unsigned, unsigned);
extern Array    make_const_array(const void*, unsigned, unsigned);
extern void     array_init      (Array*, void*, unsigned, unsigned);
extern void     array_const_init(Array*, const void*, unsigned, unsigned);
extern void     array_init_range(Array*, Range);
extern Array    array_from_range(Range);

extern Range*   array_range     (const Array*);
extern Key      array_key       (const Array*);
extern unsigned array_count     (const Array*);
extern unsigned array_elemsize  (const Array*);
extern void*    array_entry     (const Array*, unsigned);
extern bool     array_put       (Array*, const void*, unsigned);
extern bool     array_put_many  (Array*, const void*, unsigned, unsigned);
extern bool     array_get       (const Array*, void*, unsigned);
extern bool     array_get_many  (const Array*, void*, unsigned, unsigned);
extern bool     array_swap      (Array*, unsigned, unsigned);


/******************************************* END OF FILE *******************************************/
