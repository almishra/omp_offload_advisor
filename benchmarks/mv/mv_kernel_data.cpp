#include "mv.h"

void mv_kernel(double (*A)[N2], double *B, double *C, FILE *fp)
{
#pragma omp target enter data map(to: A[0:N1][0:N2], B[0:N2]) map(alloc: C[0:N1])
#pragma omp target teams distribute parallel for
  for(int i=0; i<N1; i++) {
    double sum = 0.0;
    for(int j=0; j<N2; j++) {
      sum = sum + A[i][j] * B[j];
    }
    C[i] = sum;
  }
#pragma omp target exit data map(from: C[0:N1])
}
