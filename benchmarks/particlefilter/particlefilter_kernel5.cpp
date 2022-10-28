#include "particlefilter.h"

void particlefilter_kernel5(double *u, double u1, FILE *fp)
{
#pragma omp target enter data map(to: u1) map(to: u[0:N])
#pragma omp target teams distribute parallel for map(to: u1) map(from: u[0:N])
  for(int i=0; i<N; i++) {
    u[i] = u1 + i/((double)(N));
  }
#pragma omp target exit data map(from: u1) map(from: u[0:N])
}
