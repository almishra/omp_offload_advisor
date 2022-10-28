#include "transpose.h"

long get_time()
{
  struct timeval  tv;
  gettimeofday(&tv, NULL);
  return (long)(tv.tv_sec * 1000000 + tv.tv_usec);
}

void init(double (*A)[N2])
{
  for(int i=0; i<N1; i++) {
    for(int j=0; j<N2; j++) {
      A[i][j] = (rand() % 1000) / 100.0;
    }
  }
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

  double A[N1][N2];
  double B[N2][N1];

  // Initial memory check
//#pragma omp target enter data map(to: A[0:N1][0:N2], B[0:N2][0:N1])
//  #pragma omp parallel for
//    for (int i=0; i<N1; i++);
//#pragma omp target exit data map(delete: A[0:N1][0:N2], B[0:N2][0:N1])

  init(A);
#ifdef DEBUG
  printf("Matrix A\n");
  for(int i=0; i<N1; i++) {
    for(int j=0; j<N2; j++) {
      printf("%2.2lf ", A[i][j]);
    }
    printf("\n");
  }
  printf("\n");
#endif

  // CPU
  transpose_kernel(A, B, fp);
#ifdef DEBUG
  printf("Matrix B (CPU)\n");
  for(int i=0; i<N2; i++) {
    for(int j=0; j<N1; j++) {
      printf("%2.2lf ", B[i][j]);
    }
    printf("\n");
  }
  printf("\n");
#endif

  return 0;
}
