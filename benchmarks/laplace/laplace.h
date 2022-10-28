#ifndef __LAPLACE_H__
#define __LAPLACE_H__
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string>

#ifndef M
#define M 100
#endif

#ifndef N
#define N 100
#endif

#define MAX_ITER 5
#define TOL 0.000001

void initialize(double alpha, double (*A)[N], double (*A1)[N], double (*A2)[N], 
                double (*A3)[N], double (*A4)[N], double (*A5)[N]);
long get_time();
double laplace_kernel1(double (*A)[N], double (*Anew)[N], double err, FILE *fp);
void laplace_kernel2(double (*A)[N], double (*Anew)[N], FILE *fp);
#endif
