#include <mpi.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <stdio.h>

const double EPS = 0.001;
const double MAX = 99;

void generate_system(int N, std::vector<std::vector<double> > &A, std::vector<double> &F) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			A[i][j] = (double)((rand() - RAND_MAX/2) % 10000) / 100;
			if (i == j) A[i][j] *= 13;
		}
		F[i] = (double)((rand() - RAND_MAX/2) % 10000) / 100;
	}
}

void normalize_system(int N, std::vector<std::vector<double> > &A, std::vector<double> &F) {
	for (int i = 0; i < N - 1; i++) {
		double max_v = 0;
		int max_j = i;
		for (int j = i; j < N; j++) {
			if (abs(A[j][i]) > abs(max_v)) {
				max_v = A[j][i];
				max_j = j;
			}
		}
		//std::cout << i << " <-> " << max_j << "   <=   |" << A[i][i] << "| < |" << A[max_j][i] << "|\n";
		if (i != max_j) {
			swap(A[i],A[max_j]);
			std::swap(F[i],F[max_j]);
		}
	}
}

void print_matrix(int N, std::vector<std::vector<double> > &A, std::vector<double> &F) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			std::cout << A[i][j] << " ";
		}
		std::cout << " = " << F[i] << "\n";
	}
	std::cout << "\n";
}

/**
	This program solves system of linear algebraic 
	equations using plain itaration method.
	
	Input could be presented by one or two arguments.
	
	First argument N is the dimention of desirable system.
	
	Second argument is optional and should handle value
	'time_mode' or '-t' that means that all output 
	is forbidden and time of core execution is written
	down in file 'time.txt'.
**/

int main(int argc, char* argv[]) {

	// initialization of MPI library
	MPI_Init( &argc, &argv );

	MPI_Comm comm = MPI_COMM_WORLD;
	int rank, size;
	MPI_Comm_rank( comm, &rank );
	MPI_Comm_size( comm, &size );
	MPI_Status status;

	// input parsing
	int N = 2;
	bool time_mode = false;
	
	if (argc == 1) {
		std::cerr << "One or two arguments expected.\n";
		return 1;
	} else if (argc == 2) {
		N = atoi(argv[1]);
	} else if (argc == 3) {
		if (strcmp(argv[2], "time_mode") == 0 || strcmp(argv[2], "-t") == 0) {
			time_mode = true;
		} else {
			std::cerr << "Unknown second argument. 'time_mode' or '-t' expected.\n";
		}
		N = atoi(argv[1]);
	}

	bool output = !time_mode && rank == 0;

	// initialization of system
	std::vector<std::vector<double> > A(N,std::vector<double>(N));
	std::vector<double> F(N);
	
	srand(time(NULL));
	generate_system(N, A, F);
	if (output) {
		std::cout << "Generated system:\n";
		print_matrix(N, A, F);
	}

	normalize_system(N, A, F);
	if (output) {
		std::cout << "Normalized system:\n";
		print_matrix(N, A, F);
	}

	// forming matrix C and vector B
	std::vector<std::vector<double> > C(N,std::vector<double>(N));
	std::vector<double> B(N);

	for (int i = 0; i < N; i++) {
		double a = A[i][i];
		//std::cout << a << " ";
		for (int j = 0; j < N; j++) {
			C[i][j] = i != j ? (-1)*A[i][j] / a : 0;
		}
		B[i] = F[i] / a;
	} 

	if (output) {
		std::cout << "Modified system:\n";
		print_matrix(N, C, B);
	}

	// distribution of matrix lines among processes
	if (size > N) size = N;

	int min_group = (int)trunc((double)N / size);
	int stahanofs = N % size;
	int counts[size];
	int displs[size];
	for(int i = 0; i < size; i++) 
		counts[i] = i < stahanofs ? min_group + 1 : min_group;

	int start = 0;
	for (int i = 0; i < rank; i++) {
		start += counts[i];
	}
	int finish = start + counts[rank];

	displs[0] = 0;
	for(int i = 1; i < size; i++)
		displs[i] = displs[i-1] + counts[i-1];

	if(output) {
		std::cout << "Distribution between proc:\n";
		for(int i = 0; i < size; i++) {
			std::cout << i << ": " << counts[i] << " lines  displace is " << displs[i] << "\n";
		}
		std::cout << "\n";
	}

	// main algorithm
	std::vector<double> X0(N);
	std::vector<double> X(N);
	double diff;
	int step = 0;

	double t,t2;
	t=MPI_Wtime();

	for (int i = 0; i < N; i++) X0[i] = 1;

	if(output) {
		std::cout << "Initial vector X:\n";
		for(int i = 0; i < N; i++) {
			std::cout << X0[i] << " ";
		}
		std::cout << "\n";
	}

	do {
		step++;
		for (int i = start; i < finish; i++) {
			X[i] = 0;
			for (int j = 0; j < N; j++) {
				X[i] +=  C[i][j] * X0[j];
			}
			X[i] += B[i];
			//std::cout << rank << ": X" << i << " = " << X[i] << "\n";
		}

		double rawX[N];
		std::copy(X.begin(), X.end(), rawX);
		
		MPI_Allgatherv(&rawX[start], counts[rank], MPI_DOUBLE, 
				&rawX[0], counts, displs, MPI_DOUBLE, comm);

		std::copy(rawX, rawX + N, X.begin());

		if(output) {
			std::cout << "Step " << step << "  X:\n";
			for(int i = 0; i < N; i++) {
				std::cout << X[i] << " ";
			}
			std::cout << "\n";
		}
		
		diff = 0;
		for (int i = 0; i < N; i++)
			diff += pow(X[i] - X0[i], 2);
			
		X0 = X;

		if (step > MAX) break;

	} while (diff > EPS);

	t2=MPI_Wtime();

	if (output) {
		std::cout << "Time:" << (t2-t)/100 << "\n";
	} else if (rank == 0 && time_mode) {
		std::ofstream ifs;
		ifs.open("time.txt");
		ifs << (t2-t)/100;
		ifs.close();
	}

	MPI_Finalize();
	return 0;
}
