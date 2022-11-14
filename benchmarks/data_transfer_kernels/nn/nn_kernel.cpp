#include "nn.h"

void nn_kernel(double *z, double *lat, double *lon, FILE *fp)
{
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: z[0:REC_WINDOW]) \
                   map(to: lat[0:REC_WINDOW], lon[0:REC_WINDOW])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "nn_kernel,enter,%ld,%ld\n", sizeof(double)*REC_WINDOW*3,
        (end - start));
#pragma omp target teams distribute parallel for
  for (int i = 0; i < REC_WINDOW; i++) {
    z[i] = (lat[i] - TARGET_LAT) * (lat[i] - TARGET_LAT) +
           (lon[i] - TARGET_LON) * (lon[i] - TARGET_LON);
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: z[0:REC_WINDOW]) \
                   map(from: lat[0:REC_WINDOW], lon[0:REC_WINDOW])
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "nn_kernel,exit,%ld,%ld\n", sizeof(double)*REC_WINDOW*3,
        (end - start));
}
