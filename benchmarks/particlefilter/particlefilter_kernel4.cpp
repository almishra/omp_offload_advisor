#include "particlefilter.h"

void particlefilter_kernel4(double *arrayX, double *arrayY, double *weights,
                                double &xe, double &ye, FILE *fp)
{
#pragma omp target enter data map(to: arrayX[0:N], arrayY[0:N], weights[0:N]) \
                                    map(to: xe, ye)
#pragma omp target teams distribute parallel for reduction(+:xe, ye)
  for(int i=0; i<N; i++) {
    xe += arrayX[i] * weights[i];
    ye += arrayY[i] * weights[i];
  }
#pragma omp target exit data map(from: arrayX[0:N], arrayY[0:N], weights[0:N]) \
                                    map(from: xe, ye)
}
