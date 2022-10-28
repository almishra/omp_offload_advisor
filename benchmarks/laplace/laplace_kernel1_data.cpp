#include "laplace.h"

double laplace_kernel1(double (*A)[N], double (*Anew)[N], double err, FILE *fp)
{
#pragma omp target enter data map(alloc: Anew[0:M][0:N]) \
                              map(to: A[0:M][0:N], err)
#pragma omp target teams distribute parallel for reduction(max: err)
  for(int i = 1; i < M-1; i++) {
    for(int j = 1; j < N-1; j++) {
      Anew[i][j] = 0.25 * (A[i][j+1] + A[i][j-1] + A[i-1][j] + A[i+1][j]);

      double val;
      if(Anew[i][j] > A[i][j]) val = Anew[i][j] - A[i][j];
      else val = A[i][j] - Anew[i][j];

      if(err < val)
        err = val;
    }
  }
#pragma omp target exit data map(from: Anew[0:M][0:N], err)

  return err;
}
