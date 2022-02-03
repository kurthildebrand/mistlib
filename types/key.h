/************************************************************************************************//**
 * @file		key.h
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
#ifndef KEY_H
#define KEY_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <stdint.h>
#include <string.h>


/* Public Types ---------------------------------------------------------------------------------- */
typedef int Key;


/* Public Functions ------------------------------------------------------------------------------ */
/* key_get **************************************************************************************//**
 * @brief		Returns the key of some object.
 * @warning		Call this function only on objects which contains a Key as the first member of the
 *				struct. */
inline Key key_get(const void* obj)
{
	Key value;

	memmove(&value, obj, sizeof(Key));

	return value;
}


/* key_set *********************************************************************************//**
 * @brief		Sets the key of some object.
 * @warning		Call this function only on objects which contains a Key as the first member of the
 *				struct. */
inline void* key_set(void* obj, Key key)
{
	if(obj)
	{
		memmove(obj, &key, sizeof(Key));
	}

	return obj;
}


#ifdef __cplusplus
}
#endif

#endif // KEY_H
/******************************************* END OF FILE *******************************************/
