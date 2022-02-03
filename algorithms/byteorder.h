/************************************************************************************************//**
 * @file		byteorder.h
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
#ifndef BYTE_ORDER_H
#define BYTE_ORDER_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "utils.h"


/* Public Macros --------------------------------------------------------------------------------- */
/* BE / LE **************************************************************************************//**
 * @brief		The macros be and le are helper macros which select be/le get/set based on the number
 * 				of arguments.
 *
 * 				Selects be_get/le_get if given 1 argument.
 * 				Selects be_set/le_set if given 2 arguments.
 *
 * 				See documentation for be_get/le_get and be_set/le_set for details on the
 * 				expected arguments. */
#define be(...)     APPEND_NARGS(be, __VA_ARGS__)
#define be1(x)      be_get(x)
#define be2(y, x)   be_set(y, x)

#define le(...)     APPEND_NARGS(le, __VA_ARGS__)
#define le1(x)      le_get(x)
#define le2(y, x)   le_set(y, x)



/* be_get / le_get ******************************************************************************//**
 * @brief		Returns the argument in Big Endian (be) or Little Endian (le). These are generic
 * 				macros which infer the argument type and can work on variables or pointers to
 * 				variables. For example:
 *
 * 					uint16_t a = 0x1234;
 * 					uint16_t b = be_get(a);		// Infers be_u16
 * 					uint16_t c = be_get(&a);	// Infers be_get_u16
 * 					uint16_t d = le_get(a);		// Infers le_u16
 * 					uint16_t e = le_get(&a);	// Infers le_get_u16
 */
#define be_get(x) _Generic(x, \
	uint8_t:  be_u8,     uint8_t*:  be_get_u8,     const uint8_t*:  be_get_u8,  \
	uint16_t: be_u16,    uint16_t*: be_get_u16,    const uint16_t*: be_get_u16, \
	uint32_t: be_u32,    uint32_t*: be_get_u32,    const uint32_t*: be_get_u32, \
	uint64_t: be_u64,    uint64_t*: be_get_u64,    const uint64_t*: be_get_u64, \
	int8_t:   be_i8,     int8_t*:   be_get_i8,     const int8_t*:   be_get_i8,  \
	int16_t:  be_i16,    int16_t*:  be_get_i16,    const int16_t*:  be_get_i16, \
	int32_t:  be_i32,    int32_t*:  be_get_i32,    const int32_t*:  be_get_i32, \
	int64_t:  be_i64,    int64_t*:  be_get_i64,    const int64_t*:  be_get_i64  \
)(x)

#define le_get(x) _Generic(x, \
	uint8_t:  le_u8,     uint8_t*:  le_get_u8,     const uint8_t*:  le_get_u8,  \
	uint16_t: le_u16,    uint16_t*: le_get_u16,    const uint16_t*: le_get_u16, \
	uint32_t: le_u32,    uint32_t*: le_get_u32,    const uint32_t*: le_get_u32, \
	uint64_t: le_u64,    uint64_t*: le_get_u64,    const uint64_t*: le_get_u64, \
	int8_t:   le_i8,     int8_t*:   le_get_i8,     const int8_t*:   le_get_i8,  \
	int16_t:  le_i16,    int16_t*:  le_get_i16,    const int16_t*:  le_get_i16, \
	int32_t:  le_i32,    int32_t*:  le_get_i32,    const int32_t*:  le_get_i32, \
	int64_t:  le_i64,    int64_t*:  le_get_i64,    const int64_t*:  le_get_i64  \
)(x)



/* be_set / le_set ******************************************************************************//**
 * @brief		Converts x to Big Endian (be) or Little Endian (le) and assigns it to y. */
#define be_set(y, x) _Generic(y, \
	uint8_t*:  be_set_u8,        \
	uint16_t*: be_set_u16,       \
	uint32_t*: be_set_u32,       \
	uint64_t*: be_set_u64,       \
	int8_t*:   be_set_i8,        \
	int16_t*:  be_set_i16,       \
	int32_t*:  be_set_i32,       \
	int64_t*:  be_set_i64        \
)(y,x)

#define le_set(y, x) _Generic(y, \
	uint8_t*:  le_set_u8,        \
	uint16_t*: le_set_u16,       \
	uint32_t*: le_set_u32,       \
	uint64_t*: le_set_u64,       \
	int8_t*:   le_set_i8,        \
	int16_t*:  le_set_i16,       \
	int32_t*:  le_set_i32,       \
	int64_t*:  le_set_i64        \
)(y,x)





/* ntoh / hton **********************************************************************************//**
 * @brief		The macros ntoh and hton are helper macros which select ntoh/hton get/set based on
 * 				the number of arguments.
 *
 * 				Selects ntoh_get/hton_get if given 1 argument.
 * 				Selects ntoh_set/hton_set if given 2 arguments.
 *
 * 				See documentation for ntoh_get/hton_get and ntoh_set/hton_set for details on the
 * 				expected arguments. */
#define ntoh(...)   APPEND_NARGS(ntoh, __VA_ARGS__)
#define ntoh1(x)    ntoh_get(x)
#define ntoh2(y, x) ntoh_set(y, x)

#define hton(...)   APPEND_NARGS(hton, __VA_ARGS__)
#define hton1(x)    hton_get(x)
#define hton2(y, x) hton_set(y, x)


/* ntoh_get / hton_get **************************************************************************//**
 * @brief		Returns the argument in swapped byter order. These are generic macros which infer
 * 				the argument type and can work on variables or pointers to variables. For example:
 *
 * 					uint16_t a = 0x1234;
 * 					uint16_t b = ntoh_get(a);	// Infers ntoh_u16
 * 					uint16_t c = hton_get(&a);	// Infers ntoh_get_u16
 */
#define ntoh_get(x) _Generic(x, \
	uint8_t:  ntoh_u8,     uint8_t*:  ntoh_get_u8,     const uint8_t*:  ntoh_get_u8,  \
	uint16_t: ntoh_u16,    uint16_t*: ntoh_get_u16,    const uint16_t*: ntoh_get_u16, \
	uint32_t: ntoh_u32,    uint32_t*: ntoh_get_u32,    const uint32_t*: ntoh_get_u32, \
	uint64_t: ntoh_u64,    uint64_t*: ntoh_get_u64,    const uint64_t*: ntoh_get_u64, \
	int8_t:   ntoh_i8,     int8_t*:   ntoh_get_i8,     const int8_t*:   ntoh_get_i8,  \
	int16_t:  ntoh_i16,    int16_t*:  ntoh_get_i16,    const int16_t*:  ntoh_get_i16, \
	int32_t:  ntoh_i32,    int32_t*:  ntoh_get_i32,    const int32_t*:  ntoh_get_i32, \
	int64_t:  ntoh_i64,    int64_t*:  ntoh_get_i64,    const int64_t*:  ntoh_get_i64  \
)(x)

#define hton_get(x) _Generic(x, \
	uint8_t:  hton_u8,     uint8_t*:  hton_get_u8,     const uint8_t*:  hton_get_u8,  \
	uint16_t: hton_u16,    uint16_t*: hton_get_u16,    const uint16_t*: hton_get_u16, \
	uint32_t: hton_u32,    uint32_t*: hton_get_u32,    const uint32_t*: hton_get_u32, \
	uint64_t: hton_u64,    uint64_t*: hton_get_u64,    const uint64_t*: hton_get_u64, \
	int8_t:   hton_i8,     int8_t*:   hton_get_i8,     const int8_t*:   hton_get_i8,  \
	int16_t:  hton_i16,    int16_t*:  hton_get_i16,    const int16_t*:  hton_get_i16, \
	int32_t:  hton_i32,    int32_t*:  hton_get_i32,    const int32_t*:  hton_get_i32, \
	int64_t:  hton_i64,    int64_t*:  hton_get_i64,    const int64_t*:  hton_get_i64  \
)(x)


/* ntoh_set / hton_set **************************************************************************//**
 * @brief		Assigns x to the variable pointed to by y with swapped byte order. For example:
 *
 * 					uint16_t a;
 * 					ntoh_set(&a, 0x1234);
 */
#define ntoh_set(y, x) _Generic(y, \
	uint8_t*:  ntoh_set_u8,        \
	uint16_t*: ntoh_set_u16,       \
	uint32_t*: ntoh_set_u32,       \
	uint64_t*: ntoh_set_u64,       \
	int8_t*:   ntoh_set_i8,        \
	int16_t*:  ntoh_set_i16,       \
	int32_t*:  ntoh_set_i32,       \
	int64_t*:  ntoh_set_i64        \
)(y,x)

#define hton_set(y, x) _Generic(y, \
	uint8_t*:  hton_set_u8,        \
	uint16_t*: hton_set_u16,       \
	uint32_t*: hton_set_u32,       \
	uint64_t*: hton_set_u64,       \
	int8_t*:   hton_set_i8,        \
	int16_t*:  hton_set_i16,       \
	int32_t*:  hton_set_i32,       \
	int64_t*:  hton_set_i64        \
)(y,x)





/* Public Functions ------------------------------------------------------------------------------ */
// inline uint8_t  get_u8  (const void* p) { uint8_t  x; return p ? (memmove(&x, p, 1), x) : 0; }
// inline uint16_t get_u16 (const void* p) { uint16_t x; return p ? (memmove(&x, p, 2), x) : 0; }
// inline uint32_t get_u32 (const void* p) { uint32_t x; return p ? (memmove(&x, p, 4), x) : 0; }
// inline uint64_t get_u64 (const void* p) { uint64_t x; return p ? (memmove(&x, p, 8), x) : 0; }
// inline int8_t   get_i8  (const void* p) { int8_t   x; return p ? (memmove(&x, p, 1), x) : 0; }
// inline int16_t  get_i16 (const void* p) { int16_t  x; return p ? (memmove(&x, p, 2), x) : 0; }
// inline int32_t  get_i32 (const void* p) { int32_t  x; return p ? (memmove(&x, p, 4), x) : 0; }
// inline int64_t  get_i64 (const void* p) { int64_t  x; return p ? (memmove(&x, p, 8), x) : 0; }

// inline bool     set_u8  (void* p, uint8_t  x) { return p ? (memmove(p, &x, 1),true) : false ;}
// inline bool     set_u16 (void* p, uint16_t x) { return p ? (memmove(p, &x, 2),true) : false ;}
// inline bool     set_u32 (void* p, uint32_t x) { return p ? (memmove(p, &x, 4),true) : false ;}
// inline bool     set_u64 (void* p, uint64_t x) { return p ? (memmove(p, &x, 8),true) : false ;}
// inline bool     set_i8  (void* p, int8_t   x) { return p ? (memmove(p, &x, 1),true) : false ;}
// inline bool     set_i16 (void* p, int16_t  x) { return p ? (memmove(p, &x, 2),true) : false ;}
// inline bool     set_i32 (void* p, int32_t  x) { return p ? (memmove(p, &x, 4),true) : false ;}
// inline bool     set_i64 (void* p, int64_t  x) { return p ? (memmove(p, &x, 8),true) : false ;}

inline bool     move_u8 (void* d, const void* s) { return (d && s) ? (memmove(d,s,1),true) : false; }
inline bool     move_u16(void* d, const void* s) { return (d && s) ? (memmove(d,s,2),true) : false; }
inline bool     move_u32(void* d, const void* s) { return (d && s) ? (memmove(d,s,4),true) : false; }
inline bool     move_u64(void* d, const void* s) { return (d && s) ? (memmove(d,s,8),true) : false; }
inline bool     move_i8 (void* d, const void* s) { return (d && s) ? (memmove(d,s,1),true) : false; }
inline bool     move_i16(void* d, const void* s) { return (d && s) ? (memmove(d,s,2),true) : false; }
inline bool     move_i32(void* d, const void* s) { return (d && s) ? (memmove(d,s,4),true) : false; }
inline bool     move_i64(void* d, const void* s) { return (d && s) ? (memmove(d,s,8),true) : false; }


/* Big Endian Conversion: converts val to its big endian representation. */
inline uint8_t be_u8(uint8_t val)
{
	return val;
}

inline uint16_t be_u16(uint16_t val)
{
	uint8_t b[sizeof(uint16_t)];
	memmove(b, &val, sizeof(uint16_t));
	return ((uint16_t)b[1] << 0) |
		   ((uint16_t)b[0] << 8);
}

inline uint32_t be_u32(uint32_t val)
{
	uint8_t b[sizeof(uint32_t)];
	memmove(b, &val, sizeof(uint32_t));
	return ((uint32_t)b[3] << 0)  |
	       ((uint32_t)b[2] << 8)  |
		   ((uint32_t)b[1] << 16) |
		   ((uint32_t)b[0] << 24);
}

inline uint64_t be_u64(uint64_t val)
{
	uint8_t b[sizeof(uint64_t)];
	memmove(b, &val, sizeof(uint64_t));
	return ((uint64_t)b[7] << 0)  |
	       ((uint64_t)b[6] << 8)  |
	       ((uint64_t)b[5] << 16) |
	       ((uint64_t)b[4] << 24) |
	       ((uint64_t)b[3] << 32) |
	       ((uint64_t)b[2] << 40) |
	       ((uint64_t)b[1] << 48) |
	       ((uint64_t)b[0] << 56);
}



/* Little Endian Conversion: converts val to its little endian representation. */
inline uint8_t le_u8(uint8_t val)
{
	return val;
}


inline uint16_t le_u16(uint16_t val)
{
	uint16_t ret;
	uint8_t b[sizeof(uint16_t)];
	b[1] = (uint8_t)(val >> 8);
	b[0] = (uint8_t)(val >> 0);
	memmove(&ret, b, sizeof(uint16_t));
	return ret;
}


inline uint32_t le_u32(uint32_t val)
{
	uint32_t ret;
	uint8_t b[sizeof(uint32_t)];
	b[3] = (uint8_t)(val >> 24);
	b[2] = (uint8_t)(val >> 16);
	b[1] = (uint8_t)(val >> 8);
	b[0] = (uint8_t)(val >> 0);
	memmove(&ret, b, sizeof(uint32_t));
	return ret;
}


inline uint64_t le_u64(uint64_t val)
{
	uint64_t ret;
	uint8_t b[sizeof(uint64_t)];
	b[7] = (uint8_t)(val >> 56);
	b[6] = (uint8_t)(val >> 48);
	b[5] = (uint8_t)(val >> 40);
	b[4] = (uint8_t)(val >> 32);
	b[3] = (uint8_t)(val >> 24);
	b[2] = (uint8_t)(val >> 16);
	b[1] = (uint8_t)(val >> 8);
	b[0] = (uint8_t)(val >> 0);
	memmove(&ret, b, sizeof(uint64_t));
	return ret;
}


inline int8_t  be_i8 (int8_t  val) { return (int8_t )be_u8 ((uint8_t )val); }
inline int16_t be_i16(int16_t val) { return (int16_t)be_u16((uint16_t)val); }
inline int32_t be_i32(int32_t val) { return (int32_t)be_u32((uint32_t)val); }
inline int64_t be_i64(int64_t val) { return (int64_t)be_u64((uint64_t)val); }
inline int8_t  le_i8 (int8_t  val) { return (int8_t) le_u8 ((uint8_t )val); }
inline int16_t le_i16(int16_t val) { return (int16_t)le_u16((uint16_t)val); }
inline int32_t le_i32(int32_t val) { return (int32_t)le_u32((uint32_t)val); }
inline int64_t le_i64(int64_t val) { return (int64_t)le_u64((uint64_t)val); }



inline uint8_t  be_get_u8 (const void* p) {uint8_t  x; return p ? (move_u8 (&x,p),be_u8 (x)) : 0;}
inline uint16_t be_get_u16(const void* p) {uint16_t x; return p ? (move_u16(&x,p),be_u16(x)) : 0;}
inline uint32_t be_get_u32(const void* p) {uint32_t x; return p ? (move_u32(&x,p),be_u32(x)) : 0;}
inline uint64_t be_get_u64(const void* p) {uint64_t x; return p ? (move_u64(&x,p),be_u64(x)) : 0;}
inline int8_t   be_get_i8 (const void* p) {int8_t   x; return p ? (move_i8 (&x,p),be_i8 (x)) : 0;}
inline int16_t  be_get_i16(const void* p) {int16_t  x; return p ? (move_i16(&x,p),be_i16(x)) : 0;}
inline int32_t  be_get_i32(const void* p) {int32_t  x; return p ? (move_i32(&x,p),be_i32(x)) : 0;}
inline int64_t  be_get_i64(const void* p) {int64_t  x; return p ? (move_i64(&x,p),be_i64(x)) : 0;}

inline uint8_t  le_get_u8 (const void* p) {uint8_t  x; return p ? (move_u8 (&x,p),le_u8 (x)) : 0;}
inline uint16_t le_get_u16(const void* p) {uint16_t x; return p ? (move_u16(&x,p),le_u16(x)) : 0;}
inline uint32_t le_get_u32(const void* p) {uint32_t x; return p ? (move_u32(&x,p),le_u32(x)) : 0;}
inline uint64_t le_get_u64(const void* p) {uint64_t x; return p ? (move_u64(&x,p),le_u64(x)) : 0;}
inline int8_t   le_get_i8 (const void* p) {int8_t   x; return p ? (move_i8 (&x,p),le_i8 (x)) : 0;}
inline int16_t  le_get_i16(const void* p) {int16_t  x; return p ? (move_i16(&x,p),le_i16(x)) : 0;}
inline int32_t  le_get_i32(const void* p) {int32_t  x; return p ? (move_i32(&x,p),le_i32(x)) : 0;}
inline int64_t  le_get_i64(const void* p) {int64_t  x; return p ? (move_i64(&x,p),le_i64(x)) : 0;}



inline bool be_set_u8 (void* p, uint8_t  x) { return p?(x=be_u8 (x),move_u8 (p,&x),true) : false; }
inline bool be_set_u16(void* p, uint16_t x) { return p?(x=be_u16(x),move_u16(p,&x),true) : false; }
inline bool be_set_u32(void* p, uint32_t x) { return p?(x=be_u32(x),move_u32(p,&x),true) : false; }
inline bool be_set_u64(void* p, uint64_t x) { return p?(x=be_u64(x),move_u64(p,&x),true) : false; }
inline bool be_set_i8 (void* p, int8_t   x) { return p?(x=be_i8 (x),move_i8 (p,&x),true) : false; }
inline bool be_set_i16(void* p, int16_t  x) { return p?(x=be_i16(x),move_i16(p,&x),true) : false; }
inline bool be_set_i32(void* p, int32_t  x) { return p?(x=be_i32(x),move_i32(p,&x),true) : false; }
inline bool be_set_i64(void* p, int64_t  x) { return p?(x=be_i64(x),move_i64(p,&x),true) : false; }

inline bool le_set_u8 (void* p, uint8_t  x) { return p?(x=le_u8 (x),move_u8 (p,&x),true) : false; }
inline bool le_set_u16(void* p, uint16_t x) { return p?(x=le_u16(x),move_u16(p,&x),true) : false; }
inline bool le_set_u32(void* p, uint32_t x) { return p?(x=le_u32(x),move_u32(p,&x),true) : false; }
inline bool le_set_u64(void* p, uint64_t x) { return p?(x=le_u64(x),move_u64(p,&x),true) : false; }
inline bool le_set_i8 (void* p, int8_t   x) { return p?(x=le_i8 (x),move_i8 (p,&x),true) : false; }
inline bool le_set_i16(void* p, int16_t  x) { return p?(x=le_i16(x),move_i16(p,&x),true) : false; }
inline bool le_set_i32(void* p, int32_t  x) { return p?(x=le_i32(x),move_i32(p,&x),true) : false; }
inline bool le_set_i64(void* p, int64_t  x) { return p?(x=le_i64(x),move_i64(p,&x),true) : false; }



inline bool be_move_u8 (void* d, const void* s) { return s ? be_set_u8 (d, be_get_u8 (s)) : false; }
inline bool be_move_u16(void* d, const void* s) { return s ? be_set_u16(d, be_get_u16(s)) : false; }
inline bool be_move_u32(void* d, const void* s) { return s ? be_set_u32(d, be_get_u32(s)) : false; }
inline bool be_move_u64(void* d, const void* s) { return s ? be_set_u64(d, be_get_u64(s)) : false; }
inline bool be_move_i8 (void* d, const void* s) { return s ? be_set_i8 (d, be_get_i8 (s)) : false; }
inline bool be_move_i16(void* d, const void* s) { return s ? be_set_i16(d, be_get_i16(s)) : false; }
inline bool be_move_i32(void* d, const void* s) { return s ? be_set_i32(d, be_get_i32(s)) : false; }
inline bool be_move_i64(void* d, const void* s) { return s ? be_set_i64(d, be_get_i64(s)) : false; }

inline bool le_move_u8 (void* d, const void* s) { return s ? be_set_u8 (d, be_get_u8 (s)) : false; }
inline bool le_move_u16(void* d, const void* s) { return s ? be_set_u16(d, be_get_u16(s)) : false; }
inline bool le_move_u32(void* d, const void* s) { return s ? be_set_u32(d, be_get_u32(s)) : false; }
inline bool le_move_u64(void* d, const void* s) { return s ? be_set_u64(d, be_get_u64(s)) : false; }
inline bool le_move_i8 (void* d, const void* s) { return s ? be_set_i8 (d, be_get_i8 (s)) : false; }
inline bool le_move_i16(void* d, const void* s) { return s ? be_set_i16(d, be_get_i16(s)) : false; }
inline bool le_move_i32(void* d, const void* s) { return s ? be_set_i32(d, be_get_i32(s)) : false; }
inline bool le_move_i64(void* d, const void* s) { return s ? be_set_i64(d, be_get_i64(s)) : false; }





/* ntoh / hton ----------------------------------------------------------------------------------- */
inline uint8_t  ntoh_u8 (uint8_t  x) { return be_u8 (x); }
inline uint16_t ntoh_u16(uint16_t x) { return be_u16(x); }
inline uint32_t ntoh_u32(uint32_t x) { return be_u32(x); }
inline uint64_t ntoh_u64(uint64_t x) { return be_u64(x); }
inline int8_t   ntoh_i8 (int8_t   x) { return be_i8 (x); }
inline int16_t  ntoh_i16(int16_t  x) { return be_i16(x); }
inline int32_t  ntoh_i32(int32_t  x) { return be_i32(x); }
inline int64_t  ntoh_i64(int64_t  x) { return be_i64(x); }

inline uint8_t  hton_u8 (uint8_t  x) { return ntoh_u8 (x); }
inline uint16_t hton_u16(uint16_t x) { return ntoh_u16(x); }
inline uint32_t hton_u32(uint32_t x) { return ntoh_u32(x); }
inline uint64_t hton_u64(uint64_t x) { return ntoh_u64(x); }
inline int8_t   hton_i8 (int8_t   x) { return ntoh_i8 (x); }
inline int16_t  hton_i16(int16_t  x) { return ntoh_i16(x); }
inline int32_t  hton_i32(int32_t  x) { return ntoh_i32(x); }
inline int64_t  hton_i64(int64_t  x) { return ntoh_i64(x); }



inline uint8_t  ntoh_get_u8 (const void* p) { return be_get_u8 (p); }
inline uint16_t ntoh_get_u16(const void* p) { return be_get_u16(p); }
inline uint32_t ntoh_get_u32(const void* p) { return be_get_u32(p); }
inline uint64_t ntoh_get_u64(const void* p) { return be_get_u64(p); }
inline int8_t   ntoh_get_i8 (const void* p) { return be_get_i8 (p); }
inline int16_t  ntoh_get_i16(const void* p) { return be_get_i16(p); }
inline int32_t  ntoh_get_i32(const void* p) { return be_get_i32(p); }
inline int64_t  ntoh_get_i64(const void* p) { return be_get_i64(p); }

inline uint8_t  hton_get_u8 (const void* p) { return ntoh_get_u8 (p); }
inline uint16_t hton_get_u16(const void* p) { return ntoh_get_u16(p); }
inline uint32_t hton_get_u32(const void* p) { return ntoh_get_u32(p); }
inline uint64_t hton_get_u64(const void* p) { return ntoh_get_u64(p); }
inline int8_t   hton_get_i8 (const void* p) { return ntoh_get_i8 (p); }
inline int16_t  hton_get_i16(const void* p) { return ntoh_get_i16(p); }
inline int32_t  hton_get_i32(const void* p) { return ntoh_get_i32(p); }
inline int64_t  hton_get_i64(const void* p) { return ntoh_get_i64(p); }



inline bool ntoh_set_u8 (void* p, uint8_t  x) { return be_set_u8 (p, x); }
inline bool ntoh_set_u16(void* p, uint16_t x) { return be_set_u16(p, x); }
inline bool ntoh_set_u32(void* p, uint32_t x) { return be_set_u32(p, x); }
inline bool ntoh_set_u64(void* p, uint64_t x) { return be_set_u64(p, x); }
inline bool ntoh_set_i8 (void* p, int8_t   x) { return be_set_i8 (p, x); }
inline bool ntoh_set_i16(void* p, int16_t  x) { return be_set_i16(p, x); }
inline bool ntoh_set_i32(void* p, int32_t  x) { return be_set_i32(p, x); }
inline bool ntoh_set_i64(void* p, int64_t  x) { return be_set_i64(p, x); }

inline bool hton_set_u8 (void* p, uint8_t  x) { return ntoh_set_u8 (p, x); }
inline bool hton_set_u16(void* p, uint16_t x) { return ntoh_set_u16(p, x); }
inline bool hton_set_u32(void* p, uint32_t x) { return ntoh_set_u32(p, x); }
inline bool hton_set_u64(void* p, uint64_t x) { return ntoh_set_u64(p, x); }
inline bool hton_set_i8 (void* p, int8_t   x) { return ntoh_set_i8 (p, x); }
inline bool hton_set_i16(void* p, int16_t  x) { return ntoh_set_i16(p, x); }
inline bool hton_set_i32(void* p, int32_t  x) { return ntoh_set_i32(p, x); }
inline bool hton_set_i64(void* p, int64_t  x) { return ntoh_set_i64(p, x); }



inline bool ntoh_move_u8 (void* d, const void* s) { return be_move_u8 (d, s); }
inline bool ntoh_move_u16(void* d, const void* s) { return be_move_u16(d, s); }
inline bool ntoh_move_u32(void* d, const void* s) { return be_move_u32(d, s); }
inline bool ntoh_move_u64(void* d, const void* s) { return be_move_u64(d, s); }
inline bool ntoh_move_i8 (void* d, const void* s) { return be_move_i8 (d, s); }
inline bool ntoh_move_i16(void* d, const void* s) { return be_move_i16(d, s); }
inline bool ntoh_move_i32(void* d, const void* s) { return be_move_i32(d, s); }
inline bool ntoh_move_i64(void* d, const void* s) { return be_move_i64(d, s); }

inline bool hton_move_u8 (void* d, const void* s) { return ntoh_move_u8 (d, s); }
inline bool hton_move_u16(void* d, const void* s) { return ntoh_move_u16(d, s); }
inline bool hton_move_u32(void* d, const void* s) { return ntoh_move_u32(d, s); }
inline bool hton_move_u64(void* d, const void* s) { return ntoh_move_u64(d, s); }
inline bool hton_move_i8 (void* d, const void* s) { return ntoh_move_i8 (d, s); }
inline bool hton_move_i16(void* d, const void* s) { return ntoh_move_i16(d, s); }
inline bool hton_move_i32(void* d, const void* s) { return ntoh_move_i32(d, s); }
inline bool hton_move_i64(void* d, const void* s) { return ntoh_move_i64(d, s); }


#ifdef __cplusplus
}
#endif

#endif // BYTE_ORDER_H
/******************************************* END OF FILE *******************************************/
