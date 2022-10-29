#ifndef _CPS_DRIVER_H
#define _CPS_DRIVER_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <omp.h>
#include <sys/time.h>

#ifndef LX
#define LX 8
#endif
#ifndef LY
#define LY 16
#endif
#ifndef LZ
#define LZ 16
#endif
#ifndef LT
#define LT 16
#endif

void wilson_dslash_kernel(double (*chi_p)[LZ][LY][LX/2][4][3][2],
    double (*u_p_f_tst)[LT+1][LZ+1][LY+1][(LX/2)+1][4][3][3][2],
    double (*psi_p_f_tst)[LZ+2][LY+2][(LX/2)+2][4][3][2], int cb, FILE* fp);
#endif
