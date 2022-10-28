#include "covariance.h"

double covariance_kernel1(double *X, FILE *fp)
{
  double sum = 0;
#pragma omp target enter data map(to: X[0:N], sum)
#pragma omp target teams distribute parallel for reduction(+: sum)
  for (int i=0; i<N; i++) {
    sum += X[i];
  }
#pragma omp target exit data map(from: X[0:N], sum)

  double mean = sum / (double)N;

  return mean;
}
