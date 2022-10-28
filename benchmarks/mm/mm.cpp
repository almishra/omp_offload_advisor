#include "mm.h"

int main(int argc, char **argv)
{
  double (*A)[N2] = (double (*)[N2]) malloc(sizeof(double)*N1*N2);
  double (*B)[N3] = (double (*)[N3]) malloc(sizeof(double)*N2*N3);
  double (*C)[N3] = (double (*)[N3]) malloc(sizeof(double)*N1*N3);

  // Initialize GPUs and check available memory
//#pragma omp target enter data map(alloc: A[0:N1][0:N2], B[0:N2][0:N3], C[0:N1][0:N3])
//#pragma omp target exit data map(delete: A[0:N1][0:N2], B[0:N2][0:N3], C[0:N1][0:N3])

  mm_kernel(A, B, C);
  return 0;
}
