#include <omp.h>
#include <iostream>
#include <ios>
#include <fstream>
#include <random>
#include <stdio.h>

#define DIM 3
#define EPS 0.0001f

int generate_vector_list(int size, float min, float max, float list[][DIM]) {
	std::random_device rd;
    std::mt19937 mt(rd());
	std::uniform_real_distribution<float> coordinates_dist(min, max);
	#pragma omp parallel for
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < DIM; j++) {
			list[i][j] = coordinates_dist(mt);
		}
	}
	return 0;
}

int fact(int N) {
	if (N == 0)
        return 1;
    else
        return N * fact(N - 1);
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Single argument - number of vectors or input file name - expected.\n");
		return 1;
	}
	
	int N = atoi(argv[1]);
	bool from_file = false;
	
	std::ifstream ifs;
	
	if (N <= 0) {
		ifs.open(argv[1]);
		if (ifs.fail()) {
			printf("Passed argument should be a positive integer or correct filename.\n");
			return 2;
		}
		from_file = true;
		ifs >> N;
	}
	
	if (N <= 2) {
		printf("Number of vectors should be 3 or more to find complanar ones.\n");
		return 3;
	}
	
	
	printf("Number of vectors is %d\nHere they are:\n", N);	
	float v[N][DIM];
	
	if (!from_file)	{
		generate_vector_list(N, 0, 9, v);
	} else {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < DIM; j++) {
				ifs >> v[i][j];
			}
		}
	}
	
	for (int i = 0; i < N; i++) {
		printf("%d) ", i);
		for (int j = 0; j < DIM; j++) {
			printf("%2.3f ", v[i][j]);
		}
		printf("\n");
	} 
	
	//int threads_num = omp_get_max_threads();
	int triplets_num = fact(N) / (fact(DIM) * fact(N - DIM));
	
	// indices of vectors in each potential triplet
	int a[triplets_num];
	int b[triplets_num];
	int c[triplets_num];
	
	int u = 0;
	for (int i = 0; i < N - 2; i++) {
		for (int j = i + 1; j < N - 1; j++) {
			for (int k = j + 1; k < N; k++) {
				a[u] = i;
				b[u] = j;
				c[u] = k;
				u++;
			}
		}
	}
	/*
	for (int i = 0; i < triplets_num; i++) {
		printf("%d %d %d\n", a[i], b[i], c[i]);
	}
	*/
	float pos_det[triplets_num];
	
	int i = 0;
	int j = 0;
	int k = 0;
	int complanar_count = 0;
	
	#pragma omp parallel for
	for (int u = 0; u < triplets_num; u++) {
		i = a[u];
		j = b[u];
		k = c[u];
		float det = v[i][0]*v[j][1]*v[k][2] + v[i][1]*v[j][2]*v[k][0] + v[i][2]*v[j][0]*v[k][1]
				  - v[i][2]*v[j][1]*v[k][0] - v[i][0]*v[j][2]*v[k][1] - v[i][1]*v[j][0]*v[k][2];
		if (abs(det) < EPS) {
			printf("%d %d %d: det=%.1f \t - complanar\n", i, j, k, det);
			complanar_count++;
		} else {
			printf("%d %d %d: det=%.1f\n", i, j, k, det);
		}
	}
	printf("Number of complanar vector triplets is %d\n", complanar_count);
	return 0;
}
