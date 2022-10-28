#include "nn.h"

void nn_kernel(float *z, float *lat, float *lon, FILE *fp)
{
//#pragma omp target enter data map(alloc: z[0:REC_WINDOW]) \
//                   map(to: lat[0:REC_WINDOW], lon[0:REC_WINDOW])
#pragma omp target enter data map(to: z[0:REC_WINDOW]) \
                   map(to: lat[0:REC_WINDOW], lon[0:REC_WINDOW]) \
                   map(to: z[0:REC_WINDOW])
#pragma omp target teams distribute parallel for
  for (int i = 0; i < REC_WINDOW; i++) {
    z[i] = (lat[i] - TARGET_LAT) * (lat[i] - TARGET_LAT) +
           (lon[i] - TARGET_LON) * (lon[i] - TARGET_LON);
  }
#pragma omp target exit data map(from: z[0:REC_WINDOW]) \
                   map(from: lat[0:REC_WINDOW], lon[0:REC_WINDOW]) \
                   map(from: z[0:REC_WINDOW])
//#pragma omp target exit data map(from: z[0:REC_WINDOW])
}
