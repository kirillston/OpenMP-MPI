#include <omp.h>
#include <iostream>
#include <ios>
#include <fstream>
#include <random>
#include <math.h>
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
			printf("Passed argument should a positive integer or correct filename.\n");
			return 2;
		}
		from_file = true;
		ifs >> N;
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
	
	int complanar_count = 0;
	
	for (int i = 0; i < N - 2; i++) {
		for (int j = i + 1; j < N - 1; j++) {
			for (int k = j + 1; k < N; k++) {
				float det = v[i][0]*v[j][1]*v[k][2] + v[i][1]*v[j][2]*v[k][0] + v[i][2]*v[j][0]*v[k][1]
						  - v[i][2]*v[j][1]*v[k][0] - v[i][0]*v[j][2]*v[k][1] - v[i][1]*v[j][0]*v[k][2];
				if (abs(det) < EPS) {
					printf("%d %d %d: det=%.1f \t - complanar\n", i, j, k, det);
					complanar_count++;
				} else {
					printf("%d %d %d: det=%.1f\n", i, j, k, det);
				}
			}
		}
	}
	printf("Number of complanar vector triplets is %d\n", complanar_count);
	
	return 0;
}
