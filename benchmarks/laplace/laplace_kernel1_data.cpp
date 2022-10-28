#include "laplace.h"

double laplace_kernel1(double (*A)[N], double (*Anew)[N], double err, FILE *fp)
{
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(alloc: Anew[0:M][0:N]) \
                              map(to: A[0:M][0:N], err)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "laplace_kernel1_data,enter,%ld,%ld\n", sizeof(double)*M*N + sizeof(double),
          (end - start));
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
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: Anew[0:M][0:N], err)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "laplace_kernel1_data,enter,%ld,%ld\n", sizeof(double)*M*N + sizeof(double),
          (end - start));

  return err;
}
