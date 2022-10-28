#include "transpose.h"

void transpose_kernel(double (*A)[N2], double (*B)[N1], FILE *fp)
{
#pragma omp target enter data map(to: A[0:N1][0:N2]) map(alloc: B[0:N2][0:N1])
#pragma omp target teams distribute parallel for collapse(2)
  for(int i=0; i<N2; i++) {
    for(int j=0; j<N1; j++) {
      B[i][j] = A[j][i];
    }
  }
#pragma omp target exit data map(from: B[0:N2][0:N1])
}
