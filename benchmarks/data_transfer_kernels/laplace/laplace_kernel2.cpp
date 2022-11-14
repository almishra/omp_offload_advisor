#include "laplace.h"

void laplace_kernel2(double (*A)[N], double (*Anew)[N], FILE *fp)
{
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: Anew[0:M][0:N]) \
                                    map(to: A[0:M][0:N])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "laplace_kernel2,enter,%ld,%ld\n", 2*sizeof(double)*M*N,
          (end - start));
#pragma omp target teams distribute parallel for
  for(int i = 1; i < M-1; i++) {
    for(int j = 1; j < N-1; j++) {
      A[i][j] = Anew[i][j];      
    }
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: Anew[0:M][0:N]) \
                                    map(from: A[0:M][0:N])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "laplace_kernel2,exit,%ld,%ld\n", 2*sizeof(double)*M*N,
          (end - start));
}
