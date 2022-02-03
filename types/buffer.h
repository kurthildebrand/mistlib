/************************************************************************************************//**
 * @file		buffer.h
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
#ifndef BUFFER_H
#define BUFFER_H

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

#include "byteorder.h"
#include "key.h"
#include "list.h"
#include "range.h"


/* Public Macros --------------------------------------------------------------------------------- */
/* MAKE_STRING **********************************************************************************//**
 * @brief		Initializes a String at compile time. THis macro may be called with a varying number
 * 				of arguments.
 * @param[in]	s: pointer to the string.
 * @param[in]	len: the length of the string. */
#define MAKE_STRING(...)		APPEND_NARGS(MAKE_STRING, __VA_ARGS__)
#define MAKE_STRING1(s)			MAKE_STRING2(s, strlen(s))
#define MAKE_STRING2(s,len)		(String){ .start = s, .end = &s[len] }





/* Public Types ---------------------------------------------------------------------------------- */
typedef struct Buffer {
	struct Buffer* parent;
	uint8_t* start;
	uint8_t* end;
	uint8_t* read;
	uint8_t* write;
} Buffer;


typedef struct {
	const char* start;
	const char* end;
} String;





/* Public Buffer Functions ----------------------------------------------------------------------- */
inline void*       buffer_init          (Buffer*, void*, unsigned, unsigned);
inline void*       buffer_slice         (Buffer*, const Buffer*, uint8_t*, unsigned);
inline Buffer      make_buffer          (void*, unsigned, unsigned);
inline Buffer      make_buffer_slice    (const Buffer*, uint8_t*, unsigned);

inline void        buffer_clear         (Buffer*);
inline void*       buffer_set_length    (Buffer*, unsigned);
inline Buffer*     buffer_parent        (const Buffer* b) { return (Buffer*)b->parent;              }
inline Buffer*     buffer_top_parent    (const Buffer*);
inline bool        buffer_is_valid      (const Buffer*);

inline unsigned    buffer_size          (const Buffer* b) { return b->end   - b->start;             }
inline unsigned    buffer_length        (const Buffer* b) { return b->write - b->start;             }
inline unsigned    buffer_remaining     (const Buffer* b) { return b->write - b->read;              }
inline unsigned    buffer_free          (const Buffer* b) { return buffer_size(b)-buffer_length(b); }
inline unsigned    buffer_tailroom      (const Buffer* b) { return buffer_free(b);                  }
inline uint8_t*    buffer_start         (const Buffer* b) { return b->start;                        }
inline uint8_t*    buffer_end           (const Buffer* b) { return b->end;                          }
inline uint8_t*    buffer_read          (const Buffer* b) { return b->read;                         }
inline uint8_t*    buffer_write         (const Buffer* b) { return b->write;                        }
inline uint8_t*    buffer_offset        (const Buffer*, unsigned);
inline unsigned    buffer_offsetof      (const Buffer*, const uint8_t*);

inline bool        buffer_write_set     (Buffer*, uint8_t*);
inline bool        buffer_read_set      (Buffer*, uint8_t*);
inline bool        buffer_write_seek    (Buffer*, unsigned);
inline bool        buffer_read_seek     (Buffer*, unsigned);

inline void*       buffer_peek_at       (const Buffer*, const uint8_t*, unsigned);
inline bool        buffer_read_at       (const Buffer*, void*, const uint8_t*, unsigned);
inline bool        buffer_write_at      (Buffer*, const void*, uint8_t*, unsigned);
inline void*       buffer_reserve_at    (Buffer*, uint8_t*, unsigned);
inline bool        buffer_replace_at    (Buffer*, const void*, uint8_t*, unsigned);

inline void*       buffer_peek_offset   (const Buffer*, unsigned, unsigned);
inline bool        buffer_read_offset   (const Buffer*, void*, unsigned, unsigned);
inline bool        buffer_write_offset  (Buffer*, const void*, unsigned, unsigned);
inline void*       buffer_reserve_offset(Buffer*, unsigned, unsigned);
inline bool        buffer_replace_offset(Buffer*, const void*, unsigned, unsigned);

inline void*       buffer_peek          (const Buffer*, unsigned);
inline void*       buffer_reserve       (Buffer*, unsigned);
inline void*       buffer_pop           (Buffer*, unsigned);


inline bool        buffer_push_mem      (Buffer*, const void*, unsigned);
       bool        buffer_push_printf   (Buffer*, const char*, ...);
inline bool        buffer_push_string   (Buffer*, String);
inline bool        buffer_push_u8       (Buffer* b, uint8_t  x) { return buffer_push_mem(b, &x, 1); }
inline bool        buffer_push_u16      (Buffer* b, uint16_t x) { return buffer_push_mem(b, &x, 2); }
inline bool        buffer_push_u32      (Buffer* b, uint32_t x) { return buffer_push_mem(b, &x, 4); }
inline bool        buffer_push_u64      (Buffer* b, uint64_t x) { return buffer_push_mem(b, &x, 8); }
inline bool        buffer_push_i8       (Buffer* b, int8_t   x) { return buffer_push_mem(b, &x, 1); }
inline bool        buffer_push_i16      (Buffer* b, int16_t  x) { return buffer_push_mem(b, &x, 2); }
inline bool        buffer_push_i32      (Buffer* b, int32_t  x) { return buffer_push_mem(b, &x, 4); }
inline bool        buffer_push_i64      (Buffer* b, int64_t  x) { return buffer_push_mem(b, &x, 8); }


inline bool        buffer_peek_mem      (const Buffer*, void*, unsigned);
inline String      buffer_peek_string   (const Buffer*, unsigned);
inline void*       buffer_peek_u8       (const Buffer* b) { return buffer_peek(b, 1); }
inline void*       buffer_peek_u16      (const Buffer* b) { return buffer_peek(b, 2); }
inline void*       buffer_peek_u32      (const Buffer* b) { return buffer_peek(b, 4); }
inline void*       buffer_peek_u64      (const Buffer* b) { return buffer_peek(b, 8); }
inline void*       buffer_peek_i8       (const Buffer* b) { return buffer_peek(b, 1); }
inline void*       buffer_peek_i16      (const Buffer* b) { return buffer_peek(b, 2); }
inline void*       buffer_peek_i32      (const Buffer* b) { return buffer_peek(b, 4); }
inline void*       buffer_peek_i64      (const Buffer* b) { return buffer_peek(b, 8); }


inline bool        buffer_pop_mem       (Buffer*, void*, unsigned);
inline String      buffer_pop_string    (Buffer*, unsigned);
inline void*       buffer_pop_u8        (Buffer* b) { return buffer_pop(b, 1); }
inline void*       buffer_pop_u16       (Buffer* b) { return buffer_pop(b, 2); }
inline void*       buffer_pop_u32       (Buffer* b) { return buffer_pop(b, 4); }
inline void*       buffer_pop_u64       (Buffer* b) { return buffer_pop(b, 8); }
inline void*       buffer_pop_i8        (Buffer* b) { return buffer_pop(b, 1); }
inline void*       buffer_pop_i16       (Buffer* b) { return buffer_pop(b, 2); }
inline void*       buffer_pop_i32       (Buffer* b) { return buffer_pop(b, 4); }
inline void*       buffer_pop_i64       (Buffer* b) { return buffer_pop(b, 8); }





/* Public String Functions ----------------------------------------------------------------------- */
inline void        string_init          (String*, const char*, const char*);
inline void        string_init_len      (String*, const char*, unsigned);
inline String      make_string          (const char*, const char*);
inline String      make_string_len      (const char*, unsigned);
inline String      make_empty_string    (void);
inline const char* string_start         (const String* s)          { return s->start; }
inline const char* string_end           (const String* s)          { return s->end;   }
inline const char* string_offset        (const String*, unsigned);
inline unsigned    string_offsetof      (const String*, const char*);
inline unsigned    string_length        (const String* s)          { return s->end - s->start; }
inline bool        string_empty         (const String* s)          { return string_length(s) == 0;  }

       unsigned    string_copy          (String, void*, unsigned);
inline String      string_dup           (String, void*, unsigned);
       String      string_token         (String*, const char*);
       int         string_cmp           (String,   String);
inline int         compare_strings      (const void*, const void*);
       int         string_casecmp       (String,   String);
inline bool        string_equal         (String a, String b) { return string_cmp(a, b) == 0; }
       String      string_search        (String,   String);

    //    int         string_to_int    (String);
    //    unsigned    string_to_uint   (String);
    //    float       string_to_float  (String);
    //    double      string_to_double (String);





// ----------------------------------------------------------------------------------------------- //
// Buffer                                                                                          //
// ----------------------------------------------------------------------------------------------- //
/* buffer_init **********************************************************************************//**
 * @brief		Initializes a buffer's data.
 * @param[in]	b: the buffer to initialize.
 * @param[in]	data: pointer to the buffer's data.
 * @param[in]	count: the initial number of bytes in the buffer.
 * @param[in]	size: the number of bytes in the buffer. */
inline void* buffer_init(Buffer* b, void* data, unsigned count, unsigned size)
{
	if(count <= size)
	{
		b->parent = 0;
		b->start  = data;
		b->end    = b->start + size;
		b->read   = b->start;
		b->write  = (uint8_t*)b->start + count;
		return data;
	}
	else
	{
		return 0;
	}
}


/* buffer_slice *********************************************************************************//**
 * @brief		Initializes the destination buffer to be equal to or smaller than the source buffer.
 * 				The destination buffer is linked to the source buffer so that writes to dest are
 * 				reflected in src. However, reads from dest do not modify src.
 * @param[in]	dest: the destination buffer to initialize.
 * @param[in]	src: the larger source buffer.
 * @param[in]	start: pointer to the first byte in dest.
 * @param[in]	len: number of bytes currently in dest.
 * @return		Pointer to the first byte in dest. */
inline void* buffer_slice(Buffer* dest, const Buffer* src, uint8_t* start, unsigned len)
{
	if(!dest)
	{
		return 0;
	}

	uint8_t* end = start + len;

	if(!buffer_is_valid(src) || src->start > start || start > end || end > src->write)
	{
		dest->parent = (Buffer*)src;
		dest->start  = 0;
		dest->end    = 0;
		dest->write  = 0;
		dest->read   = 0;
		return 0;
	}
	else
	{
		dest->parent = (Buffer*)(dest == src ? src->parent : src);
		dest->start  = start;
		dest->end    = src->end;
		dest->write  = end;
		dest->read   = start;
		return start;
	}
}


/* make_buffer **********************************************************************************//**
 * @brief		Returns a buffer representing the specified array.
 * @param[in]	data: pointer to the buffer's data.
 * @param[in]	count: the initial number of bytes in the buffer.
 * @param[in]	size: the number of bytes in the buffer. */
inline Buffer make_buffer(void* data, unsigned count, unsigned size)
{
	if(count <= size)
	{
		uint8_t* start = data;

		return (Buffer){
			.parent = 0,
			.start  = start,
			.end    = start + size,
			.write  = start + count,
			.read   = start,
		};
	}
	else
	{
		return (Buffer){
			.parent = 0,
			.start  = 0,
			.end    = 0,
			.write  = 0,
			.read   = 0,
		};
	}
}


/* make_buffer_slice ****************************************************************************//**
 * @brief		Returns a buffer that is equal to or smaller than the source buffer. The returned
 * 				buffer is linked to the source buffer so that writes to the returned buffer are
 * 				reflected in src. however, reads from the returned buffer do not modify src.
 * @param[in]	src: the source buffer.
 * @param[in]	start: pointer to the first byte in dest.
 * @param[in]	len: number of bytes currently in dest. */
inline Buffer make_buffer_slice(const Buffer* src, uint8_t* start, unsigned len)
{
	uint8_t* end = start + len;

	if(!buffer_is_valid(src) || src->start > start || start > end || end > src->write)
	{
		return (Buffer){
			.parent = (Buffer*)src,
			.start  = 0,
			.end    = 0,
			.write  = 0,
			.read   = 0,
		};
	}
	else
	{
		return (Buffer){
			.parent = (Buffer*)src,
			.start  = start,
			.end    = src->end,
			.write  = end,
			.read   = start,
		};
	}
}


/* buffer_clear *********************************************************************************//**
 * @brief		Zeros out the buffer's data. And resets the cursor and the end index. */
inline void buffer_clear(Buffer* b)
{
	b->read  = b->start;
	b->write = b->start;
}


/* buffer_set_length ****************************************************************************//**
 * @brief		Preloads the length of the buffer and returns a pointer to the start of the buffer.
 * 				Returns null if the desired length is larger than the capacity of the buffer. */
inline void* buffer_set_length(Buffer* b, unsigned len)
{
	if(len <= buffer_size(b))
	{
		b->read  = b->start;
		b->write = b->start + len;
		return b->start;
	}
	else
	{
		return 0;
	}
}


/* buffer_top_parent ****************************************************************************//**
 * @brief		Returns the top-most parent buffer. */
inline Buffer* buffer_top_parent(const Buffer* b)
{
	while(buffer_parent(b))
	{
		b = buffer_parent(b);
	}

	return (Buffer*)b;
}


/* buffer_is_valid ******************************************************************************//**
 * @brief		Returns true if the buffer is valid. */
inline bool buffer_is_valid(const Buffer* b)
{
	return b && b->start && b->end && b->start <= b->read && b->read <= b->write;
}


/* buffer_offset ********************************************************************************//**
 * @brief		Returns a pointer to the byte at the specified offset from the start of the buffer if
 * 				the offset does not overflow the buffer. */
inline uint8_t* buffer_offset(const Buffer* b, unsigned offset)
{
	if(offset <= buffer_length(b))
	{
		return b->start + offset;
	}
	else
	{
		return 0;
	}
}


/* buffer_offsetof ******************************************************************************//**
 * @brief		Returns the offset of the specified pointer relative to the start of the buffer.
 * 				Returns an out of bounds offset if ptr < buffer_start(b). */
inline unsigned buffer_offsetof(const Buffer* b, const uint8_t* ptr)
{
	if(ptr < b->start)
	{
		return -1;
	}
	else
	{
		return ptr - b->start;
	}
}


/* buffer_write_set *****************************************************************************//**
 * @brief		Sets the buffer's write pointer.
 * @warning		The buffer's write pointer must be >= to the buffer's read pointer. The buffer's
 * 				write pointer will be left unmodified if this constraint is not met. */
inline bool buffer_write_set(Buffer* b, uint8_t* start)
{
	if(start && b->read <= start && start <= b->end)
	{
		b->write = start;
		return true;
	}
	else
	{
		return false;
	}
}


/* buffer_read_set ******************************************************************************//**
 * @brief		Sets the buffer's read pointer.
 * @warning		The buffer's read pointer must be <= to the buffer's write pointer. The buffer's read
 * 				pointer will be left unmodified if this constraint is not met. */
inline bool buffer_read_set(Buffer* b, uint8_t* start)
{
	if(start && b->start <= start && start <= b->end)
	{
		b->read = start;
		return true;
	}
	else
	{
		return false;
	}
}


/* buffer_write_seek ****************************************************************************//**
 * @brief		Sets the buffer's write pointer to the specified offset from the start of the
 * 				buffer.
 * @warning		The buffer's write pointer must be >= to the buffer's read pointer. The buffer's
 * 				write pointer will be left unmodified if this constraint is not met. */
inline bool buffer_write_seek(Buffer* b, unsigned offset)
{
	return buffer_write_set(b, buffer_offset(b, offset));
}


/* buffer_read_seek *****************************************************************************//**
 * @brief		Sets the buffer's read pointer to the specified offset from the start of the buffer.
 * @warning		The buffer's read pointer must be <= to the buffer's write pointer. The buffer's read
 * 				pointer will be left unmodified if this constraint is not met. */
inline bool buffer_read_seek(Buffer* b, unsigned offset)
{
	return buffer_read_set(b, buffer_offset(b, offset));
}


/* buffer_peek_at *******************************************************************************//**
 * @brief		Returns a pointer to len bytes at the specified pointer in the buffer. */
inline void* buffer_peek_at(const Buffer* b, const uint8_t* start, unsigned len)
{
	const uint8_t* end = start + len;

	if(!buffer_is_valid(b) || b->start > start || start > end || end > b->write)
	{
		return 0;
	}
	else
	{
		return (void*)start;
	}
}


/* buffer_read_at *******************************************************************************//**
 * @brief		Copies len bytes out of the buffer from the specified pointer. */
inline bool buffer_read_at(const Buffer* b, void* out, const uint8_t* start, unsigned len)
{
	void* ptr = buffer_peek_at(b, start, len);

	if(ptr)
	{
		memmove(out, ptr, len);
		return true;
	}
	else
	{
		return false;
	}
}


/* buffer_write_at ******************************************************************************//**
 * @brief		Inserts len bytes into the buffer at the specified pointer. Moves existing bytes
 * 				towards the end of the buffer. */
inline bool buffer_write_at(Buffer* b, const void* in, uint8_t* start, unsigned len)
{
	uint8_t* ptr = buffer_reserve_at(b, start, len);

	if(!ptr)
	{
		return false;
	}
	else if(in)
	{
		memmove(ptr, in, len);
	}

	return true;
}


/* buffer_reserve_at ****************************************************************************//**
 * @brief		Reserves len bytes at the specified pointer. Moves existing bytes towards the end of
 * 				the buffer. */
inline void* buffer_reserve_at(Buffer* b, uint8_t* start, unsigned len)
{
	if(!buffer_is_valid(b) || b->start > start || start > b->write || len > buffer_tailroom(b))
	{
		return 0;
	}

	memmove(start + len, start, buffer_top_parent(b)->write - start);
	memset (start, 0, len);

	b->write += len;

	/* Update pointers for parent buffers. Try and maintain the relative positions. Move the parent's
	 * pointers so that they are still pointing to the same data after reserving data in the child
	 * buffer. */
	for(b = b->parent; b != 0; b = b->parent)
	{
		if(b->write >= start) { b->write += len; }
		if(b->read  >= start) { b->read  += len; }
	}

	return start;
}


/* buffer_replace_at ****************************************************************************//**
 * @brief		Overwrites len bytes starting at the specified pointer. */
inline bool buffer_replace_at(Buffer* b, const void* in, uint8_t* start, unsigned len)
{
	uint8_t* end = start + len;

	if(!buffer_is_valid(b) || b->start > start || start > end || end > b->end)
	{
		return false;
	}

	if(in)
	{
		memmove(start, in, len);
	}
	else
	{
		memset(start, 0, len);
	}

	/* Update buffer's write index if overwriting more bytes than previously existed but there is
	 * enough room in the buffer to hold the overrun. */
	if(b->write < end)
	{
		b->write = end;
	}

	return true;
}





/* buffer_peek_offset ***************************************************************************//**
 * @brief		Returns a pointer to len bytes at the specified offset from the start of the
 * 				buffer. */
inline void* buffer_peek_offset(const Buffer* b, unsigned offset, unsigned len)
{
	return buffer_peek_at(b, buffer_offset(b, offset), len);
}


/* buffer_read_offset ***************************************************************************//**
 * @brief		Copies len bytes out of the buffer from the specified offset from the start of the
 * 				buffer. */
inline bool buffer_read_offset(const Buffer* b, void* out, unsigned offset, unsigned len)
{
	return buffer_read_at(b, out, buffer_offset(b, offset), len);
}


/* buffer_write_offset **************************************************************************//**
 * @brief		Inserts len bytes into the buffer at the specified offset from the start of the
 * 				buffer. Moves existing bytes towards the end of the buffer. */
inline bool buffer_write_offset(Buffer* b, const void* in, unsigned offset, unsigned len)
{
	return buffer_write_at(b, in, buffer_offset(b, offset), len);
}


/* buffer_reserve_offset ************************************************************************//**
 * @brief		Reserves len bytes at the specified offset from the start of the buffer. Moves
 * 				existing bytes towards the end of the buffer. */
inline void* buffer_reserve_offset(Buffer* b, unsigned offset, unsigned len)
{
	return buffer_reserve_at(b, buffer_offset(b, offset), len);
}


/* buffer_replace_at ****************************************************************************//**
 * @brief		Overwrites len bytes starting at the specified offset from the start of the
 * 				buffer. */
inline bool buffer_replace_offset(Buffer* b, const void* in, unsigned offset, unsigned len)
{
	return buffer_replace_at(b, in, buffer_offset(b, offset), len);
}





/* buffer_peek **********************************************************************************//**
 * @brief		Returns a pointer to the current read end of the buffer without removing len bytes
 * 				from the buffer. */
inline void* buffer_peek(const Buffer* b, unsigned len)
{
	return buffer_peek_at(b, b->read, len);
}


/* buffer_reserve *******************************************************************************//**
 * @brief		Reserves len bytes at the write end of the buffer. */
inline void* buffer_reserve(Buffer* b, unsigned len)
{
	return buffer_reserve_at(b, b->write, len);
}


/* buffer_pop ***********************************************************************************//**
 * @brief		Retrieves a pointer to the current read end of the buffer and removes len bytes from
 * 				the buffer. */
inline void* buffer_pop(Buffer* b, unsigned len)
{
	if(!buffer_is_valid(b) || len > buffer_remaining(b))
	{
		return 0;
	}
	else
	{
		void* ptr = b->read;
		b->read  += len;
		return ptr;
	}
}





// ----------------------------------------------------------------------------------------------- //
// Buffer Push                                                                                     //
// ----------------------------------------------------------------------------------------------- //
/* buffer_push_mem ******************************************************************************//**
 * @brief		Appends len bytes onto the end of the buffer. */
inline bool buffer_push_mem(Buffer* b, const void* in, unsigned len)
{
	return buffer_write_at(b, in, b->write, len);
}


/* buffer_push_string ***************************************************************************//**
 * @brief		Appends a string onto the end of the buffer. */
inline bool buffer_push_string(Buffer* b, String s)
{
	return buffer_push_mem(b, string_start(&s), string_length(&s));
}



// ----------------------------------------------------------------------------------------------- //
// Buffer Peek                                                                                     //
// ----------------------------------------------------------------------------------------------- //
/* buffer_peek_mem ******************************************************************************//**
 * @brief		Copies len bytes from the beginning of the buffer into the out parameter without
 * 				modifying the buffer. */
inline bool buffer_peek_mem(const Buffer* b, void* out, unsigned len)
{
	return buffer_read_at(b, out, b->read, len);
}


/* buffer_peek_string ***************************************************************************//**
 * @brief		Returns a string of the specified length from the beginning of the buffer without
 * 				modifying the buffer. */
inline String buffer_peek_string(const Buffer* b, unsigned len)
{
	return make_string_len(buffer_peek(b, len), len);
}





// ----------------------------------------------------------------------------------------------- //
// Buffer Pop                                                                                      //
// ----------------------------------------------------------------------------------------------- //
/* buffer_pop_mem *******************************************************************************//**
 * @brief		Removes len bytes from the beginning of the buffer and copies the bytes into the out
 * 				parameter. */
inline bool buffer_pop_mem(Buffer* b, void* out, unsigned len)
{
	void* ptr = buffer_pop(b, len);

	if(!ptr)
	{
		return false;
	}

	if(out)
	{
		memmove(out, ptr, len);
	}

	return true;
}


/* buffer_pop_string ****************************************************************************//**
 * @brief		Removes a string of the specified length from the buffer. Returns a string of 0
 * 				length if not enough bytes exist in the buffer. */
inline String buffer_pop_string(Buffer* b, unsigned len)
{
	return make_string_len(buffer_pop(b, len), len);
}





// ----------------------------------------------------------------------------------------------- //
// String                                                                                          //
// ----------------------------------------------------------------------------------------------- //
/* string_init **********************************************************************************//**
 * @brief		Initializes a string defined by the half open range [start, end). */
inline void string_init(String* s, const char* start, const char* end)
{
	if(!start || start > end)
	{
		s->start = 0;
		s->end   = 0;
	}
	else
	{
		s->start = start;
		s->end   = end;
	}
}


/* string_init_len ******************************************************************************//**
 * @brief		Initializes a string len bytes long and starting at the specified pointer. */
inline void string_init_len(String* s, const char* start, unsigned len)
{
	string_init(s, start, start + len);
}


/* make_string **********************************************************************************//**
 * @brief		Creates a string defined by the half open range [start, end). */
inline String make_string(const char* start, const char* end)
{
	if(!start || start > end)
	{
		return (String){ .start = 0, .end = 0 };
	}
	else
	{
		return (String){ .start = start, .end = end };
	}
}


/* make_string_len ******************************************************************************//**
 * @brief		Creates a string len bytes long and starting at the specified pointer. */
inline String make_string_len(const char* start, unsigned len)
{
	return make_string(start, start + len);
}


/* make_empty_string ****************************************************************************//**
 * @brief  		*/
inline String make_empty_string(void)
{
	return make_string(0, 0);
}


/* string_offset ********************************************************************************//**
 * @brief		Returns a pointer to the character at the specified offset from the start of the
 * 				string. Returns null if the offset is out of bounds. */
inline const char* string_offset(const String* s, unsigned o)
{
	if(o > string_length(s))
	{
		return 0;
	}
	else
	{
		return s->start + o;
	}
}


/* string_offsetof ******************************************************************************//**
 * @brief		Returns the offset of the specified pointer relative to the start of the string.
 * 				Returns an out of bounds offset if ptr < string_start(s). */
inline unsigned string_offsetof(const String* s, const char* ptr)
{
	if(ptr < s->start || ptr > s->end)
	{
		return -1;
	}
	else
	{
		return ptr - s->start;
	}
}


/* string_dup ***********************************************************************************//**
 * @brief		Copies the string into the specified destination buffer and returns a string pointing
 * 				to the destination. Zeros unused bytes in the destination. */
inline String string_dup(String s, void* dest, unsigned size)
{
	return make_string_len(dest, string_copy(s, dest, size));
}


/* compare_strings ******************************************************************************//**
 * @brief		Compares two strings given by pointers and returns their lexicographical order.
 * @return		>0 if a is lexicographically greater than b.
 *				 0 if the strings are equal.
 *				<0 is a is lexicographically less than b. */
inline int compare_strings(const void* a, const void* b)
{
	const String* s1 = a;
	const String* s2 = b;
	return string_cmp(*s1, *s2);
}


#ifdef __cplusplus
}
#endif

#endif // BUFFER_H
/******************************************* END OF FILE *******************************************/
