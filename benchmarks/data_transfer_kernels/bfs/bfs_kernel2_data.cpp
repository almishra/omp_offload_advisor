#include "bfs.h"

////////////////////////////////////////////////////////////////////////////////
// Kernel 2 - GPU MEMCPY
////////////////////////////////////////////////////////////////////////////////
bool kernel2(bool *graph_mask, bool *updating_graph_mask, bool *graph_visited,
             bool stop, FILE *fp)
{
  struct timeval  tv1, tv2;
  long start, end;
  gettimeofday(&tv1, NULL);
#pragma omp target enter data map(to: updating_graph_mask[0:N], \
                                      graph_mask[0:N], graph_visited[0:N], \
                                      stop)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "bfs_kernel2_data,enter,%ld,%ld\n",  sizeof(bool)*N + sizeof(bool)*N + sizeof(bool)*N + sizeof(bool) ,
          (end - start));
#pragma omp target teams distribute parallel for
  for(int tid = 0; tid < N ; tid++) {
    if (updating_graph_mask[tid] == true) {
      graph_mask[tid] = true;
      graph_visited[tid] = true;
      stop = true;
      updating_graph_mask[tid] = false;
    }
  }
  gettimeofday(&tv1, NULL);
#pragma omp target exit data map(from: updating_graph_mask[0:N], \
                                       graph_mask[0:N], graph_visited[0:N], \
                                       stop)
  gettimeofday(&tv2, NULL);
  start = (long)(tv1.tv_sec * 1000000 + tv1.tv_usec);
  end = (long)(tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp, "bfs_kernel2_data,exit,%ld,%ld\n",  sizeof(bool)*N + sizeof(bool)*N + sizeof(bool)*N + sizeof(bool) ,
          (end - start));
  return stop;
}
