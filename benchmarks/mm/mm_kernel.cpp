#include "mm.h"

void mm_kernel(double (*A)[N2],
                   double (*B)[N3],
                   double (*C)[N3])
{
#pragma omp target enter data map(to: A[0:N1][0:N2], B[0:N2][0:N3], C[0:N1][0:N3]) 
#pragma omp target teams distribute parallel for collapse(2)
  for(int i=0; i<N1; i++) {
    for(int j=0; j<N3; j++) {
      double sum = 0.0;
      for (int k=0; k<N2; k++)
        sum = sum + A[i][k] * B[k][j];
      C[i][j] = sum;
    }
  }
#pragma omp target exit data map(from: A[0:N1][0:N2], B[0:N2][0:N3], C[0:N1][0:N3]) 
}
