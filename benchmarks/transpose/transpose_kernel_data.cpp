#include "transpose.h"

void transpose_kernel(double (*A)[N2], double (*B)[N1], FILE *fp)
{
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: A[0:N1][0:N2]) map(alloc: B[0:N2][0:N1])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "transpose_kernel_data,enter,%ld,%ld\n", sizeof(double)*(N1*N2),
        (end - start));
#pragma omp target teams distribute parallel for collapse(2)
  for(int i=0; i<N2; i++) {
    for(int j=0; j<N1; j++) {
      B[i][j] = A[j][i];
    }
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: B[0:N2][0:N1])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "transpose_kernel_data,exit,%ld,%ld\n", sizeof(double)*(N1*N2),
        (end - start));
}
