#include "particlefilter.h"

void particlefilter_kernel1(double *weights, double *arrayX, double *arrayY,
                                double xe, double ye, FILE *fp)
{
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(alloc: weights[0:N], arrayX[0:N], arrayY[0:N]) \
                              map(to: xe, ye)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "particle_kernel1_data,enter,%ld,%ld\n", sizeof(double)*2,
          (end-start));
#pragma omp target teams distribute parallel for 
  for(int i=0; i<N; i++) {
    weights[i] = 1.0 / N;
    arrayX[i] = xe;
    arrayY[i] = ye;
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: weights[0:N], arrayX[0:N], arrayY[0:N])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "particle_kernel1_data,exit,%ld,%ld\n", sizeof(double)*N*3,
          (end-start));
}
