#include "gauss.h"

float kernel(double (*mat)[N], FILE *fp)
{
  float diff = 0;
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: mat[0:N][0:N], diff)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "gauss_kernel,enter,%ld,%ld\n", sizeof(double)*N*N + sizeof(double),
          (end - start));
#pragma omp target teams distribute parallel for reduction(+:diff) collapse(2)
  for (int i = 1; i < N-1; i++) {
    for (int j = 1; j < N-1; j++) {
      const float temp = mat[i][j];
      mat[i][j] = 0.2f * (
          mat[i][j]
          + mat[i][j-1]
          + mat[i-1][j]
          + mat[i][j+1]
          + mat[i+1][j]
          );

      float x = mat[i][j] - temp;
      if(x < 0) x *= -1;
      diff += x;
    }
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: mat[0:N][0:N], diff)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "gauss_kernel,exit,%ld,%ld\n", sizeof(double)*N*N + sizeof(double),
          (end - start));

  return diff;
}

