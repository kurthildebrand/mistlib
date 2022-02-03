/************************************************************************************************//**
 * @file		utils.h
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
 * @ref			https://groups.google.com/forum/#!topic/comp.std.c/d-6Mj5Lko_s
 *
 ***************************************************************************************************/
#ifndef UTILS_H
#define UTILS_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* offsetof *************************************************************************************//**
 * @brief		Computes the offset in bytes of a given member within a struct or union. */
#if !defined(offsetof)
#define offsetof(type, member) ((size_t) &((type*)0)->member)
#endif


/* CONTAINER_OF *********************************************************************************//**
 * @brief		Computes the address of the containing structure given a pointer to a member. */
#if !defined(CONTAINER_OF)
#define CONTAINER_OF(ptr, type, member) ((type*)(((char*)(ptr)) - offsetof(type, member)))
#endif


/* APPEND_NARGS *********************************************************************************//**
 * @brief		Concatenates a string with the number of arguments passed to the variadic macro. The
 *				format is base ## N or baseN where N is the number of arguments. Example:
 *
 *					#define EXPAND(...)	APPEND_NARGS(EXPANDED, __VA_ARGS__)
 *					EXPAND(a)		EXPANDED1(a)
 *					EXPAND(a,b)		EXPANDED2(a,b)
 *					EXPAND(a,b,c)	EXPANDED3(a,b,c)
 *					EXPAND(a,b,c,d)	EXPANDED4(a,b,c,d)
 *
 *				Usage: initializing a list at compile time.
 *
 *					#define MAKE_LIST(...) \
 *						APPEND_NARGS(MAKE_LIST, __VA_ARGS__)
 *					#define MAKE_LIST3(d,s,e) \
 *						(List){ .entries=(d),.elemsize=(e),.size=(s),.count=(s) }
 *					#define MAKE_LIST4(k,d,s,e) \
 *						(List){ .key=(k),.entries=(d),.elemsize=(e),.size=(s),.count=(s) }
 *
 *					int data[10];
 *					List list = MAKE_LIST(data, 10, sizeof(int)); // which expands to
 *					List list = MAKE_LIST3(data, 10, sizeof(int));
 */
#define APPEND_NARGS(base, ...) \
	APPEND_NARGS_(base, COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)
#define APPEND_NARGS_(base, count, ...) \
	APPEND_NARGS__(base, count, __VA_ARGS__)
#define APPEND_NARGS__(base, count, ...) \
	base ## count(__VA_ARGS__)


/* COUNT_ARGS ***********************************************************************************//**
 * @brief		Counts the number of arguments passed into a variadic macro up to a maximum of 64
 *				arguments. Does not work for macros with 0 arguments. Usage:
 *
 *					printf("%d", COUNT_ARGS(a, b, c, d));
 *
 *					CIO: 4
 */
#define COUNT_ARGS(...) \
    COUNT_ARGS_(__VA_ARGS__, RSEQ_N())
#define COUNT_ARGS_(...) \
    ARG_N(__VA_ARGS__)
#define ARG_N( \
     _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
    _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
    _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
    _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
    _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
    _61,_62,_63,  N, ...) N
#define RSEQ_N() \
    63,62,61,60,                   \
    59,58,57,56,55,54,53,52,51,50, \
    49,48,47,46,45,44,43,42,41,40, \
    39,38,37,36,35,34,33,32,31,30, \
    29,28,27,26,25,24,23,22,21,20, \
    19,18,17,16,15,14,13,12,11,10, \
     9, 8, 7, 6, 5, 4, 3, 2, 1, 0


#ifdef __cplusplus
}
#endif

#endif // UTILS_H
/******************************************* END OF FILE *******************************************/
