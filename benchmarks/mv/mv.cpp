#include "mv.h"

long get_time()
{
  struct timeval  tv;
  gettimeofday(&tv, NULL);
  return (long)(tv.tv_sec * 1000000 + tv.tv_usec);
}

int main(int argc, char **argv)
{
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

  double (*A)[N2] = (double (*)[N2]) malloc(sizeof(double)*N1*N2);
  double *B = (double (*)) malloc(sizeof(double)*N2);
  double *C = (double (*)) malloc(sizeof(double)*N1);

  // Initialize GPUs and check available memory
//#pragma omp target enter data map(alloc: A[0:N1][0:N2], B[0:N2], \
//                                         C[0:N1])
//#pragma omp target exit data map(delete: A[0:N1][0:N2], B[0:N2], \
//                                         C[0:N1])

  mv_kernel(A, B, C, fp);

  return 0;
}
