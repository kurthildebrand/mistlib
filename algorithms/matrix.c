/************************************************************************************************//**
 * @file		matrix.c
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
#include <math.h>
#include <string.h>

#include "matrix.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern Vec3     make_vec3     (float, float, float);
extern void     vec3_set      (Vec3*, float, float, float);
extern bool     vec3_is_finite(const Vec3*);

extern float    vec3_dot      (Vec3, Vec3);
extern Vec3     vec3_cross    (Vec3, Vec3);
extern float    vec3_dist     (Vec3, Vec3);
extern float    vec3_norm2    (Vec3);
extern float    vec3_norm     (Vec3);
extern Vec3     vec3_add      (Vec3, Vec3);
extern Vec3     vec3_sub      (Vec3, Vec3);
extern Vec3     vec3_scale    (Vec3, float);
extern Vec3     vec3_unit     (Vec3);

extern void     mat_init      (Matrix*, unsigned r, unsigned c, float[r][c]);
extern void     mat_slice     (Matrix*, const Matrix*, uint16_t, uint16_t, uint16_t, uint16_t);
extern unsigned mat_rows      (const Matrix*);
extern unsigned mat_cols      (const Matrix*);
extern void     mat_zero      (Matrix*);


/* Private Functions ----------------------------------------------------------------------------- */
static        void     mat_scale_row(const Matrix*, unsigned, float);
static        void     mat_add_rows (const Matrix*, unsigned, unsigned, float);
static        void     mat_swap_rows(const Matrix*, unsigned, unsigned);
static inline unsigned mat_xrows    (const Matrix*, bool);
static inline unsigned mat_xcols    (const Matrix*, bool);
static inline float*   mxentry      (const Matrix*, unsigned, unsigned, bool);
static inline float*   mxoffset     (const Matrix*, unsigned, unsigned, bool);


/* mat_ident ************************************************************************************//**
 * @brief		Initializes a matrix to be an identity matrix. */
void mat_ident(Matrix* m)
{
	mat_zero(m);

	unsigned i;
	unsigned rows = mat_rows(m);
	unsigned cols = mat_cols(m);
	unsigned end  = rows < cols ? rows : cols;

	for(i = 0; i < end; i++)
	{
		*mxentry(m,i,i,0) = 1.0f;
	}
}


/* mat_copy *************************************************************************************//**
 * @brief		Copys the source matrix to the destination. Accepts matrix slices. */
bool mat_copy(Matrix* dest, const Matrix* src)
{
	if(mat_rows(dest) != mat_rows(src) ||
	   mat_cols(dest) != mat_cols(src))
	{
		return false;
	}

	unsigned i;

	for(i = 0; i < mat_rows(dest); i++)
	{
		memmove(
			mxoffset(dest, i, 0, 0),
			mxoffset(src,  i, 0, 0),
			mat_cols(dest) * sizeof(float));
	}

	return true;
}


/* mat_norm2 ************************************************************************************//**
 * @brief		Computes the squared euclidean norm of the entries in the matrix. For example:
 *
 * 					m = [ 10, 20, 30 ]
 * 					norm2 = 10^2 + 20^2 + 30^2 = 1400
 *
 * 				Iterates every entry in the matrix, even if the matrix is not a row or column
 * 				vector. */
float mat_norm2(const Matrix* m)
{
	unsigned i,j;

	float norm2 = 0;

	for(i = m->startrow; i < m->endrow; i++)
	{
		for(j = m->startcol; j < m->endcol; j++)
		{
			float value = *mxentry(m,i,j,0);

			norm2 += value * value;
		}
	}

	return norm2;
}


// /* mat_mult *************************************************************************************//**
//  * @brief		Computes c = alpha * a * b + beta * c. The matrix c must not overlap a or b. */
// bool mat_mult(Matrix* c, float alpha, const Matrix* a, const Matrix* b, float beta)
// {
// 	return mat_xmult(c, alpha, a, b, beta, 0);
// }


// /* mat_mult_at **********************************************************************************//**
//  * @brief		Computes c = alpha * a' * b + beta * c. The matrix c must not overlap a or b. */
// bool mat_mult_at(Matrix* c, float alpha, const Matrix* a, const Matrix* b, float beta)
// {
// 	return mat_xmult(c, alpha, a, b, beta, MATRIX_A_TRANS);
// }


// /* mat_mult_bt **********************************************************************************//**
//  * @brief		Computes c = alpha * a * b' + beta * c. The matrix c must not overlap a or b. */
// bool mat_mult_bt(Matrix* c, float alpha, const Matrix* a, const Matrix* b, float beta)
// {
// 	return mat_xmult(c, alpha, a, b, beta, MATRIX_B_TRANS);
// }


/* mat_mult *************************************************************************************//**
 * @brief		Computes c = alpha * a * b + beta * c. The matrix c must not overlap a or b.
 * @param[in]	c: matrix to store the result.
 * @param[in]	alpha: factor to multiply along with a * b.
 * @param[in]	a: the first matrix to multiply.
 * @param[in]	b: the second matrix to multiply.
 * @param[in]	beta: the factor to multiply along with c.
 * @param[in]	xflags: bitflags controlling which matrices are transposed. The following values may
 * 				be or'ed together: MATRIX_A_TRANS, MATRIX_B_TRANS, MATRIX_C_TRANS. */
bool mat_mult(Matrix* c, float alpha, const Matrix* a, const Matrix* b, float beta, unsigned xflags)
{
	unsigned i, j, k;

	bool xa = xflags & MATRIX_A_TRANS;
	bool xb = xflags & MATRIX_B_TRANS;
	bool xc = xflags & MATRIX_C_TRANS;

	/* Verify dimensions of the matrices. Anm * Bmp = Cnp */
	if(mat_xrows(a,xa) != mat_xrows(c,xc) ||
	   mat_xcols(a,xa) != mat_xrows(b,xb) ||
	   mat_xcols(b,xb) != mat_xcols(c,xc))
	{
		return false;
	}

	for(i = 0; i < mat_xrows(c,xc); i++)
	{
		for(j = 0; j < mat_xcols(c,xc); j++)
		{
			float temp = 0;

			for(k = 0; k < mat_xcols(a,xa); k++)
			{
				temp += (*mxoffset(a,i,k,xa)) * (*mxoffset(b,k,j,xb));
			}

			*mxoffset(c,i,j,xc) = (alpha * temp) + (beta * (*mxoffset(c,i,j,xc)));
		}
	}

	return true;
}


/* 	Original A          R Matrix             Q Matrix           A, decomposed into R and Q
 * 	A = [               R = [                Q = [              A = [
 * 		a00 a01 a02     	r00 r01 r02      	1   0   0       	r00 r01 r02
 * 		a10 a11 a12     	0   r11 r12      	v10 1   0       	v10 r11 r12
 * 		a20 a21 a22     	0   0   r22      	v20 v21 1       	v20 v21 r22
 * 		a30 a31 a32     ]                    	v30 v31 v32     	v30 v31 v32
 * 		a40 a41 a42                          	v40 v41 v42     	v40 v41 v42
 * 	]                                        ]                  ]
 *
 * 	Q = H1 * H2 * ... * Hn
 * 	R = Hn * ... * H2 * H1 * A
 *
 * 	Q = I - V * T * V'
 *
 * 	alpha = a_ii
 * 	beta  = -sign(alpha)
 * 	V =
 *
 */


/* mat_qr ***************************************************************************************//**
 * @brief		Performs QR decomposition on the matrix A. Stores the decomposition in compact form.
 * 				That is, the upper triangular part of A contains R. The lower triangular part of A
 * 				contains the Householder reflectors used to perform the decomposition.
 * @warning		The tau array must have length == cols(A). */
void mat_qr(Matrix* a, float* tau)
{
	unsigned m = mat_rows(a);
	unsigned n = mat_cols(a);

	float vdata[m][1];
	float vtadata[1][n];

	Matrix u;	/* Slice which accesses the column to reflect.                             */
	Matrix v;	/* Slice which copies the column out of A.                                 */
	Matrix vta;	/* Row vector that stores the intermediate result: v(k:m,k)' * A(k:m,k:n). */

	mat_init(&v,   m, 1, vdata);
	mat_init(&vta, 1, n, vtadata);
	mat_zero(&v);
	mat_zero(&vta);

	unsigned i,k;

	for(k = 0; k < n; k++)
	{
		mat_slice(&u, a,  k, m, k, k+1);	/* u = A(k:m,k) */
		mat_slice(&v, &v, k, m, 0, 1);		/* v = v(k:m,1) */
		mat_copy (&v, &u);					/* v = u        */

		float alpha = *mxoffset(&u, 0, 0, 0);
		float beta  = -copysignf(1.0f, alpha) * sqrtf(mat_norm2(&u));
		tau[k]      = (beta - alpha) / beta;

		/* v(k,k) = 1.0 */
		*mxentry(&v, k, 0, 0) = 1.0f;

		/* v(k+1:m,1) = u / (alpha - beta) */
		for(i = k+1; i < m; i++)
		{
			*mxentry(&v, i, 0, 0) /= (alpha - beta);
		}

		/* Reuse u to slice into A. u = A(k:m,k:n) */
		mat_slice(&u, a, k, m, k, n);

		/* Row vector to store the result v(k:m,1)' * A(k:m,k:n) */
		mat_slice(&vta, &vta, 0, 1, k, n);

		/* vta = v(k:m,1)' * A(k:m,k:n) */
		mat_mult(&vta, 1.0, &v, &u, 0, MATRIX_A_TRANS);

		/* A(k:m,k:n) = A(k:m,k:n) - tau(k) * v(k:m,1) * (v(k:m,1)' * A(k:m,k:n)) */
		mat_mult(&u, -tau[k], &v, &vta, 1.0f, 0);

		/* Store the reflector vector back into A */
		mat_slice(&u,  a, k+1, m, k, k+1);
		mat_slice(&v, &v, k+1, m, 0, 1);
		mat_copy (&u, &v);

		// mat_mult(&w, 1.0, mat_trans(mat_slice(v, k,m, k,k)), mat_slice(A, k,m, k,n), 0);
	}
}


/* mat_mult_qt **********************************************************************************//**
 * @brief		Computes Q^T * b. Stores the result in b. Q is represented by n Householder
 * 				reflectors Q = H1 * H2 * ... * Hn where n is the number of columns in Q. */
void mat_mult_qt(Matrix* a, Matrix* b, float* tau)
{
	unsigned i,j;

	/* Q^T = I - V * T' * V'. Form the T matrix. */
	unsigned ma = mat_rows(a);
	unsigned na = mat_cols(a);
	unsigned nb = mat_cols(b);

	float rdata[na][na];	/* Store a copy of the upper triangular part of A */
	float tdata[na][na];	/* Store T */
	float w1data[na][nb];	/* Work data 1 */
	float w2data[na][nb];	/* Work data 2 */

	memset(tdata, 0, sizeof(tdata));
	memset(w1data, 0, sizeof(w1data));

	Matrix t1, t2;
	Matrix v1, v2;
	Matrix w;

	/* Temporarily store a copy the upper triangular part of A */
	memmove(rdata, mxoffset(a, 0, 0, 0), sizeof(rdata));

	/* Form Q inplace in matrix A. */
	for(i = 0; i < na; i++) {
		for(j = i; j < na; j++) {
			*mxoffset(a, i, j, 0) = i == j ? 1.0f : 0.0f;
		}
	}

	/* Compute the T matrix. */
	for(i = 0; i < na; i++)
	{
		mat_init(&w,  na, 1,  w1data);
		mat_init(&t1, na, na, tdata);
		mat_init(&t2, na, na, tdata);

		if(i != 0)
		{
			mat_slice(&v1, a,   0, ma, 0, i);	/* v1 = v(:,1:i-1)     */
			mat_slice(&v2, a,   0, ma, i, i+1);	/* v2 = v(:,i)         */
			mat_slice(&t1, &t1, 0, i,  i, i+1);	/* t1 = T(1:i-1,i)     */
			mat_slice(&t2, &t2, 0, i,  0, i);	/* t2 = T(1:i-1,1:i-1) */
			mat_slice(&w,  &w,  0, i,  0, 1);

			/* w = v(:,0:i-1)' * v(:,i) */
			mat_mult(&w, 1.0f, &v1, &v2, 0.0f, MATRIX_A_TRANS);

			/* T(0:i-1,i) = -tau(i) * T(0:j-1,0:j-1) * v(:,0:i-1)' * v(:,i) */
			mat_mult(&t1, -tau[i], &t2, &w, 0.0f, 0);
		}

		/* T(i,i) = tau[i]; */
		*mxentry(&t1, i, i, 0) = tau[i];
	}

	/* Compute Q^T * b:
	 *
	 * 		Q^T * b = (I - V * T * V')' * b
	 * 		        = (I - V * T' * V') * b
	 * 		        =  b - V * T' * V' * b
	 * 		        =  b - V * (T' * (V' * b))
	 *
	 * 		      b =  b - V  * T' * V'  * b
	 * 		      █    █   ██   ██   ███   █
	 * 		      █ =  █ - ██ * ██ * ███ * █
	 * 		      █    █   ██              █
	 *
	 * Note:
	 *
	 * 		(V1 * T * V2')' = V2'' * T' * V1'
	 * 		                = V2 * T' * V1'
	 *
	 * let v1 = v2:
	 *
	 * 		                = V1 * T' * V1'
	 *
	 * thus:
	 *
	 * 		(I - V * T * V')' = I - V * T' * V'
	 */
	mat_init(&t1, na, na, tdata);

	/* v1 = V' * b
	 *    = cols(V) x cols(b) */
	mat_init(&v1, na, nb, w1data);
	mat_mult(&v1, 1.0f, a, b, 0.0, MATRIX_A_TRANS);

	/* v2 = t1' * v1
	 *    =  T' * V' * b
	 *    = cols(V) x cols(b) */
	mat_init(&v2, na, nb, w2data);
	mat_mult(&v2, 1.0f, &t1, &v1, 0, MATRIX_A_TRANS);

	/* b = b - V * v2
	 *   = b - V * T' * V' * b */
	mat_mult(b, -1.0f, a, &v2, 1.0f, 0);

	/* Restore the upper triangular part of A */
	memmove(mxoffset(a, 0, 0, 0), rdata, sizeof(rdata));
}


/* mat_backsub **********************************************************************************//**
 * @brief		*/
void mat_qr_backsub(Matrix* a, Matrix* b)
{
	unsigned i,j,k;

	unsigned m   = mat_rows(a);
	unsigned n   = mat_cols(a);
	unsigned end = m < n ? m : n;

	/* Zero lower diagonal */
	for(i = 1; i < m; i++)
	{
		for(j = 0; j < i; j++)
		{
			*mxoffset(a, i, j, 0) = 0;
		}
	}

	/* Solve for the solution by back substituting, traversing backwards along the diagonal */
	for(k = end; k-- > 0; )
	{
		/* Find the row's pivot element. Pivot will exist at or to the right of the diagonal. */
		for(j = k; j < n; j++)
		{
			if(fabs(*mxentry(a, k, j, 0)) > MATRIX_TOLERANCE)
			{
				float temp = 1.0 / *mxentry(a, k, j, 0);

				mat_scale_row(a, k, temp);
				mat_scale_row(b, k, temp);

				/* Elimintate the solution from preceding rows */
				for(i = 0; i < k; i++)
				{
					temp = *mxentry(a, i, j, 0) / *mxentry(a, k, j, 0);

					mat_add_rows(a, i, k, -1.0f * temp);
					mat_add_rows(b, i, k, -1.0f * temp);
				}

				break;
			}
		}
	}
}





// ----------------------------------------------------------------------------------------------- //
// Private Functions                                                                               //
// ----------------------------------------------------------------------------------------------- //
/* mat_scale_row ********************************************************************************//**
 * @brief		Multiplies the specified row by the specified scalar. */
static void mat_scale_row(const Matrix* m, unsigned row, float scalar)
{
	unsigned col;
	unsigned end = mat_cols(m);

	for(col = 0; col < end; col++)
	{
		*mxoffset(m, row, col, 0) *= scalar;
	}
}


/* mat_add_rows *********************************************************************************//**
 * @brief		Performs the following row operation to the specified matrix:
 *
 * 					row rd = scalar * row rs
 *
 * @param[in]	m: the matrix to operate on.
 * @param[in]	rd: the destination row.
 * @param[in]	rs: the source row
 * @param[in]	scalar: the scalar multiplied to the source row. */
static void mat_add_rows(const Matrix* m, unsigned rd, unsigned rs, float scalar)
{
	unsigned col;
	unsigned end = mat_cols(m);

	for(col = 0; col < end; col++)
	{
		*mxoffset(m, rd, col, 0) += scalar * (*mxoffset(m, rs, col, 0));
	}
}


/* mat_swap_rows ********************************************************************************//**
 * @brief		Swaps r1 and r2 in the matrix. */
static void mat_swap_rows(const Matrix* m, unsigned r1, unsigned r2)
{
	if(r1 != r2)
	{
		unsigned col;
		unsigned end = mat_cols(m);
		for(col = 0; col < end; col++)
		{
			float temp = *mxoffset(m, r1, col, 0);
			*mxoffset(m, r1, col, 0) = *mxoffset(m, r2, col, 0);
			*mxoffset(m, r2, col, 0) = temp;
		}
	}
}


/* mat_xrows ************************************************************************************//**
 * @brief		Returns the number of rows in the normal or transposed matrix. */
static inline unsigned mat_xrows(const Matrix* m, bool transpose)
{
	return !transpose ? mat_rows(m) : mat_cols(m);
}


/* mat_xrows ************************************************************************************//**
 * @brief		Returns the number of cols in the normal or transposed matrix. */
static inline unsigned mat_xcols(const Matrix* m, bool transpose)
{
	return !transpose ? mat_cols(m) : mat_rows(m);
}


/* mxentry **************************************************************************************//**
 * @brief		Returns a pointer to the element at the specified absolute row and column. The
 * 				transpose flag changes how the matrix m is accessed.
 *
 * 					If transpose is true, then matrix m is accessed as if it was transposed.
 * 					If transpose is false, then matrix m is accessed normally.
 */
static inline float* mxentry(const Matrix* m, unsigned row, unsigned col, bool transpose)
{
	unsigned r = !transpose ? row : col;
	unsigned c = !transpose ? col : row;
	return &m->data[r * m->numcols + c];
}


/* mxoffset *************************************************************************************//**
 * @brief		Returns a pointer to the element at the specified offset row and column. The
 * 				transpose flag changes how the matrix m is accessed.
 *
 * 					If transpose is true, then matrix m is accessed as if it was transposed.
 * 					If transpose is false, then matrix m is accessed normally.
 */
static inline float* mxoffset(const Matrix* m, unsigned row, unsigned col, bool transpose)
{
	unsigned r = !transpose ? row : col;
	unsigned c = !transpose ? col : row;
	return &m->data[(r + m->startrow) * m->numcols + (c + m->startcol)];
}


/******************************************* END OF FILE *******************************************/
