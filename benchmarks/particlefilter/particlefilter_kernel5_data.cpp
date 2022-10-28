#include "particlefilter.h"

void particlefilter_kernel5(double *u, double u1, FILE *fp)
{
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: u1) map(alloc: u[0:N])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "particle_kernel5_data,enter,%ld,%ld\n", sizeof(double),
          (end-start));
#pragma omp target teams distribute parallel for 
  for(int i=0; i<N; i++) {
    u[i] = u1 + i/((double)(N));
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: u[0:N])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "particle_kernel5_data,enter,%ld,%ld\n", sizeof(double)*N,
          (end-start));
}
