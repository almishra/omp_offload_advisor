#include "mm.h"

void mm_kernel(double (*A)[N2],
                   double (*B)[N3],
                   double (*C)[N3],
                   FILE *fp)
{
  struct timeval  tv1, tv2;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: A[0:N1][0:N2], B[0:N2][0:N3]) \
                              map(alloc: C[0:N1][0:N3]) 
  gettimeofday(&tv2, NULL);
  long start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  long end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "mm_kernel_data,enter,%ld,%ld\n", sizeof(double)*(N1*N2 + N2*N3),(end - start));
#pragma omp target teams distribute parallel for collapse(2)
  for(int i=0; i<N1; i++) {
    for(int j=0; j<N3; j++) {
      double sum = 0.0;
      for (int k=0; k<N2; k++)
        sum = sum + A[i][k] * B[k][j];
      C[i][j] = sum;
    }
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: C[0:N1][0:N3]) 
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "mm_kernel_data,exit,%ld,%ld\n", sizeof(double)*(N3*N1),(end - start));
}
