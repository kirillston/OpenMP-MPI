#include <mpi.h>
#include <random>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>

void generate_array(int *array, int length) {
  	for (int i = 0; i < length; i++) {
    		array[i] = (rand() % 100);
  	}
}

std::string array_to_string(int rank, int *array, int length) {
	std::string output = std::to_string(rank) + ": ";
	for (int i = 0; i < length; i++) {
		output += "\t" + std::to_string(array[i]);
  	}
	return output + "\n";
}

void print_array(int rank, int *array, int length) {
	//printf("%d: ", rank);
  	for (int i = 0; i < length; i++) {
    		printf("%d\t", array[i]);
  	}
  	printf("\n");
}

/**
	Each process of the program generates its own array of integers
	with the size equal to processes number.
	
	Process simulates MPI_Reduce_scatter() call and measures time 
	between sending data to other processes and accomplishing all
	the computations.
	
	Resulting time is then written down in output 'time.txt' file.
**/

int main(int argc, char* argv[]) {
	MPI_Init( &argc, &argv );

	MPI_Comm comm = MPI_COMM_WORLD;
	int rank, size;
	MPI_Comm_rank( comm, &rank );
	MPI_Comm_size( comm, &size );
	MPI_Status status;

	int array[size];
	srand(time(NULL) + rank);
	generate_array(array, size);
	std::string str = array_to_string(rank, array, size);
	const char* cstr = str.c_str();
	printf(cstr);

	int accum = array[rank];
	int recv_value = 0;

	double t,t2;
	t=MPI_Wtime();
	
	for (int i = 0; i < size; i++) {
		if (i != rank) {
			MPI_Send(&array[i], 1, MPI_INT,i,0,comm);
			MPI_Recv(&recv_value, 1, MPI_INT,i,0,comm, &status);
			accum += recv_value;
		}
	}
	t2=MPI_Wtime();
	printf("%d: Sum is %d\n", rank, accum);
	if (rank == 0) {
		printf("%d: Time of Sending and Recieving = %le\n",rank,(t2-t)/100);
		std::ofstream ifs;
		ifs.open("time.txt");
		ifs << (t2-t)/100;
		ifs.close();
	}
	MPI_Finalize();
	return 0;
}

