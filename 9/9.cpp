#include "mpi.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

int N;

int process_file(const char* filename) {
	std::ifstream ifs;
	
	ifs.open(filename);
	if (ifs.fail()) {
		printf("Passed argument should be a correct filename.\n");
		return 1;
	}
	ifs >> N;

	int v[N];
	for (int i = 0; i < N; i++) {
		ifs >> v[i];
	}

	FILE *fptr;
	fptr = fopen("temp","w");
	if(fptr == NULL)
   	{
      		printf("Error!");   
      		return 1;             
   	}

	for (int i = 0; i < N; i++) {
		fwrite(&v[i], sizeof(int), 1, fptr);
	}
	fclose(fptr);
	return 0;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Single argument - input file name - expected.\n");
		return 1;
	}

	if (process_file(argv[1]) != 0) {
		return 2;
	}

	MPI_Init( &argc, &argv );

	MPI_Comm comm = MPI_COMM_WORLD;
	int rank, size;
	MPI_Comm_rank( comm, &rank );
	MPI_Comm_size( comm, &size );
	int real_size = size;
	MPI_Status status;

	if (size*2 > N) size = (int)N/2;

	int max_group = 2;
	while (max_group * size < N) {
		max_group ++;
	}
	int group = max_group;
	size_t offset = 0;

	if (max_group*size - N > rank && 2*rank < N ) {
		group -= 1;
		offset = rank*group;
	} else {
		offset = rank*(group-1) + rank - (max_group*size - N);
	}

	offset *= sizeof(int);

	MPI_File fh;
	MPI_File_open( comm, "temp", MPI_MODE_RDONLY, MPI_INFO_NULL, &fh );

	MPI_File_set_view(fh,offset,MPI_CHAR,MPI_CHAR,"native",MPI_INFO_NULL);

	if (N/2 - rank >= 1) {

		int* buf = (int*)malloc( sizeof(int) * group);
		*buf = rank;
		MPI_File_read(fh, buf, group, MPI_INT, &status);

		long long int result = *buf;

		for(int i = 1; i < group; i++) {
			result *= buf[i];
		}
		//printf("%d: Result = %lld\n", rank, result);

		free(buf);

		long long int* recv_buf = (long long int*)malloc(sizeof(long long int));
		*recv_buf = 1;

		while(rank < (float)size/2 && size != 1) {
			if (rank < ((float)size-1)/2) {
				//printf("%d: recv from %d\n", rank, size-rank-1);
				MPI_Recv(recv_buf, 1, MPI_LONG_LONG_INT,size-rank-1,0,comm, &status);
				result *= *recv_buf;
			} 
			size = size % 2 == 0 ? size/2 : (size + 1)/2;
		}
		if (size > 1) {
			//printf("%d: send to %d\n", rank, size-rank-1);
			MPI_Send(&result, 1, MPI_LONG_LONG_INT,size-rank-1,0,comm);
		} else {
			if (rank == 0) {
				printf("%d: Final result = %lld\n", rank, result);
			} else {
				//printf("%d: send to %d\n", 1, 0);
				MPI_Send(&result, 1, MPI_LONG_LONG_INT,0,0,comm);
			}
		}
		free(recv_buf);
	}

	MPI_File_close(&fh);

	MPI_Finalize();
	return 0;
}
