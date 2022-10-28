#ifndef __COVARIANCE_H__
#define __COVARIANCE_H__

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <omp.h>
#include <string>
#include <math.h>

#ifndef N
#define N 1000
#endif

long get_time();
void init(double *X);
 
/* Kernels for calculating the Covariance */
double covariance_kernel1(double *X, FILE *fp);
double covariance_kernel2(double *X, double *Y, double meanX, double meanY, FILE *fp);

#endif
