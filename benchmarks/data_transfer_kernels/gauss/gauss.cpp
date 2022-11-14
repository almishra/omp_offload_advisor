#include "gauss.h"

long get_time()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long)(tv.tv_sec * 1000000 + tv.tv_usec);
}

float rand_float(const int max) {
  return ((float)rand() / (float)(RAND_MAX)) * max;
}

int get_max_rows(const int num_threads, const int n) {
  float val = (n-2) / num_threads + 2;
  int ret = (int)val;
  if(val != ret) ret++;
  return ret;
}

int main(int argc, char *argv[]) {

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

  double mat[N][N];
  double mat_coll[N][N];
  double mat_gpu[N][N];
  double mat_gpu_coll[N][N];
  double mat_gpu_mem[N][N];
  double mat_gpu_coll_mem[N][N];

  // Initialize GPUs and check available memory
//  printf("Checking for %.4lf GB\n", sizeof(double)*N*N / 1024.0 / 1024.0 / 1024.0);
//#pragma omp target enter data map(to: mat_gpu[0:N][0:N])                        
//#pragma omp target exit data map(delete: mat_gpu[0:N][0:N])                        
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++) {
      mat[i][j] = rand_float(MAX);
      mat_coll[i][j] = mat[i][j];
      mat_gpu[i][j] = mat[i][j];
      mat_gpu_coll[i][j] = mat[i][j];
      mat_gpu_mem[i][j] = mat[i][j];
      mat_gpu_coll_mem[i][j] = mat[i][j];
    }

  float diff;

  int done;
  int cnt_iter;
  const int mat_dim = N * N;

  // CPU
  done = 0;
  cnt_iter = 0;
  while (!done && (cnt_iter < MAX_ITER)) {
    diff = kernel(mat, fp);
    if (diff/mat_dim < TOL) {
      done = 1;
    }
    cnt_iter ++;
  }

  return 0;
}
