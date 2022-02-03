/************************************************************************************************//**
 * @file		byteorder.c
 *
 * @copyright	Copyright 2022 Kurt Hildebrand.
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
#include "byteorder.h"


/* Inline Function Instances --------------------------------------------------------------------- */
// extern uint8_t  get_u8  (const void*);
// extern uint16_t get_u16 (const void*);
// extern uint32_t get_u32 (const void*);
// extern uint64_t get_u64 (const void*);
// extern int8_t   get_i8  (const void*);
// extern int16_t  get_i16 (const void*);
// extern int32_t  get_i32 (const void*);
// extern int64_t  get_i64 (const void*);

// extern bool     set_u8  (void*, uint8_t);
// extern bool     set_u16 (void*, uint16_t);
// extern bool     set_u32 (void*, uint32_t);
// extern bool     set_u64 (void*, uint64_t);
// extern bool     set_i8  (void*, int8_t);
// extern bool     set_i16 (void*, int16_t);
// extern bool     set_i32 (void*, int32_t);
// extern bool     set_i64 (void*, int64_t);

extern bool     move_u8 (void*, const void*);
extern bool     move_u16(void*, const void*);
extern bool     move_u32(void*, const void*);
extern bool     move_u64(void*, const void*);
extern bool     move_i8 (void*, const void*);
extern bool     move_i16(void*, const void*);
extern bool     move_i32(void*, const void*);
extern bool     move_i64(void*, const void*);



extern uint8_t  be_u8 (uint8_t);
extern uint16_t be_u16(uint16_t);
extern uint32_t be_u32(uint32_t);
extern uint64_t be_u64(uint64_t);
extern int8_t   be_i8 (int8_t);
extern int16_t  be_i16(int16_t);
extern int32_t  be_i32(int32_t);
extern int64_t  be_i64(int64_t);

extern uint8_t  le_u8 (uint8_t);
extern uint16_t le_u16(uint16_t);
extern uint32_t le_u32(uint32_t);
extern uint64_t le_u64(uint64_t);
extern int8_t   le_i8 (int8_t);
extern int16_t  le_i16(int16_t);
extern int32_t  le_i32(int32_t);
extern int64_t  le_i64(int64_t);



extern uint8_t  be_get_u8 (const void*);
extern uint16_t be_get_u16(const void*);
extern uint32_t be_get_u32(const void*);
extern uint64_t be_get_u64(const void*);
extern int8_t   be_get_i8 (const void*);
extern int16_t  be_get_i16(const void*);
extern int32_t  be_get_i32(const void*);
extern int64_t  be_get_i64(const void*);

extern uint8_t  le_get_u8 (const void*);
extern uint16_t le_get_u16(const void*);
extern uint32_t le_get_u32(const void*);
extern uint64_t le_get_u64(const void*);
extern int8_t   le_get_i8 (const void*);
extern int16_t  le_get_i16(const void*);
extern int32_t  le_get_i32(const void*);
extern int64_t  le_get_i64(const void*);



extern bool be_set_u8 (void*, uint8_t);
extern bool be_set_u16(void*, uint16_t);
extern bool be_set_u32(void*, uint32_t);
extern bool be_set_u64(void*, uint64_t);
extern bool be_set_i8 (void*, int8_t);
extern bool be_set_i16(void*, int16_t);
extern bool be_set_i32(void*, int32_t);
extern bool be_set_i64(void*, int64_t);

extern bool le_set_u8 (void*, uint8_t);
extern bool le_set_u16(void*, uint16_t);
extern bool le_set_u32(void*, uint32_t);
extern bool le_set_u64(void*, uint64_t);
extern bool le_set_i8 (void*, int8_t);
extern bool le_set_i16(void*, int16_t);
extern bool le_set_i32(void*, int32_t);
extern bool le_set_i64(void*, int64_t);



extern bool be_move_u8 (void*, const void*);
extern bool be_move_u16(void*, const void*);
extern bool be_move_u32(void*, const void*);
extern bool be_move_u64(void*, const void*);
extern bool be_move_i8 (void*, const void*);
extern bool be_move_i16(void*, const void*);
extern bool be_move_i32(void*, const void*);
extern bool be_move_i64(void*, const void*);

extern bool le_move_u8 (void*, const void*);
extern bool le_move_u16(void*, const void*);
extern bool le_move_u32(void*, const void*);
extern bool le_move_u64(void*, const void*);
extern bool le_move_i8 (void*, const void*);
extern bool le_move_i16(void*, const void*);
extern bool le_move_i32(void*, const void*);
extern bool le_move_i64(void*, const void*);





/* ntoh / hton ----------------------------------------------------------------------------------- */
extern uint8_t  ntoh_u8 (uint8_t);
extern uint16_t ntoh_u16(uint16_t);
extern uint32_t ntoh_u32(uint32_t);
extern uint64_t ntoh_u64(uint64_t);
extern int8_t   ntoh_i8 (int8_t);
extern int16_t  ntoh_i16(int16_t);
extern int32_t  ntoh_i32(int32_t);
extern int64_t  ntoh_i64(int64_t);

extern uint8_t  hton_u8 (uint8_t);
extern uint16_t hton_u16(uint16_t);
extern uint32_t hton_u32(uint32_t);
extern uint64_t hton_u64(uint64_t);
extern int8_t   hton_i8 (int8_t);
extern int16_t  hton_i16(int16_t);
extern int32_t  hton_i32(int32_t);
extern int64_t  hton_i64(int64_t);



extern uint8_t  ntoh_get_u8 (const void*);
extern uint16_t ntoh_get_u16(const void*);
extern uint32_t ntoh_get_u32(const void*);
extern uint64_t ntoh_get_u64(const void*);
extern int8_t   ntoh_get_i8 (const void*);
extern int16_t  ntoh_get_i16(const void*);
extern int32_t  ntoh_get_i32(const void*);
extern int64_t  ntoh_get_i64(const void*);

extern uint8_t  hton_get_u8 (const void*);
extern uint16_t hton_get_u16(const void*);
extern uint32_t hton_get_u32(const void*);
extern uint64_t hton_get_u64(const void*);
extern int8_t   hton_get_i8 (const void*);
extern int16_t  hton_get_i16(const void*);
extern int32_t  hton_get_i32(const void*);
extern int64_t  hton_get_i64(const void*);



extern bool ntoh_set_u8 (void*, uint8_t);
extern bool ntoh_set_u16(void*, uint16_t);
extern bool ntoh_set_u32(void*, uint32_t);
extern bool ntoh_set_u64(void*, uint64_t);
extern bool ntoh_set_i8 (void*, int8_t);
extern bool ntoh_set_i16(void*, int16_t);
extern bool ntoh_set_i32(void*, int32_t);
extern bool ntoh_set_i64(void*, int64_t);

extern bool hton_set_u8 (void*, uint8_t);
extern bool hton_set_u16(void*, uint16_t);
extern bool hton_set_u32(void*, uint32_t);
extern bool hton_set_u64(void*, uint64_t);
extern bool hton_set_i8 (void*, int8_t);
extern bool hton_set_i16(void*, int16_t);
extern bool hton_set_i32(void*, int32_t);
extern bool hton_set_i64(void*, int64_t);



extern bool ntoh_move_u8 (void*, const void*);
extern bool ntoh_move_u16(void*, const void*);
extern bool ntoh_move_u32(void*, const void*);
extern bool ntoh_move_u64(void*, const void*);
extern bool ntoh_move_i8 (void*, const void*);
extern bool ntoh_move_i16(void*, const void*);
extern bool ntoh_move_i32(void*, const void*);
extern bool ntoh_move_i64(void*, const void*);

extern bool hton_move_u8 (void*, const void*);
extern bool hton_move_u16(void*, const void*);
extern bool hton_move_u32(void*, const void*);
extern bool hton_move_u64(void*, const void*);
extern bool hton_move_i8 (void*, const void*);
extern bool hton_move_i16(void*, const void*);
extern bool hton_move_i32(void*, const void*);
extern bool hton_move_i64(void*, const void*);


/******************************************* END OF FILE *******************************************/
