/************************************************************************************************//**
 * @file		json.c
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

#include "json.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern String   json_key  (const JsonToken*);
extern String   json_value(const JsonToken*);
extern JsonType json_type (const JsonToken*);


/* Private Types --------------------------------------------------------------------------------- */
typedef enum {
	JSON_STRING_INIT,
	JSON_STRING_QUOTED,
	JSON_STRING_UNQUOTED,
	JSON_STRING_IGNORE,
} JsonState;


/* Private Functions ----------------------------------------------------------------------------- */
static String json_read_string         (Buffer*);
//static String json_read_quoted_string  (Buffer*);
//static String json_read_unquoted_string(Buffer*);


/* json_init ************************************************************************************//**
 * @brief		*/
void json_init(Buffer* b, JsonToken* base)
{
	const char* ptr;
	base->key    = make_string(0, 0);
	base->value  = make_string(0, 0);

	/* Read until either '{' or '[' */
	while((ptr = buffer_pop(b, 1)))
	{
		if(*ptr == '{' || *ptr == '[')
		{
			base->value_type = *ptr;
			string_init(&base->value, ptr, ptr);
			break;
		}
	}
}


/* json_read ************************************************************************************//**
 * @brief		*/
bool json_read(Buffer* b, JsonToken* parent, JsonToken* token)
{
	/* Detect if the final field has been parsed. Parent's length is set once the final object is
	 * parsed and 0 otherwise. Therefore, done parsing parent if it's length is nonzero. */
	if(string_length(&parent->value) > 0)
	{
		return false;
	}

	const char* ptr;

	/* Find the next key or the end of the object/array */
	while((ptr = buffer_peek(b, 1)))
	{
		if(*ptr == '"')
		{
			break;
		}

		buffer_pop(b, 1);

		if(*ptr == '}' || *ptr == ']')
		{
			parent->value.end = ptr + 1;
			return false;
		}
	}

	if(parent->value_type == JSON_OBJECT)
	{
		/* Read key */
		token->key = json_read_string(b);

		/* Find ':' */
		while((ptr = buffer_pop(b, 1)) && *ptr != ':') { }
	}

	/* Read value */
	while((ptr = buffer_peek(b, 1)))
	{
		if(*ptr == '{' || *ptr == '[')
		{
			token->value_type = *ptr;
			token->value = make_string(ptr, ptr);
			return true;
		}
		else if(!isspace(*ptr))
		{
			token->value_type = JSON_STRING;
			token->value = json_read_string(b);
			return true;
		}

		buffer_pop(b, 1);
	}

	return false;
}


/* json_read_string *****************************************************************************//**
 * @brief			Tokenizes a string from the underlying stream. The string to be read may either
 * 					be quoted or unquoted. For example:
 *
 * 						              buffer read       buffer read
 * 						              |                 |
 * 						              v                 v
 * 						Quoted: "bold",     Unquoted: 36,
 * 						         ^   ^                ^ ^
 * 						         |   |                | |
 * 						         |   end              | end
 * 						         start                 start
 *
 * 					Returns a string representing the actual value (quotes removed if applicable). */
static String json_read_string(Buffer* b)
{
	String    s;
	JsonState state = JSON_STRING_INIT;
	const char* ptr;

	while((ptr = buffer_peek(b, 1)))
	{
		switch(state) {
		case JSON_STRING_INIT:
			if(*ptr == '}' || *ptr == ']')
			{
				goto exit;
			}
			else if(*ptr == '"')
			{
				state   = JSON_STRING_QUOTED;
				s.start = ptr + 1;
			}
			else if(*ptr != '{' && *ptr != '[' && *ptr != ',' && !isspace(*ptr))
			{
				state   = JSON_STRING_UNQUOTED;
				s.start = ptr;
			}
			break;

		case JSON_STRING_QUOTED:
			if(*ptr == '\\')
			{
				state = JSON_STRING_IGNORE;
			}
			else if(*ptr == '"')
			{
				s.end = ptr;
				buffer_pop_i8(b);	/* Remove '"' */
				return s;
			}
			break;

		case JSON_STRING_UNQUOTED:
			if(*ptr == ',' || *ptr == '}' || *ptr == ']')
			{
				s.end = ptr;
				return s;
			}
			break;

		case JSON_STRING_IGNORE:
			state = JSON_STRING_QUOTED;
			break;
		}

		buffer_pop_i8(b);
	}

	exit:
		s.start = 0;
		s.end   = 0;
		return s;
}


// /* json_read_string *****************************************************************************//**
//  * @brief			Tokenizes a string from the underlying stream. The string to be read may either
//  * 					be quoted or unquoted. For example:
//  *
//  * 						              buffer read       buffer read
//  * 						              |                 |
//  * 						              v                 v
//  * 						Quoted: "bold",     Unquoted: 36,
//  * 						         ^   ^                ^ ^
//  * 						         |   |                | |
//  * 						         |   end              | end
//  * 						         start                 start
//  *
//  * 					Returns a string representing the actual value (quotes removed if applicable). */
// static String json_read_string(Buffer* b)
// {
// 	const char* ptr;

// 	for(ptr = buffer_peek(b, 1); ptr; ptr = buffer_peek(b, 1))
// 	{
// 		if(*ptr == '}' || *ptr == ']')
// 		{
// 			break;
// 		}
// 		else if(*ptr == '"')
// 		{
// 			return json_read_quoted_string(b);
// 		}
// 		else if(*ptr != '{' && *ptr != '[' && *ptr != ',' && !isspace(*ptr))
// 		{
// 			return json_read_unquoted_string(b);
// 		}

// 		buffer_pop_i8(b);
// 	}

// 	return make_string(0, 0);
// }


// /* json_read_quoted_string **********************************************************************//**
//  * @brief		Tokenizes a quoted string.
//  * @warning		Expects the calling function to check that the string starts with a quote (") mark.
//  *
//  *					              buffer read
//  *					              |
//  *					              v
//  *					Quoted: "bold",
//  *					         ^   ^
//  *					         |   |
//  *					         |   end
//  *					         start
//  */
// static String json_read_quoted_string(Buffer* b)
// {
// 	String s;
// 	const char* ptr;

// 	buffer_pop_i8(b);

// 	for(s.start = ptr = buffer_peek(b, 1); ptr; ptr = buffer_peek(b, 1))
// 	{
// 		if(*ptr == '\\')
// 		{
// 			buffer_pop_i8(b);	/* Remove '\' */
// 		}
// 		else if(*ptr == '"')
// 		{
// 			s.end = ptr;
// 			buffer_pop_i8(b);	/* Remove '"' */
// 			return s;
// 		}

// 		buffer_pop(b, 1);
// 	}

// 	s.start = 0;
// 	s.end   = 0;
// 	return s;
// }


// /* json_read_unquoted_string ********************************************************************//**
//  * @brief		Tokenizes an unquoted string.
//  * @warning		Expects the calling function to check that the string does not start with a quote (")
//  * 				mark.
//  *
//  * 					            buffer read      buffer read
//  * 					            |                |
//  * 					            v                v
//  * 					Unquoted: 36,            1234}
//  * 					          ^ ^            ^   ^
//  * 					          | |            |   |
//  * 					          | end          |   end
//  * 					          start          start
//  *
//  */
// static String json_read_unquoted_string(Buffer* b)
// {
// 	String s;
// 	const char* ptr;

// 	for(s.start = ptr = buffer_peek(b, 1); ptr; ptr = buffer_peek(b, 1))
// 	{
// 		if(*ptr == ',' || *ptr == '}' || *ptr == ']')
// 		{
// 			s.end = ptr;
// 			return s;
// 		}

// 		buffer_pop(b, 1);
// 	}

// 	s.start = 0;
// 	s.end   = 0;
// 	return s;
// }


/******************************************* END OF FILE *******************************************/
