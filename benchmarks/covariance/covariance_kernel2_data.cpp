#include "covariance.h"

double covariance_kernel2(double *X, double *Y, double meanX, double meanY, FILE *fp)
{
  double sum = 0;
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: X[0:N], Y[0:N], meanX, meanY, sum)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "cov_kernel2_data,enter,%ld,%ld\n", sizeof(double)*N*2 + sizeof(double)*3,
          (end - start));
#pragma omp target teams distribute parallel for reduction(+: sum)
  for (int i=0; i<N; i++) {
    sum += (X[i] - meanX) * (Y[i] - meanY);
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: sum)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "cov_kernel2_data,exit,%ld,%ld\n", sizeof(double),
          (end - start));

  double cov = sum / (double)(N-1);

  return cov;
}
