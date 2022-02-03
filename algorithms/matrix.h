/************************************************************************************************//**
 * @file		matrix.h
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
#ifndef MATRIX_H
#define MATRIX_H

#if __STDC_VERSION__ < 19901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "calc.h"


/* Public Macros --------------------------------------------------------------------------------- */
#define MATRIX_TOLERANCE	(1E-6)

/* 33222222222211111111110000000000
 * 10987654321098765432109876543210
 *         ccccccccbbbbbbbbaaaaaaaa
 * bit  0:   transpose
 * bits 1-2: trangular
 * bit  3:   unitriangular */
#define MATRIX_A_TRANS			(0x1 << 0)
#define MATRIX_A_TRIU			(0x1 << 1)	/* Todo */
#define MATRIX_A_TRIL			(0x2 << 1)	/* Todo */
#define MATRIX_A_UNITRIANGULAR	(0x1 << 3)	/* Todo */

#define MATRIX_B_TRANS			(0x1 << 8)
#define MATRIX_B_TRIU			(0x1 << 9)	/* Todo */
#define MATRIX_B_TRIL			(0x2 << 9)	/* Todo */
#define MATRIX_B_UNITRIANGULAR	(0x1 << 11)	/* Todo */

#define MATRIX_C_TRANS			(0x1 << 16)
#define MATRIX_C_TRIU			(0x1 << 17)	/* Todo */
#define MATRIX_C_TRIL			(0x2 << 17)	/* Todo */
#define MATRIX_C_UNITRIANGULAR	(0x1 << 19)	/* Todo */


/* Public Types ---------------------------------------------------------------------------------- */
typedef struct {
	float*   data;
	unsigned numcols;
	uint16_t startrow;
	uint16_t endrow;
	uint16_t startcol;
	uint16_t endcol;
} Matrix;

typedef struct {
	float x,y,z;
} Vec3;


/* Public Functions ------------------------------------------------------------------------------ */
inline Vec3     make_vec3     (float x, float y, float z)          { return (Vec3){ x, y, z };     }
inline void     vec3_set      (Vec3* v, float x, float y, float z) { v->x = x, v->y = y, v->z = z; }
inline bool     vec3_is_finite(const Vec3*);

inline float    vec3_dot  (Vec3 a, Vec3 b)            { return a.x*b.x + a.y*b.y + a.z*b.z;         }
inline Vec3     vec3_cross(Vec3, Vec3);
inline float    vec3_dist (Vec3 a, Vec3 b);
inline float    vec3_norm2(Vec3 a)                    { return a.x*a.x + a.y*a.y + a.z*a.z;         }
inline float    vec3_norm (Vec3 a)                    { return sqrtf(vec3_norm2(a));                }
inline Vec3     vec3_add  (Vec3 a, Vec3 b)            { return (Vec3){ a.x+b.x, a.y+b.y, a.z+b.z }; }
inline Vec3     vec3_sub  (Vec3 a, Vec3 b)            { return (Vec3){ a.x-b.x, a.y-b.y, a.z-b.z }; }
inline Vec3     vec3_scale(Vec3 a, float s)           { return (Vec3){ a.x*s, a.y*s, a.z*s };       }
inline Vec3     vec3_unit (Vec3 a) { float m=vec3_norm(a); return vec3_scale(a, !m ? 0.0f:1.0f/m);  }

inline void     mat_init  (Matrix*, unsigned r, unsigned c, float[r][c]);
inline void     mat_slice (Matrix*, const Matrix*, uint16_t, uint16_t, uint16_t, uint16_t);
inline unsigned mat_rows  (const Matrix* m) { return m->endrow - m->startrow; }
inline unsigned mat_cols  (const Matrix* m) { return m->endcol - m->startcol; }
inline void     mat_zero  (Matrix*);
       void     mat_ident (Matrix*);
       bool     mat_copy  (Matrix*, const Matrix*);

       float    mat_norm2     (const Matrix*);
       bool     mat_mult      (Matrix*, float, const Matrix*, const Matrix*, float, unsigned);
       void     mat_qr        (Matrix*, float*);
       void     mat_mult_qt   (Matrix*, Matrix*, float*);
       void     mat_qr_backsub(Matrix*, Matrix*);


// ----------------------------------------------------------------------------------------------- //
// Vec3                                                                                            //
// ----------------------------------------------------------------------------------------------- //
/* vec3_is_finite *******************************************************************************//**
 * @brief		Returns true if the vector is finite. */
inline bool vec3_is_finite(const Vec3* v)
{
	return isfinite(v->x) && isfinite(v->y) && isfinite(v->z);
}


/* vec3_cross ***********************************************************************************//**
 * @brief		Computes the cross product of two vectors a x b. */
inline Vec3 vec3_cross(Vec3 a, Vec3 b)
{
	return (Vec3){
		.x = calc_dop_f(a.y, b.z, a.z, b.y),
		.y = calc_dop_f(a.z, b.x, a.x, b.z),
		.z = calc_dop_f(a.x, b.y, a.y, b.x),
	};
}


/* vec3_dist ************************************************************************************//**
 * @brief		Computes the euclidean distance between two vectors. */
inline float vec3_dist(Vec3 a, Vec3 b)
{
	float dx = b.x-a.x;
	float dy = b.y-a.y;
	float dz = b.z-a.z;

	return sqrtf(dx*dx + dy*dy + dz*dz);
}





// ----------------------------------------------------------------------------------------------- //
// Matrix                                                                                          //
// ----------------------------------------------------------------------------------------------- //
/* mat_init *************************************************************************************//**
 * @brief		Initializes a matrix. */
inline void mat_init(Matrix* m, unsigned r, unsigned c, float data[r][c])
{
	m->data     = &data[0][0];
	m->numcols  = c;
	m->startrow = 0;
	m->endrow   = r;
	m->startcol = 0;
	m->endcol   = c;
}


/* mat_slice ************************************************************************************//**
 * @brief		Creates a smaller slice of a matrix. The indices and dimensions are specified with
 * 				half open intervals. For example:
 *
 * 					slice rows = [startrow, endrow),
 * 					slice cols = [startcol, endcol).
 *
 * 					Matrix A
 * 					| 00  01  02  03  04 |   | 01  02  03 | = slice(A, 0, 1, 1, 4)
 * 					| 10  11  12  13  14 |                             |  |  |  |
 * 					| 20  21  22  23  24 |   | 22  23  24 | = slice(A, 2, 5, 2, 5)
 * 					| 30  31  32  33  34 |   | 32  33  34 |            |  |  |  |
 * 					| 40  41  42  43  44 |   | 42  43  44 |            |  |  |  |
 * 					                                                   rows  cols
 * @param[in]	m: the matrix to slice.
 * @param[in]	startr: the first row to include in the slice.
 * @param[in]	endr:   the last row to include in the slice.
 * @param[in]	startc: the first column to include in the slice.
 * @param[in]	endc:   the last column to include in the slice. */
inline void mat_slice(
	Matrix* dest,
	const Matrix* src,
	uint16_t startrow,
	uint16_t endrow,
	uint16_t startcol,
	uint16_t endcol)
{
	if(src->startrow <= startrow && startrow <= endrow && endrow <= src->endrow &&
	   src->startcol <= startcol && startcol <= endcol && endcol <= src->endcol)
	{
		dest->data     = src->data;
		dest->numcols  = src->numcols;
		dest->startrow = startrow;
		dest->endrow   = endrow;
		dest->startcol = startcol;
		dest->endcol   = endcol;
//		return true;
	}
//	else
//	{
//		return false;
//	}
}


/* mat_zero *************************************************************************************//**
 * @brief		Sets every entry in the matrix to 0. */
inline void mat_zero(Matrix* m)
{
	unsigned i;

	for(i = m->startrow; i < m->endrow; i++)
	{
		memset(&m->data[i * m->numcols + m->startcol], 0, mat_cols(m) * sizeof(float));
	}
}


#ifdef __cplusplus
}
#endif

#endif // MATRIX_H
/******************************************* END OF FILE *******************************************/
