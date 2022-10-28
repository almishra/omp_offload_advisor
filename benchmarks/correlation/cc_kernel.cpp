#include "cc.h"

double cc_kernel(double *X, double *Y, FILE *fp)
{
  double sum_X = 0, sum_Y = 0, sum_XY = 0;
  double squareSum_X = 0, squareSum_Y = 0;
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: X[0:N], Y[0:N]) \
                              map(to: sum_X, sum_Y, sum_XY, squareSum_X, squareSum_Y)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "cc_kernel,enter,%ld,%ld\n", sizeof(double)*N + sizeof(double)*N + sizeof(double)*5 ,
          (end - start));

#pragma omp target teams distribute parallel for reduction(+: sum_X, sum_Y, sum_XY, squareSum_X, squareSum_Y)
  for (int i=0; i<N; i++) {
    sum_X += X[i];
    sum_Y += Y[i];
    sum_XY += X[i] * Y[i];
    squareSum_X += X[i] * X[i];
    squareSum_Y += Y[i] * Y[i];
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: X[0:N], Y[0:N]) \
                             map(from: sum_X, sum_Y, sum_XY, squareSum_X, squareSum_Y)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "cc_kernel,exit,%ld,%ld\n", sizeof(double)*N + sizeof(double)*N + sizeof(double)*5 ,
          (end - start));

  double corr = (double)(N * sum_XY - sum_X * sum_Y) / 
                sqrt((N * squareSum_X - sum_X * sum_X) * 
                     (N * squareSum_Y - sum_Y * sum_Y));
  return corr;
}
