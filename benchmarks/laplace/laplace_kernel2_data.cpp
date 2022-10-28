#include "laplace.h"

void laplace_kernel2(double (*A)[N], double (*Anew)[N], FILE *fp)
{
#pragma omp target enter data map(to: Anew[0:M][0:N]) \
                              map(alloc: A[0:M][0:N])
#pragma omp target teams distribute parallel for
  for(int i = 1; i < M-1; i++) {
    for(int j = 1; j < N-1; j++) {
      A[i][j] = Anew[i][j];      
    }
  }
#pragma omp target exit data map(from: A[0:M][0:N])
}
