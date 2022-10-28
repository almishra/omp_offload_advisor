#include "covariance.h"

double covariance_kernel2(double *X, double *Y, double meanX, double meanY, FILE *fp)
{
  double sum = 0;
#pragma omp target enter data map(to: X[0:N], Y[0:N], meanX, meanY, sum)
#pragma omp target teams distribute parallel for reduction(+: sum)
  for (int i=0; i<N; i++) {
    sum += (X[i] - meanX) * (Y[i] - meanY);
  }
#pragma omp target exit data map(from: X[0:N], Y[0:N], meanX, meanY, sum)

  double cov = sum / (double)(N-1);

  return cov;
}
