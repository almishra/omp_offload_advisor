#include "particlefilter.h"

void particlefilter_kernel4(double *arrayX, double *arrayY, double *weights,
                                double &xe, double &ye, FILE *fp)
{
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: arrayX[0:N], arrayY[0:N], weights[0:N]) \
                                    map(to: xe, ye)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "particle_kernel4_data,enter,%ld,%ld\n", 3*sizeof(double)*N + 2*sizeof(double),
          (end-start));
#pragma omp target teams distribute parallel for reduction(+:xe, ye)
  for(int i=0; i<N; i++) {
    xe += arrayX[i] * weights[i];
    ye += arrayY[i] * weights[i];
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: xe, ye)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "particle_kernel4_data,exit,%ld,%ld\n", 2*sizeof(double),
          (end-start));
}
