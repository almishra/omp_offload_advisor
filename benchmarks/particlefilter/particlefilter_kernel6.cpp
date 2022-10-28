#include "particlefilter.h"

void particlefilter_kernel6(double *CDF, double *u, double *arrayX,
                                double *arrayY, double *xj, double *yj,
                                FILE *fp)
{
#pragma omp target enter data map(to: xj[0:N], yj[0:N]) \
                                    map(to: CDF[0:N], u[0:N], arrayX[0:N], arrayY[0:N])
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
#pragma omp target exit data map(from: xj[0:N], yj[0:N]) \
                                    map(from: CDF[0:N], u[0:N], arrayX[0:N], arrayY[0:N])
}
