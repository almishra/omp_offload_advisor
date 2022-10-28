#include "proxy_app.h"

int main(int argc, char **argv)
{
  float (*matA)[LB][LC][LD][LE][M][N] =
    (float (*)[LB][LC][LD][LE][M][N]) malloc(sizeof(float)*LA*LB*LC*LD*LE*M*N);
  float (*vecB)[LB][LC][LD][LE][N] =
    (float (*)[LB][LC][LD][LE][N]) malloc(sizeof(float)*LA*LB*LC*LD*LE*N);
  float (*vecC)[LB][LC][LD][N] =
    (float (*)[LB][LC][LD][N]) malloc(sizeof(float)*LA*LB*LC*LD*N);

//#pragma omp target map(matA[0:LA][0:LB][0:LC][0:LD][0:LE][0:M][0:N]) \
//  map(vecB[0:LA][0:LB][0:LC][0:LD][0:LE][0:N]) \
//  map(vecC[0:LA][0:LB][0:LC][0:LD][0:N])
//  {}

  for(int a=0; a<LA; a++)
    for(int b=0; b<LB; b++)
      for(int c=0; c<LC; c++)
        for(int d=0; d<LD; d++)
          for(int e=0;e<LE;e++)
            for(int m=0;m<M;m++)
              for(int n=0;n<N;n++)
                  matA[a][b][c][d][e][m][n] = 0.1;

  for(int a=0; a<LA; a++)
    for(int b=0; b<LB; b++)
      for(int c=0; c<LC; c++)
        for(int d=0; d<LD; d++)
          for(int e=0;e<LE;e++)
            for(int n=0;n<N;n++)
                vecB[a][b][c][d][e][n] = 2.5;

  proxy_app_kernel(matA, vecB, vecC);

  return 0;
}
