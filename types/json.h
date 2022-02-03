/************************************************************************************************//**
 * @file		json.h
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
#ifndef JSON_H
#define JSON_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include "buffer.h"


/* Public Types ---------------------------------------------------------------------------------- */
typedef enum {
	JSON_OBJECT = '{',
	JSON_ARRAY  = '[',
	JSON_STRING = '"',
} JsonType;

typedef struct JsonToken {
	JsonType value_type;
	String   key;
	String   value;
} JsonToken;


/* Public Functions ------------------------------------------------------------------------------ */
       void     json_init (Buffer*, JsonToken*);
inline String   json_key  (const JsonToken* t) { return t->key;        }
inline String   json_value(const JsonToken* t) { return t->value;      }
inline JsonType json_type (const JsonToken* t) { return t->value_type; }
       bool     json_read (Buffer*, JsonToken*, JsonToken*);

inline bool     json_write_init_obj  (Buffer* b) { return buffer_push_printf(b, "{"); }
inline bool     json_write_init_array(Buffer* b) { return buffer_push_printf(b, "["); }

inline bool     json_obj_write       (Buffer* b, const char* key);
inline bool     json_obj_write_end   (Buffer* b);
inline bool     json_obj_write_int   (Buffer* b, const char* key, int      value);
inline bool     json_obj_write_i64   (Buffer* b, const char* key, int64_t  value);
inline bool     json_obj_write_uint  (Buffer* b, const char* key, unsigned value);
inline bool     json_obj_write_u64   (Buffer* b, const char* key, uint64_t value);
inline bool     json_obj_write_f     (Buffer* b, const char* key, float    value);
inline bool     json_obj_write_d     (Buffer* b, const char* key, double   value);
inline bool     json_obj_write_bool  (Buffer* b, const char* key, bool     value);
inline bool     json_obj_write_null  (Buffer* b, const char* key);
inline bool     json_obj_write_str   (Buffer* b, const char* key, String   value);

inline bool     json_array_write     (Buffer* b, const char* key);
inline bool     json_array_write_end (Buffer* b);
inline bool     json_array_write_int (Buffer* b, int      value);
inline bool     json_array_write_i64 (Buffer* b, int64_t  value);
inline bool     json_array_write_uint(Buffer* b, unsigned value);
inline bool     json_array_write_u64 (Buffer* b, uint64_t value);
inline bool     json_array_write_f   (Buffer* b, float    value);
inline bool     json_array_write_d   (Buffer* b, double   value);
inline bool     json_array_write_bool(Buffer* b, bool     value);
inline bool     json_array_write_null(Buffer* b);
inline bool     json_array_write_str (Buffer* b, String   value);





// ----------------------------------------------------------------------------------------------- //
// JSON Object Fields "key":"value",                                                               //
// ----------------------------------------------------------------------------------------------- //
inline bool json_obj_write(Buffer* b, const char* key)
{
	return buffer_push_printf(b, "\"%s\":{", key);
}

inline bool json_obj_write_end(Buffer* b)
{
	/* Remove trailing comma */
	uint8_t* last = buffer_peek_at(b, buffer_write(b) - 1, 1);

	if(!last)
	{
		return false;
	}
	else if(*last == ',')
	{
		buffer_write_set(b, last);
	}

	return buffer_push_printf(b, "},");
}

inline bool json_obj_write_int(Buffer* b, const char* key, int value)
{
	return buffer_push_printf(b, "\"%s\":%d,", key, value);
}

inline bool json_obj_write_i64(Buffer* b, const char* key, int64_t value)
{
	return buffer_push_printf(b, "\"%s\":%lld,", key, value);
}

inline bool json_obj_write_uint(Buffer* b, const char* key, unsigned value)
{
	return buffer_push_printf(b, "\"%s\":%u,", key, value);
}

inline bool json_obj_write_u64(Buffer* b, const char* key, uint64_t value)
{
	return buffer_push_printf(b, "\"%s\":%llu,", key, value);
}

inline bool json_obj_write_f(Buffer* b, const char* key, float value)
{
	return buffer_push_printf(b, "\"%s\":%f,", key, value);
}

inline bool json_obj_write_d(Buffer* b, const char* key, double value)
{
	return buffer_push_printf(b, "\"%s\":%f,", key, value);
}

inline bool json_obj_write_bool(Buffer* b, const char* key, bool value)
{
	return buffer_push_printf(b, "\"%s\":%s,", key, value ? "true" : "false");
}

inline bool json_obj_write_null(Buffer* b, const char* key)
{
	return buffer_push_printf(b, "\"%s\":null,", key);
}

inline bool json_obj_write_str(Buffer* b, const char* key, String value)
{
	return buffer_push_printf(b, "\"%s\":\"%.*s\",", key,
		string_length(&value), string_start(&value));
}





// ----------------------------------------------------------------------------------------------- //
// JSON Object Fields "key":"value",                                                               //
// ----------------------------------------------------------------------------------------------- //
inline bool json_array_write(Buffer* b, const char* key)
{
	return buffer_push_printf(b, "\"%s\":[", key);
}

inline bool json_array_write_end(Buffer* b)
{
	/* Remove trailing comma */
	uint8_t* last = buffer_peek_at(b, buffer_write(b) - 1, 1);

	if(*last == ',')
	{
		buffer_write_set(b, last);
	}

	return buffer_push_printf(b, "],");
}

inline bool json_array_write_int(Buffer* b, int value)
{
	return buffer_push_printf(b, "%d,", value);
}

inline bool json_array_write_i64(Buffer* b, int64_t value)
{
	return buffer_push_printf(b, "%lld,", value);
}

inline bool json_array_write_uint(Buffer* b, unsigned value)
{
	return buffer_push_printf(b, "%u,", value);
}

inline bool json_array_write_u64(Buffer* b, uint64_t value)
{
	return buffer_push_printf(b, "%llu,", value);
}

inline bool json_array_write_f(Buffer* b, float value)
{
	return buffer_push_printf(b, "%f,", value);
}

inline bool json_array_write_d(Buffer* b, double value)
{
	return buffer_push_printf(b, "%f,", value);
}

inline bool json_array_write_bool(Buffer* b, bool value)
{
	return buffer_push_printf(b, "%s,", value ? "true" : "false");
}

inline bool json_array_write_null(Buffer* b)
{
	return buffer_push_printf(b, "null,");
}

inline bool json_array_write_str(Buffer* b, String value)
{
	return buffer_push_printf(b, "\"%.*s\",", string_length(&value), string_start(&value));
}


#ifdef __cplusplus
}
#endif

#endif // JSON_H
/******************************************* END OF FILE *******************************************/
