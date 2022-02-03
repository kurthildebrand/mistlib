/************************************************************************************************//**
 * @file		order.h
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
#ifndef ORDER_H
#define ORDER_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <stdbool.h>

#include "compare.h"
#include "range.h"


/* Public Functions ------------------------------------------------------------------------------ */
bool     ascending   (const Range*, ICompare);
bool     descending  (const Range*, ICompare);
bool     equivalent  (const Range*, const Range*, ICompare);
void     reverse     (Range*);
void     shuffle     (Range*);
unsigned rotate_left (Range*, unsigned);
unsigned rotate_right(Range*, unsigned);


#ifdef __cplusplus
}
#endif

#endif // ORDER_H
/******************************************* END OF FILE *******************************************/
