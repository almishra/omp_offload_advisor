#include "bfs.h"

////////////////////////////////////////////////////////////////////////////////
// Kernel 1
////////////////////////////////////////////////////////////////////////////////
void kernel1(Node* graph_nodes, bool *graph_mask,
             bool *updating_graph_mask, bool *graph_visited,
             int *graph_edges, int *cost, int totalEdges, FILE *fp)
{
#pragma omp target enter data map(to: graph_nodes[0:N], \
                                      graph_edges[0:totalEdges], \
                                      graph_visited[0:N], cost[0:N], \
                                      updating_graph_mask[0:N],\
                                      graph_mask[0:N])
#pragma omp target teams distribute parallel for
  for(int tid = 0; tid < N; tid++ ) {
    if (graph_mask[tid] == true) { 
      graph_mask[tid]=false;
      int num = graph_nodes[tid].num_edges + graph_nodes[tid].start;
      for(int i = graph_nodes[tid].start; i < num; i++) {
        int id = graph_edges[i];
        if(!graph_visited[id]) {
          cost[id] = cost[tid]+1;
          updating_graph_mask[id] = true;
        }
      }
    }
  }
#pragma omp target exit data map(from: graph_nodes[0:N], \
                                       graph_edges[0:totalEdges], \
                                       graph_visited[0:N], cost[0:N], \
                                       updating_graph_mask[0:N],\
                                       graph_mask[0:N])
}
