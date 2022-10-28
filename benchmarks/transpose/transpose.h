#ifndef __TRANSPOSE_H__
#define __TRANSPOSE_H__

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <omp.h>
#include <string>
#include <math.h>

#ifndef N1
#define N1 100
#endif

#ifndef N2
#define N2 100
#endif

long get_time();
void init(double (*A)[N2]);
 
/* Kernels for calculating the Transpose of a matrix */
void transpose_kernel(double (*A)[N2], double (*B)[N1], FILE *fp);

#endif
