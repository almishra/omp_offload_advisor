#include <stdio.h> 

int main() {
  int nDevices;

  cudaGetDeviceCount(&nDevices);
  if(nDevices < 1) {
    printf("No device found\n");
    return -1;
  }

  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, 0);
  printf("%d%d\n", prop.major, prop.minor);

  return 0;
}
