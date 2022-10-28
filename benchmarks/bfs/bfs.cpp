#include "bfs.h"

long get_time()                                                                 
{                                                                               
  struct timeval  tv;                                                           
  gettimeofday(&tv, NULL);                                                      
  return (long)(tv.tv_sec * 1000000 + tv.tv_usec);                              
} 

////////////////////////////////////////////////////////////////////////////////
// Main Program
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv) 
{
  // Initiate GPUs and check if it has enough memory
  long total_size = 3*sizeof(bool)*N + sizeof(Node)*N + 6*sizeof(int)*N;
#ifdef DEBUG
  printf("Checking for total size %ld\n", total_size);
#endif
  char s[total_size];
#pragma omp target enter data map(to: s[0:total_size])
#pragma omp target exit data map(delete: s[0:total_size])

#ifdef DEBUG
  srand(0);
#else
  srand(time(NULL));
#endif
  std::string ofile;
  if(argc > 1) {
    ofile = argv[1];
  } else {
    ofile = argv[0];
    ofile = ofile.substr(ofile.find_last_of("/\\")+1);
    ofile = ofile.substr(0, ofile.size() - 3);
    ofile = "output_" + ofile + "csv";
  }

  printf("%s\n", ofile.c_str());
  FILE *fp = fopen(ofile.c_str(), "w");
  BFSGraph(fp);
}

////////////////////////////////////////////////////////////////////////////////
// Create Graph Funtion
// Returns Graph edges
////////////////////////////////////////////////////////////////////////////////
int* CreateGraph(Node* graph_nodes, bool *graph_mask, bool *updating_graph_mask,
                 bool *graph_visited, int *cost, Graph *g) {
  g->source = 0;

  node * graph = new node[N];
  uint numEdges;
  ulong nodeID;
  uint weight;
  uint j;

  for (long i = 0; i < N; i++ ) {
    numEdges = rand() % ( MAX_INIT_EDGES - MIN_EDGES + 1 ) + MIN_EDGES;
    for ( j = 0; j < numEdges; j++ ) {
      //      nodeID = randNode( gen );
      nodeID = rand() % numEdges;
      weight = rand() % ( MAX_WEIGHT - MIN_WEIGHT + 1 ) + MIN_WEIGHT;
      graph[i].push_back( edge() );
      graph[i].back().dest = nodeID;
      graph[i].back().weight = weight;
      graph[nodeID].push_back( edge() );
      graph[nodeID].back().dest = i;
      graph[nodeID].back().weight = weight;
    }
  }

  int start, edgeno;
  g->totalEdges = 0;
  for (long i = 0; i < N; i++ ) {
    numEdges = graph[i].size();
    start = g->totalEdges;
    edgeno = numEdges;
    g->totalEdges += numEdges;
    graph_nodes[i].start = start;
    graph_nodes[i].num_edges = edgeno;
    graph_mask[i]=false;
    updating_graph_mask[i]=false;
    graph_visited[i]=false;
    cost[i] = -1;
  }

  g->source = rand() % numEdges;
  graph_mask[g->source] = true;
  graph_visited[g->source] = true;

  int* graph_edges = (int*) malloc(sizeof(int) * g->totalEdges);

  int k = 0;
  for (long i = 0; i < N; i++ ) {
    for ( uint j = 0; j < graph[i].size(); j++ ) {
      graph_edges[k++] = graph[i][j].dest;
    }
  }

  return graph_edges;
}

////////////////////////////////////////////////////////////////////////////////
// Apply BFS on a Graph using OpenMP
////////////////////////////////////////////////////////////////////////////////
void BFSGraph(FILE *fp)
{
  // allocate memory
  Node *graph_nodes = (Node*) malloc(sizeof(Node) * N);
  bool *graph_mask = (bool*) malloc(sizeof(bool) * N);
  bool *updating_graph_mask = (bool*) malloc(sizeof(bool) * N);
  bool *graph_visited = (bool*) malloc(sizeof(bool) * N);
  int *cost = (int*) malloc( sizeof(int)*N);
  Graph *g = (Graph*) malloc(sizeof(Graph));
  int  *graph_edges = CreateGraph(graph_nodes, graph_mask, updating_graph_mask, 
                                  graph_visited, cost, g);

  cost[g->source] = 0;

  // Run on CPU 
  bool stop = true;
  while(stop) {
    kernel1(graph_nodes, graph_mask, updating_graph_mask, graph_visited,
            graph_edges, cost, g->totalEdges, fp);
    stop = kernel2(graph_mask, updating_graph_mask, graph_visited, false, fp);
  }
#ifdef DEBUG
  {
    //Store the result into a file
    std::string res_file = "result_gpu_" + std::to_string(N) + ".log";
    FILE *fpo = fopen(res_file.c_str(),"w");
    for(int i=0; i<N; i++) fprintf(fpo, "%d) cost:%d\n", i, cost[i]);
    fclose(fpo);
    printf("Result stored in %s\n", res_file.c_str());
  }
#endif

  // cleanup memory
  free(graph_nodes);
  free(graph_edges);
  free(graph_mask);
  free(updating_graph_mask);
  free(graph_visited);
  free(cost);
}
