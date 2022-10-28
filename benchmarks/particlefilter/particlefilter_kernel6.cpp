#include "particlefilter.h"

void particlefilter_kernel6(double *CDF, double *u, double *arrayX,
                                double *arrayY, double *xj, double *yj,
                                FILE *fp)
{
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: xj[0:N], yj[0:N]) \
                                    map(to: CDF[0:N], u[0:N], arrayX[0:N], arrayY[0:N])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "particle_kernel6,enter,%ld,%ld\n", sizeof(double)*N*6,
          (end-start));
#pragma omp target teams distribute parallel for
  for(int i=0; i<N; i++) {
    int x = -1;
    for(int j=0; j<N; j++) {
      if(CDF[j] >= u[i]) {
        x = j;
        break;
      }
    }
    if(x == -1) x = N-1;

    xj[i] = arrayX[x];
    yj[i] = arrayY[x];
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: xj[0:N], yj[0:N]) \
                                    map(from: CDF[0:N], u[0:N], arrayX[0:N], arrayY[0:N])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "particle_kernel6,exit,%ld,%ld\n", sizeof(double)*N*6,
          (end-start));
}
