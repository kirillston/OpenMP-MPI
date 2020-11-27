#include <omp.h>
#include <math.h>
#include <stdio.h>


int main(int argc, char** argv) {
	
	if (argc != 2) {
		printf("Single argument - array length - expected.\n");
		return 1;
	}
	int N = atoi(argv[1]);
	if (N == 0) {
		printf("Passed argument - array length - should be a positive integer.\n");
		return 2;
	}
	double a[N];
	// array initialization
	# pragma omp parallel for
	for (int i = 0; i < N; i++) {
		a[i] = 1;
	}
	
	// auxiliary sina array initialization
	int sin_num = N % 2 == 0 ? (N-2)/2 : (N-1)/2;
	double sina[sin_num];
	# pragma omp parallel for
	for (int i = 0; i < sin_num; i++) {
		sina[i] = sin(((i + 1) / (double)N) * M_PI);
	}
	
	for (int i = 0; i < sin_num; i++) {
		# pragma omp parallel for
		for (int j = i + 1; j < N; j++) {
			a[j] *= sina[i];
		}
	}
	for (int i = sin_num - 1; i > -1; i--) {
		# pragma omp parallel for
		for (int j = N - i - 1; j < N; j++) {
			a[j] *= sina[i];
		}
	}
	
	for (int i = 0; i < N; i++) {
		printf("a[%d] = %1.32f\n", i, a[i]);
	}
	return 0;
}