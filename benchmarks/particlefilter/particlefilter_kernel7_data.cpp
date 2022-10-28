#include "particlefilter.h"

void particlefilter_kernel7(double *weights, double *arrayX, double *arrayY,
                                double *xj, double *yj, FILE *fp)
{
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: xj[0:N], yj[0:N]) \
                              map(alloc: arrayX[0:N], arrayY[0:N], weights[0:N])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "particle_kernel6,enter,%ld,%ld\n", sizeof(double)*N*2,
          (end-start));
#pragma omp target teams distribute parallel for
  for(int i=0; i<N; i++) {
    arrayX[i] = xj[i];
    arrayY[i] = yj[i];
    weights[i] = 1.0 / N;
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: arrayX[0:N], arrayY[0:N], weights[0:N])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "particle_kernel6,enter,%ld,%ld\n", sizeof(double)*N*3,
          (end-start));
}
