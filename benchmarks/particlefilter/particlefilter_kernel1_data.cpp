#include "particlefilter.h"

void particlefilter_kernel1(double *weights, double *arrayX, double *arrayY,
                                double xe, double ye, FILE *fp)
{
#pragma omp target enter data map(alloc: weights[0:N], arrayX[0:N], arrayY[0:N]) \
                              map(to: xe, ye)
#pragma omp target teams distribute parallel for 
  for(int i=0; i<N; i++) {
    weights[i] = 1.0 / N;
    arrayX[i] = xe;
    arrayY[i] = ye;
  }
#pragma omp target exit data map(from: weights[0:N], arrayX[0:N], arrayY[0:N])
}
