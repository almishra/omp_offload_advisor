#include "gauss.h"

float kernel(double (*mat)[N], FILE *fp)
{
  float diff = 0;
#pragma omp target enter data map(to: mat[0:N][0:N], diff)
#pragma omp target teams distribute parallel for reduction(+:diff) collapse(2)
  for (int i = 1; i < N-1; i++) {
    for (int j = 1; j < N-1; j++) {
      const float temp = mat[i][j];
      mat[i][j] = 0.2f * (
          mat[i][j]
          + mat[i][j-1]
          + mat[i-1][j]
          + mat[i][j+1]
          + mat[i+1][j]
          );

      float x = mat[i][j] - temp;
      if(x < 0) x *= -1;
      diff += x;
    }
  }
#pragma omp target exit data map(from: mat[0:N][0:N], diff)

  return diff;
}

