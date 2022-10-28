#include "particlefilter.h"

long get_time()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long)(tv.tv_sec * 1000000 + tv.tv_usec);
}

// Returns the number of seconds elapsed between the two specified times
//float elapsed_time(long long start_time, long long end_time) {
//  return (float) (end_time - start_time) / (1000 * 1000);
//}
/** 
 * Takes in a double and returns an integer that approximates to that double
 * @return if the mantissa < .5 => return value < input value; else return value > input value
 */
double roundDouble(double value){
  int newValue = (int)(value);
  if(value - newValue < .5)
    return newValue;
  else
    return newValue++;
}
/**
 * Set values of the 3D array to a newValue if that value is equal to the testValue
 * @param testValue The value to be replaced
 * @param newValue The value to replace testValue with
 * @param array3D The image vector
 * @param dimX The x dimension of the frame
 * @param dimY The y dimension of the frame
 * @param dimZ The number of frames
 */
void setIf(int testValue, int newValue, int *array3D, int *dimX, int *dimY, int *dimZ){
  int x, y, z;
  for(x = 0; x < *dimX; x++){
    for(y = 0; y < *dimY; y++){
      for(z = 0; z < *dimZ; z++){
        if(array3D[x * *dimY * *dimZ+y * *dimZ + z] == testValue)
          array3D[x * *dimY * *dimZ + y * *dimZ + z] = newValue;
      }
    }
  }
}
/**
 * Generates a uniformly distributed random number using the provided seed and GCC's settings for the Linear Congruential Generator (LCG)
 * @see http://en.wikipedia.org/wiki/Linear_congruential_generator
 * @note This function is thread-safe
 * @param seed The seed array
 * @param index The specific index of the seed to be advanced
 * @return a uniformly distributed number [0, 1)
 */
double randu(int *seed, int index)
{
  int num = A*seed[index] + C;
  seed[index] = num % M;
  return fabs(seed[index]/((double) M));
}
/**
 * Generates a normally distributed random number using the Box-Muller transformation
 * @note This function is thread-safe
 * @param seed The seed array
 * @param index The specific index of the seed to be advanced
 * @return a double representing random number generated using the Box-Muller algorithm
 * @see http://en.wikipedia.org/wiki/Normal_distribution, section computing value for normal random distribution
 */
double randn(int * seed, int index){
  /*Box-Muller algorithm*/
  double u = randu(seed, index);
  double v = randu(seed, index);
  double cosine = cos(2*PI*v);
  double rt = -2*log(u);
  return sqrt(rt)*cosine;
}
/**
 * Sets values of 3D matrix using randomly generated numbers from a normal distribution
 * @param array3D The video to be modified
 * @param dimX The x dimension of the frame
 * @param dimY The y dimension of the frame
 * @param dimZ The number of frames
 * @param seed The seed array
 */
void addNoise(int * array3D, int * dimX, int * dimY, int * dimZ, int * seed){
  int x, y, z;
  for(x = 0; x < *dimX; x++){
    for(y = 0; y < *dimY; y++){
      for(z = 0; z < *dimZ; z++){
        array3D[x * *dimY * *dimZ + y * *dimZ + z] = array3D[x * *dimY * *dimZ + y * *dimZ + z] + (int)(5*randn(seed, 0));
      }
    }
  }
}
/**
 * Fills a radius x radius matrix representing the disk
 * @param disk The pointer to the disk to be made
 * @param radius  The radius of the disk to be made
 */
void strelDisk(int * disk, int radius)
{
  int diameter = radius*2 - 1;
  int x, y;
  for(x = 0; x < diameter; x++){
    for(y = 0; y < diameter; y++){
      double distance = sqrt(pow((double)(x-radius+1),2) + pow((double)(y-radius+1),2));
      if(distance < radius)
        disk[x*diameter + y] = 1;
    }
  }
}
/**
 * Dilates the provided video
 * @param matrix The video to be dilated
 * @param posX The x location of the pixel to be dilated
 * @param posY The y location of the pixel to be dilated
 * @param poxZ The z location of the pixel to be dilated
 * @param dimX The x dimension of the frame
 * @param dimY The y dimension of the frame
 * @param dimZ The number of frames
 * @param error The error radius
 */
void dilate_matrix(int * matrix, int posX, int posY, int posZ, int dimX, int dimY, int dimZ, int error)
{
  int startX = posX - error;
  while(startX < 0)
    startX++;
  int startY = posY - error;
  while(startY < 0)
    startY++;
  int endX = posX + error;
  while(endX > dimX)
    endX--;
  int endY = posY + error;
  while(endY > dimY)
    endY--;
  int x,y;
  for(x = startX; x < endX; x++){
    for(y = startY; y < endY; y++){
      double distance = sqrt( pow((double)(x-posX),2) + pow((double)(y-posY),2) );
      if(distance < error)
        matrix[x*dimY*dimZ + y*dimZ + posZ] = 1;
    }
  }
}

/**
 * Dilates the target matrix using the radius as a guide
 * @param matrix The reference matrix
 * @param dimX The x dimension of the video
 * @param dimY The y dimension of the video
 * @param dimZ The z dimension of the video
 * @param error The error radius to be dilated
 * @param newMatrix The target matrix
 */
void imdilate_disk(int * matrix, int dimX, int dimY, int dimZ, int error, int * newMatrix)
{
  int x, y, z;
  for(z = 0; z < dimZ; z++){
    for(x = 0; x < dimX; x++){
      for(y = 0; y < dimY; y++){
        if(matrix[x*dimY*dimZ + y*dimZ + z] == 1){
          dilate_matrix(newMatrix, x, y, z, dimX, dimY, dimZ, error);
        }
      }
    }
  }
}
/**
 * Fills a 2D array describing the offsets of the disk object
 * @param se The disk object
 * @param numOnes The number of ones in the disk
 * @param neighbors The array that will contain the offsets
 * @param radius The radius used for dilation
 */
void getneighbors(int * se, int numOnes, double * neighbors, int radius)
{
  int x, y;
  int neighY = 0;
  int center = radius - 1;
  int diameter = radius*2 -1;
  for(x = 0; x < diameter; x++) {
    for(y = 0; y < diameter; y++) {
      if(se[x*diameter + y]) {
        neighbors[neighY*2] = (int)(y - center);
        neighbors[neighY*2 + 1] = (int)(x - center);
        neighY++;
      }
    }
  }
}
/**
 * The synthetic video sequence we will work with here is composed of a
 * single moving object, circular in shape (fixed radius)
 * The motion here is a linear motion
 * the foreground intensity and the backgrounf intensity is known
 * the image is corrupted with zero mean Gaussian noise
 * @param I The video itself
 * @param IszX The x dimension of the video
 * @param IszY The y dimension of the video
 * @param Nfr The number of frames of the video
 * @param seed The seed array used for number generation
 */
void videoSequence(int * I, int IszX, int IszY, int Nfr, int * seed)
{
  int k;
  int max_size = IszX*IszY*Nfr;
  /*get object centers*/
  int x0 = (int)roundDouble(IszY/2.0);
  int y0 = (int)roundDouble(IszX/2.0);
  I[x0 *IszY *Nfr + y0 * Nfr  + 0] = 1;

  /*move point*/
  int xk, yk, pos;
  for(k = 1; k < Nfr; k++){
    xk = abs(x0 + (k-1));
    yk = abs(y0 - 2*(k-1));
    pos = yk * IszY * Nfr + xk *Nfr + k;
    if(pos >= max_size)
      pos = 0;
    I[pos] = 1;
  }

  /*dilate matrix*/
  int * newMatrix = (int *)malloc(sizeof(int)*IszX*IszY*Nfr);
  imdilate_disk(I, IszX, IszY, Nfr, 5, newMatrix);
  int x, y;
  for(x = 0; x < IszX; x++){
    for(y = 0; y < IszY; y++){
      for(k = 0; k < Nfr; k++){
        I[x*IszY*Nfr + y*Nfr + k] = newMatrix[x*IszY*Nfr + y*Nfr + k];
      }
    }
  }
  free(newMatrix);

  /*define background, add noise*/
  setIf(0, 100, I, &IszX, &IszY, &Nfr);
  setIf(1, 228, I, &IszX, &IszY, &Nfr);
  /*add noise*/
  addNoise(I, &IszX, &IszY, &Nfr, seed);
}

void init(double *arrayX, double *arrayY, double *weights, int *I, int *seed, 
    int countOnes, double *objxy, int IszX, int IszY, int Nfr, int k)
{
  int max_size = IszX * IszY * Nfr;
  int ind[countOnes*N];
  double likelihood[N];

  // apply motion model
  // draws sample from motion model (random walk). The only prior information
  // is that the object moves 2x as fast as in the y direction
  for(int x = 0; x < N; x++) {
    arrayX[x] += 1 + 5*randn(seed, x);
    arrayY[x] += -2 + 2*randn(seed, x);
  }

  // particle filter likelihood
  for(int x = 0; x < N; x++) {
    // compute the likelihood: remember our assumption is that you know
    // foreground and the background image intensity distribution.
    // Notice that we consider here a likelihood ratio, instead of
    // p(z|x). It is possible in this case. why? a hometask for you.		
    // calc ind
    for(int y = 0; y < countOnes; y++) {
      int indX = roundDouble(arrayX[x]) + objxy[y*2 + 1];
      int indY = roundDouble(arrayY[x]) + objxy[y*2];
      ind[x*countOnes + y] = fabs(indX*IszY*Nfr + indY*Nfr + k);
      if(ind[x*countOnes + y] >= max_size)
        ind[x*countOnes + y] = 0;
    }
    likelihood[x] = 0;
    for(int y = 0; y < countOnes; y++)
      likelihood[x] += (pow((I[ind[x*countOnes + y]] - 100),2) - pow((I[ind[x*countOnes + y]]-228),2))/50.0;
    likelihood[x] = likelihood[x]/((double) countOnes);
  }

  // update & normalize weights
  // using equation (63) of Arulampalam Tutorial
  for(int x = 0; x < N; x++){
    weights[x] = weights[x] * exp(likelihood[x]);
  }
}

void resample(double *CDF, double *weights) 
{
  //resampling
  CDF[0] = weights[0];
  for(int x = 1; x < N; x++){
    CDF[x] = weights[x] + CDF[x-1];
  }
}

/**
 * The implementation of the particle filter using OpenMP for many frames
 * @see http://openmp.org/wp/
 * @note This function is designed to work with a video of several frames. In addition, it references a provided MATLAB function which takes the video, the objxy matrix and the x and y arrays as arguments and returns the likelihoods
 * @param I The video to be run
 * @param IszX The x dimension of the video
 * @param IszY The y dimension of the video
 * @param Nfr The number of frames
 * @param seed The seed array used for random number generation
 * @param N The number of particles to be used
 */
void particleFilter(int *I, int IszX, int IszY, int Nfr, int * seed, FILE *fp) 
{
  //original particle centroid
  double xe = roundDouble(IszY/2.0);
  double ye = roundDouble(IszX/2.0);

  //expected object locations, compared to center
  int radius = 5;
  int diameter = radius*2 - 1;
  int disk[diameter*diameter];
  strelDisk(disk, radius);
  int countOnes = 0;

  for(int x = 0; x < diameter; x++) {
    for(int y = 0; y < diameter; y++) {
      if(disk[x*diameter + y] == 1)
        countOnes++;
    }
  }

  double objxy[countOnes*2];
  getneighbors(disk, countOnes, objxy, radius);

  double weights[N];
  double arrayX[N];
  double arrayY[N];
  double xj[N];
  double yj[N];
  double CDF[N];
  double u[N];
  double u1 = (1/((double)(N)))*randu(seed, 0);

  //CPU execution
  particlefilter_kernel1(weights, arrayX, arrayY, xe, ye, fp);
  for(int k = 1; k < 1/*Nfr*/; k++) {
    init(arrayX, arrayY, weights, I, seed, countOnes, objxy, IszX, IszY, Nfr, k);
    double sumWeights = particlefilter_kernel2(weights, fp);
    particlefilter_kernel3(weights, sumWeights, fp);
    xe = 0; ye = 0;
    particlefilter_kernel4(arrayX, arrayY, weights, xe, ye, fp);
    resample(CDF, weights);
    particlefilter_kernel5(u, u1, fp);
    particlefilter_kernel6(CDF, u, arrayX, arrayY, xj, yj, fp);
    particlefilter_kernel7(weights, arrayX, arrayY, xj, yj, fp);
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


  int IszX = 128, IszY = 128, Nfr = 10;
  //establish seed
  int *seed = (int *)malloc(sizeof(int)*N);
  for(int i = 0; i < N; i++)
    seed[i] = time(0) * i;
  //malloc matrix
  int *I = (int *)malloc(sizeof(int)*IszX*IszY*Nfr);

  //call video sequence
  videoSequence(I, IszX, IszY, Nfr, seed);

  //call particle filter
  particleFilter(I, IszX, IszY, Nfr, seed, fp);

  free(seed);
  free(I);
  return 0;
}
