#include "nn.h"

long get_time()
{
  struct timeval  tv;
  gettimeofday(&tv, NULL);
  return (long)(tv.tv_sec * 1000000 + tv.tv_usec);
}

/**
 * This program finds the k-nearest neighbors
 * REC_WINDOW should be arbitrarily assigned at compile time; 
 * A larger value would allow more work for the threads
 */
int main(int argc, char* argv[]) {
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

  char names[42][10] = { "ALBERTO", "BERYL", "CHRIS", "DEBBY", "ERNESTO",
                         "FLORENCE", "GORDON", "HELENE", "ISAAC", "JOYCE",
                         "KIRK", "LESLIE", "MICHAEL", "NADINE", "OSCAR",
                         "PATTY", "RAFAEL", "SANDY", "TONY", "VALERIE",
                         "WILLIAM", "ALEX", "BONNIE", "COLIN", "DANIELLE",
                         "EARL", "FIONA", "GASTON", "HERMINE", "IAN",
                         "JULIA", "KARL", "LISA", "MARTIN", "NICOLE",
                         "OWEN", "PAULA", "RICHARD", "SHARY", "TOBIAS",
                         "VIRGINIE", "WALTER" };
  int hours[4] = { 0, 6, 12, 18 };

  struct neighbor neighbors[K];
  //Initialize list of nearest neighbors to very large dist
  for(int j = 0 ; j < K ; j++ ) {
    neighbors[j].entry[0] = '\0';
    neighbors[j].dist = OPEN;
  }

  double z[REC_WINDOW];

  // Create random records
  double lat[REC_WINDOW];
  double lon[REC_WINDOW];
  int year[REC_WINDOW];
  int month[REC_WINDOW];
  int date[REC_WINDOW];
  int hour[REC_WINDOW];
  int num[REC_WINDOW];
  int speed[REC_WINDOW];
  int press[REC_WINDOW];
  char *name[REC_WINDOW];

#ifdef DEBUG                                                                    
  srand(0);                                                                     
#else                                                                           
  srand(time(NULL));                                                            
#endif

  for (int i = 0; i < REC_WINDOW; i++) {
    year[i] = 1950 + rand() % 55;
    month[i] = 1 + rand() % 12;
    date[i] = 1 + rand() % 28;
    hour[i] = hours[rand() % 4];
    num[i] = 1 + rand() % 28;
    name[i] = names[rand() % 42];
    lat[i] = ((double)(7 + rand() % 63)) + ((double)rand() / (double)0x7fffffff);
    lon[i] = ((double)(rand() % 358)) + ((double)rand() / (double)0x7fffffff);
    speed[i] = 10 + rand() % 155;
    press[i] = rand() % 900;
  }

  // CPU Execution
  nn_kernel(z, lat, lon, fp);

  for(int i = 0 ; i < REC_WINDOW ; i++ ) {
    double max_dist = -1;
    int max_idx = 0;
    for(int j = 0 ; j < K ; j++ ) {
      if( neighbors[j].dist > max_dist ) {
        max_dist = neighbors[j].dist;
        max_idx = j;
      }
    }
    if(z[i] < neighbors[max_idx].dist) {
      sprintf(neighbors[max_idx].entry, 
              "%4d %2d %2d %2d %2d %-9s %5.1f %5.1f %4d %4d", year[i], 
              month[i], date[i], hour[i], num[i], name[i], lat[i], lon[i], 
              speed[i], press[i]);
      neighbors[max_idx].dist = sqrt(z[i]);
    }
  }

#ifdef DEBUG
  fprintf(stderr, "The %d nearest neighbors are:\n", K);
  for(int j = 0 ; j < K ; j++ ) {
    if( !(neighbors[j].dist == OPEN) )
      fprintf(stderr, "%s --> %f\n", neighbors[j].entry, neighbors[j].dist);
  }
#endif

  return 0;
}

