#include "laplace.h"

long get_time()
{
  struct timeval  tv;
  gettimeofday(&tv, NULL);
  return (long)(tv.tv_sec * 1000000 + tv.tv_usec);
}

void initialize(double alpha, double (*A)[N], double (*A1)[N], double (*A2)[N], double (*A3)[N], double (*A4)[N], double (*A5)[N])
{
  int i, j;
  /* Initilize initial condition*/
  for (i=0; i<M; i++){
    for (j=0; j<N; j++){
      A[i][j] = (rand() % 3) * alpha;
      A1[i][j] = A[i][j];
      A2[i][j] = A[i][j];
      A3[i][j] = A[i][j];
      A4[i][j] = A[i][j];
      A5[i][j] = A[i][j];
    }
  }
}

int main(int argc, char**argv) {
  std::string output_file_name;
  if(argc > 1) {
    output_file_name = argv[1];
  } else {
    output_file_name = argv[0];
    output_file_name = output_file_name.substr(output_file_name.find_last_of("/\\")+1);
    output_file_name = output_file_name.substr(0, output_file_name.size() - 3);
    output_file_name = "output_" + output_file_name + "csv";
  }

  printf("%s\n", output_file_name.c_str());
  FILE *fp = fopen(output_file_name.c_str(), "w");

  int iter;
  double err;
  double A_cpu[M][N];
  double A_cpu_coll[M][N];
  double A_gpu_mem[M][N];
  double A_gpu_coll_mem[M][N];
  double A_gpu[M][N];
  double A_gpu_coll[M][N];
  double Anew[M][N];
  double alpha = 0.0543;
  initialize(alpha, A_cpu, A_cpu_coll, A_gpu_mem, A_gpu_coll_mem, A_gpu, A_gpu_coll);

  // CPU
  iter = 0;
  err = 1.0;
  while (err>TOL && iter<MAX_ITER) {
    err = 0.0;
    err = laplace_kernel1(A_cpu, Anew, err, fp);
    laplace_kernel2(A_cpu, Anew, fp);
    iter++;
  }

  return 0;
}
