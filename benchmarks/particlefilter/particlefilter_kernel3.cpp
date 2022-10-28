#include "particlefilter.h"

void particlefilter_kernel3(double *weights, double sumWeights, FILE *fp)
{
#pragma omp target enter data map(to: sumWeights) \
                              map(to: weights[0:N])
#pragma omp target teams distribute parallel for
  for(int i=0; i<N; i++) {
    weights[i] = weights[i]/sumWeights;
  }
#pragma omp target exit data map(from: sumWeights) \
                              map(from: weights[0:N])

}
