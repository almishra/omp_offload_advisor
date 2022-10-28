#include "particlefilter.h"

void particlefilter_kernel7(double *weights, double *arrayX, double *arrayY,
                                double *xj, double *yj, FILE *fp)
{
#pragma omp target enter data map(to: xj[0:N], yj[0:N]) \
                              map(alloc: arrayX[0:N], arrayY[0:N], weights[0:N])
#pragma omp target teams distribute parallel for
  for(int i=0; i<N; i++) {
    arrayX[i] = xj[i];
    arrayY[i] = yj[i];
    weights[i] = 1.0 / N;
  }
#pragma omp target exit data map(from: arrayX[0:N], arrayY[0:N], weights[0:N])
}
