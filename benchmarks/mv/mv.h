#ifndef __MV_HEADER__
#define __MV_HEADER__

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <omp.h>
#include <string> 

#ifndef N1
#define N1 10
#endif

#ifndef N2
#define N2 10
#endif

long get_time();

/* Kernels for Matrix-Vector multiplication */
void mv_kernel(double (*A)[N2], double (*B), double (*C), FILE *fp);

#endif
