#include "laplace.h"

void laplace_kernel2(double (*A)[N], double (*Anew)[N], FILE *fp)
{
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: Anew[0:M][0:N]) \
                              map(alloc: A[0:M][0:N])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "laplace_kernel2_data,enter,%ld,%ld\n", sizeof(double)*M*N,
          (end - start));
#pragma omp target teams distribute parallel for
  for(int i = 1; i < M-1; i++) {
    for(int j = 1; j < N-1; j++) {
      A[i][j] = Anew[i][j];      
    }
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: A[0:M][0:N])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "laplace_kernel2_data,exit,%ld,%ld\n", sizeof(double)*M*N,
          (end - start));
}
