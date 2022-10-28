#ifndef __NN_HEADER__
#define __NN_HEADER__

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <omp.h>
#include <string>
#include <math.h>

#define REC_LENGTH 49	// size of a record in db
#define OPEN 10000	// initial value of nearest neighbors

#ifndef REC_WINDOW
//#define REC_WINDOW 500000	// number of records to read at a time
#define REC_WINDOW 50000000 // number of records to read at a time
#endif

#define K 10
#define TARGET_LAT 30
#define TARGET_LON 90

#define MIN(a,b) ((a<b) ? a : b)
#define MAX(a,b) ((a<b) ? b : a)

struct neighbor {
    char entry[REC_LENGTH];
    double dist;
};

long get_time();
void nn_kernel(double *z, double *lat, double *lon, FILE *fp);

#endif
