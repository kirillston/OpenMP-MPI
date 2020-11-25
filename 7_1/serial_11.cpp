#include <math.h>
#include <stdio.h>

int main(int argc, char** argv) {
	int N = atoi(argv[1]);
	double a[N];
	a[0]=1;
	double w;
	for (int i = 1; i < N; i++)
	{
		w = sin(i / (double)N * M_PI);
		a[i] = w * a[i-1];
	}
	for (int i = 0; i < N; i++) {
		printf("a[%d] = %f\n", i, a[i]);
	}
		
	return 0;
}