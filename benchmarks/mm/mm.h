#ifndef __MM_HEADER__
#define __MM_HEADER__

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <omp.h>
#include <string> 

#ifndef N1
#define N1 100
#endif

#ifndef N2
#define N2 200
#endif

#ifndef N3
#define N3 300
#endif

void mm_kernel(double (*A)[N2], double (*B)[N3], double (*C)[N3], FILE *fp);

#endif
