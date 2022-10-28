#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <omp.h>
#include <sys/time.h>
#include <unistd.h>

#ifndef LA
#define LA 10
#endif
#ifndef LB
#define LB 10
#endif
#ifndef LC
#define LC 10
#endif
#ifndef LD
#define LD 10
#endif
#ifndef LE
#define LE 3
#endif
#ifndef M
#define M 3
#endif
#ifndef N
#define N 3
#endif
#ifndef COUNT
#define COUNT 100
#endif

void proxy_app_kernel(float (*matrixA)[LB][LC][LD][LE][M][N],
                      float (*vectorB)[LB][LC][LD][LE][N], 
                      float (*vectorC)[LB][LC][LD][N]);
