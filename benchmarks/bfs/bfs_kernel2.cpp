#include "bfs.h"

////////////////////////////////////////////////////////////////////////////////
// Kernel 2 - GPU MEMCPY
////////////////////////////////////////////////////////////////////////////////
bool kernel2(bool *graph_mask, bool *updating_graph_mask, bool *graph_visited,
             bool stop, FILE *fp)
{
#pragma omp target enter data map(to: updating_graph_mask[0:N], \
                                      graph_mask[0:N], graph_visited[0:N], \
                                      stop)
#pragma omp target teams distribute parallel for
  for(int tid = 0; tid < N ; tid++) {
    if (updating_graph_mask[tid] == true) {
      graph_mask[tid] = true;
      graph_visited[tid] = true;
      stop = true;
      updating_graph_mask[tid] = false;
    }
  }
#pragma omp target exit data map(from: updating_graph_mask[0:N], \
                                      graph_mask[0:N], graph_visited[0:N], \
                                      stop)
  return stop;
}
