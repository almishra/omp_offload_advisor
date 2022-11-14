#include "particlefilter.h"

double particlefilter_kernel2(double *weights, FILE *fp)
{
  double sumWeights = 0;

  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: sumWeights) map(to:weights[0:N])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "particle_kernel2_data,enter,%ld,%ld\n", sizeof(double)*N + sizeof(double),
          (end-start));
#pragma omp target teams distribute parallel for reduction(+:sumWeights) 
  for(int i=0; i<N; i++) {
    sumWeights += weights[i];
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: sumWeights)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "particle_kernel2_data,exit,%ld,%ld\n", sizeof(double),
          (end-start));

  return sumWeights;
}
