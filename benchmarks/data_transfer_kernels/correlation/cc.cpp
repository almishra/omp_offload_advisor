#include "cc.h"

void init(double *X)
{
  for(int i=0; i<N; i++) {
    X[i] = (rand() % 1000) / 10;
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

  double X[N];
  double Y[N];

  // Initial memory check
//#pragma omp target enter data map(to: X[0:N], Y[0:N])
//  #pragma omp parallel for
//    for (int i=0; i<N; i++);
//#pragma omp target exit data map(delete: X[0:N], Y[0:N])

  init(X);
  init(Y);

  double corr __attribute__((unused)); // Attribute set to avoid unused warning

  // CPU
  corr = cc_kernel(X, Y, fp);
#ifdef DEBUG
  printf("Correlation Coefficient (CPU)     = %lf\n", corr);
#endif


  return 0;
}
