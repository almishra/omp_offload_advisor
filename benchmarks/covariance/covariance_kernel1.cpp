#include "covariance.h"

double covariance_kernel1(double *X, FILE *fp)
{
  double sum = 0;
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: X[0:N], sum)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "cov_kernel1,enter,%ld,%ld\n", sizeof(double)*N + sizeof(double),
          (end - start));
#pragma omp target teams distribute parallel for reduction(+: sum)
  for (int i=0; i<N; i++) {
    sum += X[i];
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: X[0:N], sum)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "cov_kernel1,exit,%ld,%ld\n", sizeof(double)*N + sizeof(double),
          (end - start));

  double mean = sum / (double)N;

  return mean;
}
