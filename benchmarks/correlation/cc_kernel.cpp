#include "cc.h"

double cc_kernel(double *X, double *Y, FILE *fp)
{
  double sum_X = 0, sum_Y = 0, sum_XY = 0;
  double squareSum_X = 0, squareSum_Y = 0;

#pragma omp target enter data map(to: X[0:N], Y[0:N]) \
                              map(to: sum_X, sum_Y, sum_XY, squareSum_X, squareSum_Y)
#pragma omp target teams distribute parallel for reduction(+: sum_X, sum_Y, sum_XY, squareSum_X, squareSum_Y)
  for (int i=0; i<N; i++) {
    sum_X += X[i];
    sum_Y += Y[i];
    sum_XY += X[i] * Y[i];
    squareSum_X += X[i] * X[i];
    squareSum_Y += Y[i] * Y[i];
  }
#pragma omp target exit data map(from: X[0:N], Y[0:N]) \
                             map(from: sum_X, sum_Y, sum_XY, squareSum_X, squareSum_Y)

  double corr = (double)(N * sum_XY - sum_X * sum_Y) / 
                sqrt((N * squareSum_X - sum_X * sum_X) * 
                     (N * squareSum_Y - sum_Y * sum_Y));
  return corr;
}
