#include <omp.h>
#include <math.h>
#include <stdio.h>
#define N 20

int main() {
	double a[N];
	a[0]=1;
	double w;
	# pragma omp parallel for
	for (int i = 1; i < N; i++)
	{
		w = sin(i / N * 3.14);
		a[i] = w * a[i-1];
	}
	for (int i = 0; i < N; i++) {
		printf("%f\n", w);
	}
		
	return 0;
}

/*
Found problems:

1) Cast to double in argument of sinus expected
2) Not good PI precision
3) Read-after-Write loop collision

*/