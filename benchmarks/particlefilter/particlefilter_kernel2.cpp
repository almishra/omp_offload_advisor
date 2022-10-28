#include "particlefilter.h"

double particlefilter_kernel2(double *weights, FILE *fp)
{
  double sumWeights = 0;

#pragma omp target enter data map(to: sumWeights) map(to:weights[0:N])
#pragma omp target teams distribute parallel for reduction(+:sumWeights) 
  for(int i=0; i<N; i++) {
    sumWeights += weights[i];
  }
#pragma omp target exit data map(from: sumWeights) map(from:weights[0:N])

  return sumWeights;
}
