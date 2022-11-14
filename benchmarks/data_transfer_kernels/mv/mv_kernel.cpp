#include "mv.h"

void mv_kernel(double (*A)[N2], double *B, double *C, FILE *fp)
{
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: A[0:N1][0:N2], B[0:N2], C[0:N1])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "mv_kernel,enter,%ld,%ld\n", sizeof(double)*(N1*N2) + sizeof(double)*(N2+N1), (end - start));

#pragma omp target teams distribute parallel for
  for(int i=0; i<N1; i++) {
    double sum = 0.0;
    for(int j=0; j<N2; j++) {
      sum = sum + A[i][j] * B[j];
    }
    C[i] = sum;
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: A[0:N1][0:N2], B[0:N2], C[0:N1])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "mv_kernel,exit,%ld,%ld\n", sizeof(double)*(N1*N2) + sizeof(double)*(N2+N1), (end - start));
}
