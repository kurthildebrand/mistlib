/************************************************************************************************//**
 * @file		pool.c
 *
 * @copyright	Copyright Kurt Hildebrand.
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
#include "pool.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern bool     pool_init           (Pool*, void*, unsigned, unsigned);
extern bool     pool_init_range     (Pool*, Range);
extern void     pool_clear          (Pool*);
extern unsigned pool_size           (const Pool*);
extern unsigned pool_elemsize       (const Pool*);
extern unsigned pool_free           (const Pool*);
extern unsigned pool_count          (const Pool*);
extern bool     pool_empty          (const Pool*);
extern bool     pool_full           (const Pool*);
extern void*    pool_entry          (const Pool*, unsigned);
extern bool     pool_idx_is_reserved(const Pool*, unsigned);
extern bool     pool_ptr_is_reserved(const Pool*, void*);
extern void*    pool_reserve        (Pool*);
extern bool     pool_release        (Pool*, const void*);


/******************************************* END OF FILE *******************************************/
