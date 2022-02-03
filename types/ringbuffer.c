/************************************************************************************************//**
 * @file		ringbuffer.c
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
#include "ringbuffer.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern bool     rb_init      (RingBuffer*, void*, unsigned, unsigned);
extern bool     rb_init_range(RingBuffer*, Range);
extern Range*   rb_range     (const RingBuffer*);
extern void     rb_clear     (RingBuffer*);
extern Key      rb_key       (const RingBuffer*);
extern unsigned rb_size      (const RingBuffer*);
extern unsigned rb_elemsize  (const RingBuffer*);
extern unsigned rb_count     (const RingBuffer*);
extern unsigned rb_free      (const RingBuffer*);
extern bool     rb_empty     (const RingBuffer*);
extern bool     rb_full      (const RingBuffer*);
extern void*    rb_entry     (const RingBuffer*, unsigned);
extern void*    rb_next      (const RingBuffer*);

extern bool     rb_push_many (RingBuffer*, const void*, unsigned);
extern bool     rb_pop_many  (RingBuffer*, unsigned);

extern void*    rb_reserve   (RingBuffer*);
extern bool     rb_push      (RingBuffer*, const void*);
extern bool     rb_peek      (const RingBuffer*, void*);
extern bool     rb_peek_at   (const RingBuffer*, void*, unsigned);
extern bool     rb_pop       (RingBuffer*);
extern bool     rb_get       (RingBuffer*, void*);


/******************************************* END OF FILE *******************************************/
