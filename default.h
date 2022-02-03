/************************************************************************************************//**
 * @file
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
 * @brief
 * @desc		Default print settings:
 *
 *					1. Courier New, 8 pt font.
 *					2. 4 spaces per tab. Keep tab character.
 *					3. 101 column limit.
 *					2. 1 inch left margin.
 *						a. 1/2 inches to line numbers.
 *						b. 1/2 inches to to left alignment of code block.
 *					3. 1/2 inch right margin.
 *
 *				Column ruler:
 *
 *				1  2         3         4         5         6         7         8         9         A
 *				7890123456789012345678901234567890123456789012345678901234567890123456789012345678901
 *
 *				Comment area ruler:
 *
 *				0         1         2         3         4         5         6         7         8
 *				0123456789012345678901234567890123456789012345678901234567890123456789012345678901234
 *
 *				Unicode Boxes
 *
 *				┌───┬───╥───┐   ╓───┬───╥───╖   ╒═══╤═══╦═══╕   ╔═══╤═══╦═══╗
 *				│   │   ║   │   ║   │   ║   ║   │   │   ║   │   ║   │   ║   ║
 *				├───┼───╫───┤   ╟───┼───╫───╢   ├───┼───╫───┤   ╟───┼───╫───╢
 *				│   │   ║   │   ║   │   ║   ║   │   │   ║   │   ║   │   ║   ║
 *				╞═══╪═══╬═══╡   ╠═══╪═══╬═══╣   ╞═══╪═══╬═══╡   ╠═══╪═══╬═══╣
 *				│   │   ║   │   ║   │   ║   ║   │   │   ║   │   ║   │   ║   ║
 *				└───┴───╨───┘   ╙───┴───╨───╜   ╘═══╧═══╩═══╛   ╚═══╧═══╩═══╝
 *
 ***************************************************************************************************/
#ifndef DEFAULT_H
#define DEFAULT_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
/* Public Macros --------------------------------------------------------------------------------- */
/* Public Types ---------------------------------------------------------------------------------- */
/* Public Functions ------------------------------------------------------------------------------ */


// ----------------------------------------------------------------------------------------------- //
// Section Separator 1                                                                             //
// ----------------------------------------------------------------------------------------------- //

// =============================================================================================== //
// Section Separator 2                                                                             //
// =============================================================================================== //


#ifdef __cplusplus
}
#endif

#endif // DEFAULT_H
/******************************************* END OF FILE *******************************************/
