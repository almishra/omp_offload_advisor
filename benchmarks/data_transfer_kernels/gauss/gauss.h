#ifndef __GAUSS_H__
#define __GAUSS_H__

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <omp.h>

#define MAX_ITER 100
#define MAX 100
#define TOL 0.000001

#ifndef N
#define N 128
#endif

// Generate a random float number with the maximum value of max
float rand_float(const int max);

// Calculates how many rows are given, as maximum, to each thread
int get_max_rows(const int num_threads, const int n);

long get_time();

float kernel(double (*mat)[N], FILE *fp);
#endif
