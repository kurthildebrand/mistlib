/************************************************************************************************//**
 * @file		buffer.c
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
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

#include "buffer.h"
#include "byteorder.h"
#include "calc.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern void*       buffer_init          (Buffer*, void*, unsigned, unsigned);
extern void*       buffer_slice         (Buffer*, const Buffer*, uint8_t*, unsigned);
extern Buffer      make_buffer          (void*, unsigned, unsigned);
extern Buffer      make_buffer_slice    (const Buffer*, uint8_t*, unsigned);

extern void        buffer_clear         (Buffer*);
extern void*       buffer_set_length    (Buffer*, unsigned);
extern Buffer*     buffer_parent        (const Buffer*);
extern Buffer*     buffer_top_parent    (const Buffer*);
extern bool        buffer_is_valid      (const Buffer*);

extern unsigned    buffer_size          (const Buffer*);
extern unsigned    buffer_length        (const Buffer*);
extern unsigned    buffer_remaining     (const Buffer*);
extern unsigned    buffer_free          (const Buffer*);
extern unsigned    buffer_tailroom      (const Buffer*);
extern uint8_t*    buffer_start         (const Buffer*);
extern uint8_t*    buffer_end           (const Buffer*);
extern uint8_t*    buffer_read          (const Buffer*);
extern uint8_t*    buffer_write         (const Buffer*);
extern uint8_t*    buffer_offset        (const Buffer*, unsigned);
extern unsigned    buffer_offsetof      (const Buffer*, const uint8_t*);
extern bool        buffer_write_set     (Buffer*, uint8_t*);
extern bool        buffer_read_set      (Buffer*, uint8_t*);
extern bool        buffer_write_seek    (Buffer*, unsigned);
extern bool        buffer_read_seek     (Buffer*, unsigned);


extern void*       buffer_peek_at       (const Buffer*, const uint8_t*, unsigned);
extern bool        buffer_read_at       (const Buffer*, void*, const uint8_t*, unsigned);
extern bool        buffer_write_at      (Buffer*, const void*, uint8_t*, unsigned);
extern void*       buffer_reserve_at    (Buffer*, uint8_t*, unsigned);
extern bool        buffer_replace_at    (Buffer*, const void*, uint8_t*, unsigned);

extern void*       buffer_peek_offset   (const Buffer*, unsigned, unsigned);
extern bool        buffer_read_offset   (const Buffer*, void*, unsigned, unsigned);
extern bool        buffer_write_offset  (Buffer*, const void*, unsigned, unsigned);
extern void*       buffer_reserve_offset(Buffer*, unsigned, unsigned);
extern bool        buffer_replace_offset(Buffer*, const void*, unsigned, unsigned);

extern void*       buffer_peek          (const Buffer*, unsigned);
extern void*       buffer_reserve       (Buffer*, unsigned);
extern void*       buffer_pop           (Buffer*, unsigned);


extern bool        buffer_push_mem      (Buffer*, const void*, unsigned);
extern bool        buffer_push_printf   (Buffer*, const char*, ...);
extern bool        buffer_push_string   (Buffer*, String  );
extern bool        buffer_push_u8       (Buffer*, uint8_t );
extern bool        buffer_push_u16      (Buffer*, uint16_t);
extern bool        buffer_push_u32      (Buffer*, uint32_t);
extern bool        buffer_push_u64      (Buffer*, uint64_t);
extern bool        buffer_push_i8       (Buffer*, int8_t  );
extern bool        buffer_push_i16      (Buffer*, int16_t );
extern bool        buffer_push_i32      (Buffer*, int32_t );
extern bool        buffer_push_i64      (Buffer*, int64_t );


extern bool        buffer_peek_mem      (const Buffer*, void*, unsigned);
extern String      buffer_peek_string   (const Buffer*, unsigned);
extern void*       buffer_peek_u8       (const Buffer*);
extern void*       buffer_peek_u16      (const Buffer*);
extern void*       buffer_peek_u32      (const Buffer*);
extern void*       buffer_peek_u64      (const Buffer*);
extern void*       buffer_peek_i8       (const Buffer*);
extern void*       buffer_peek_i16      (const Buffer*);
extern void*       buffer_peek_i32      (const Buffer*);
extern void*       buffer_peek_i64      (const Buffer*);


extern bool        buffer_pop_mem       (Buffer*, void*, unsigned);
extern String      buffer_pop_string    (Buffer*, unsigned);
extern void*       buffer_pop_u8        (Buffer*);
extern void*       buffer_pop_u16       (Buffer*);
extern void*       buffer_pop_u32       (Buffer*);
extern void*       buffer_pop_u64       (Buffer*);
extern void*       buffer_pop_i8        (Buffer*);
extern void*       buffer_pop_i16       (Buffer*);
extern void*       buffer_pop_i32       (Buffer*);
extern void*       buffer_pop_i64       (Buffer*);





/* String ---------------------------------------------------------------------------------------- */
extern void        string_init          (String*, const char*, const char*);
extern void        string_init_len      (String*, const char*, unsigned);
extern String      make_string          (const char*, const char*);
extern String      make_string_len      (const char*, unsigned);
extern String      make_empty_string    (void);
extern const char* string_start         (const String*);
extern const char* string_end           (const String*);
extern const char* string_offset        (const String*, unsigned);
extern unsigned    string_offsetof      (const String*, const char*);
extern unsigned    string_length        (const String*);
extern bool        string_empty         (const String*);

extern String      string_dup           (String, void*, unsigned);
extern int         compare_strings      (const void*, const void*);
extern bool        string_equal         (String, String);


/* Private Functions ----------------------------------------------------------------------------- */
static const char* string_break         (const String*, const char*);


// ----------------------------------------------------------------------------------------------- //
// Buffer                                                                                          //
// ----------------------------------------------------------------------------------------------- //
/* buffer_push_printf ***************************************************************************//**
 * @brief  		 */
bool buffer_push_printf(Buffer* b, const char* str, ...)
{
	va_list args;
	va_start(args, str);

	if(b && b->write)
	{
		unsigned written = vsnprintf((char*)buffer_write(b), buffer_remaining(b), str, args);
		// unsigned written = snprintf((char*)buffer_write(b), buffer_remaining(b), "Test");
		// unsigned written = printf("This is a test");

		if(written > 0)
		{
			/* Strip null character */
			b->write += written - 1;
		}
	}

	va_end(args);
	return true;
}


// ----------------------------------------------------------------------------------------------- //
// String                                                                                          //
// ----------------------------------------------------------------------------------------------- //
/* string_copy **********************************************************************************//**
 * @brief		Copies the string into the specified destination buffer. Zeros unused bytes in the
 * 				destination.
 * @param[in]	s: the string to copy from.
 * @param[in]	dest: the buffer to copy to.
 * @param[in]	size: the size of the destination buffer in bytes.
 * @return		The number of bytes copied. */
unsigned string_copy(String s, void* dest, unsigned size)
{
	if(!dest)
	{
		return 0;
	}

	/* Note: expects string_length to return the number of bytes in the string. */
	unsigned len = calc_min_uint(string_length(&s), size);
	char*    ptr = dest;

	/* Todo: search backward and zero multibyte character in order to handle utf8. */
	memmove(&ptr[0], string_offset(&s, 0), len);
	memset (&ptr[len], 0, size - len);

	return len;
}


/* string_token *********************************************************************************//**
 * @brief		Separates a string into tokens.
 * @desc		If the string iterator points to a null character then string_token returns an empty
 * 				string. Otherwise, string_token finds the first token from the iterator that is
 * 				delimited by one of the characters in the null terminiated string delim. The iterator
 * 				is updated to point past the token. In case no delimiter was found the token is taken
 * 				to be the entire string.
 * @param[in]	it: iterator to store the context of the search.
 * @param[in]	delim: token delimiters.
 * @return		Returns the token. */
String string_token(String* it, const char* delim)
{
	String token;

	const char* brk = string_break(it, delim);
	string_init(&token, string_start(it), brk);
	string_init(it,     brk + 1, string_end(it));

	return token;
}


/* string_cmp ***********************************************************************************//**
 * @brief		Compares two strings to find their lexicographical ordering.
 * @param[in]	s1: first string to compare
 * @param[in]	s2: second string to compare
 * @return		>0 if s1 is lexicographically greater than s2.
 *				 0 if the strings are equal.
 *				<0 is s1 is lexicographically less than s2. */
int string_cmp(String s1, String s2)
{
	unsigned i;

	for(i = 0; i < string_length(&s1) && i < string_length(&s2); i++)
	{
		char a   = *string_offset(&s1, i);
		char b   = *string_offset(&s2, i);
		int  cmp = a - b;

		if(cmp != 0)
		{
			return cmp;
		}
	}

	return string_length(&s1) - string_length(&s2);
}


/* string_casecmp *******************************************************************************//**
 * @brief		Performs a case-insensitive string comparison for the first len characters in string
 * 				s1 and s2.
 * @retval		>0 if s1 is lexicographically greater than s2.
 *				 0 if the strings are equal.
 *				<0 is s1 is lexicographically less than s2. */
int string_casecmp(String s1, String s2)
{
	unsigned i;

	for(i = 0; i < string_length(&s1) && i < string_length(&s2); i++)
	{
		char a   = tolower(*string_offset(&s1, i));
		char b   = tolower(*string_offset(&s2, i));
		int  cmp = a - b;

		if(cmp != 0)
		{
			return cmp;
		}
	}

	return string_length(&s1) - string_length(&s2);
}


/* string_search ********************************************************************************//**
 * @brief		Searches a given string (haystack) for the specified substring (needle). Returns a
 * 				string representing the first occurance of needle in haystack. Returns an empty
 * 				string if the needle was not found. */
String string_search(String haystack, String needle)
{
	String substring;

	if(0 < string_length(&needle) && string_length(&needle) <= string_length(&haystack))
	{
		string_init_len(&substring, string_start(&haystack), string_length(&needle));

		while(substring.start && substring.end && substring.end <= haystack.end)
		{
			if(string_cmp(substring, needle) == 0)
			{
				return substring;
			}

			substring.start++;
			substring.end++;
		}
	}

	substring.start = 0;
	substring.end   = 0;
	return substring;
}


/* string_break *********************************************************************************//**
 * @brief		Returns the index of the next delimiter in the string. If no delimiter was found,
 * 				returns the length of the string.
 * @param[in]	s: the string to search for delimiters.
 * @param[in]	delim: null terminated string of delimiters to search for.
 * @return		The index of the first instance of a character in delim. Returns the length of the
 * 				string if no delimiters were found. */
static const char* string_break(const String* s, const char* delim)
{
	const char* i;
	const char* j;

	/* Compare every character in string s with every delimiter character until the first match or
	 * the end of the string is reached. */
	for(i = string_start(s); i < string_end(s); i++)
	{
		for(j = delim; *j != 0; j++)
		{
			if(*i == *j)
			{
				return i;
			}
		}
	}

	return i;
}


/******************************************* END OF FILE *******************************************/
