#include <stdio.h>
#include <x86intrin.h>
#include <omp.h>

void mat_op(double * self_mat, double * mat, double a, int dim) {
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			self_mat[i*dim+j] = (1 - a) * self_mat[i*dim+j] + a * mat[i*dim+j];
		}
	}
}

void mat_op_vec(double * self_mat, double * mat, double a, const int dim) {
	__m128d ai = _mm_set_pd(1-a, 1-a);
	__m128d bi = _mm_set_pd(a, a);
	for (int i = 0; i < dim*dim; i+=2) {
		__m128d Ai = _mm_loadu_pd(self_mat + i);
		__m128d Bi = _mm_loadu_pd(mat + i);

		__m128d R1 = _mm_mul_pd(Ai, ai);
		__m128d R2 = _mm_mul_pd(Bi, bi);
		__m128d R  = _mm_add_pd(R1, R2);
		
		_mm_storeu_pd(self_mat + i, R);
	}
}

void mat_op_omp(double * self_mat, double * mat, double a, const int dim) {
	#pragma omp parallel for
	for (int i = 0; i < dim*dim; i++) {
		self_mat[i] = (1-a) * self_mat[i] + a * mat[i];
	}
}

void mat_op_vec_omp(double * self_mat, double * mat, double a, const int dim) {
	__m128d ai = _mm_set_pd(1-a, 1-a);
	__m128d bi = _mm_set_pd(a, a);

	#pragma omp parallel for
	for (int i = 0; i < dim*dim; i+=2) {
		__m128d Ai = _mm_loadu_pd(self_mat + i);
		__m128d Bi = _mm_loadu_pd(mat + i);

		__m128d R1 = _mm_mul_pd(Ai, ai);
		__m128d R2 = _mm_mul_pd(Bi, bi);
		__m128d R  = _mm_add_pd(R1, R2);
		
		_mm_storeu_pd(self_mat + i, R);
	}
}
