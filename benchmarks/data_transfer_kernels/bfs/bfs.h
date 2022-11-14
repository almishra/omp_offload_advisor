#ifndef __BFS_HEADER__
#define __BFS_HEADER__
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <vector>

#ifndef N
#define N 10
#endif

long get_time();

//Structure to hold the graph information
typedef struct {
  int source;
  long totalEdges;
} Graph;

//Structure to hold a node information
struct Node {
  int start;
  int num_edges;
};

int* CreateGraph(Node *graph_nodes, bool *graph_mask, bool *updating_graph_mask,
                 bool *graph_visited, int *cost, Graph *g);
void BFSGraph(FILE *fp);

struct edge;
typedef std::vector<edge> node;                                                      
struct edge {                                                                   
  ulong dest;                                                                   
  uint weight;                                                                  
};

#define MIN_NODES 20
#define MAX_NODES ULONG_MAX
#define MIN_EDGES 2
#define MAX_INIT_EDGES 4 // Nodes will have, on average, 2*MAX_INIT_EDGES edges
#define MIN_WEIGHT 1
#define MAX_WEIGHT 10

void kernel1(Node* graph_nodes, bool *graph_mask, bool *updating_graph_mask,
             bool *graph_visited, int *graph_edges, int *cost, int totalEdges,  
             FILE *fp);
bool kernel2(bool *graph_mask, bool *updating_graph_mask, bool *graph_visited,  
             bool stop, FILE *fp);
#endif
