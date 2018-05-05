#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <immintrin.h>
#include <malloc.h>

int
calc(float* data, size_t NX, size_t NY, int n_steps);

float*
alloc_mem(size_t NX, size_t NY);

int
free_mem(float*);

int
process_args(int argc, char *argv[], int &n_steps, size_t &NX, size_t &NY)
{
  switch(argc) {
    case 1:
      // No arguments
      n_steps = 20;
      NX = 8192;
      NY = 8192;
      return 0;
    case 2:
      // One argument, take as n_steps
      n_steps = atoi(argv[1]);
      NX = 8192;
      NY = 8192;
      return 0;
    case 3:
      n_steps = 20;
      NX = (size_t)atoi(argv[1]);
      NY = (size_t)atoi(argv[2]);
      if (NX % 32 != 0) return 1;
      return 0;
    case 4:
      // Three arguments, take as all specified
      n_steps = atoi(argv[1]);
      NX = (size_t)atoi(argv[2]);
      NY = (size_t)atoi(argv[3]);
      if (NX % 32 != 0) return 1;
      return 0;
    default:
      std::cerr << "Usage: " << std::endl;
      std::cerr << "\t" << argv[0] << "\t" << "n_steps" << std::endl;
      std::cerr << "\t" << argv[0] << "\t" << "NX" << "\t" << "NY" << std::endl;
      std::cerr << "\t" << argv[0] << "\t" << "n_steps";
      std::cerr << "\t" << "NX" << "\t" << "NY" << std::endl;
      return 1;
  }

}

/* in microseconds (us) */
double get_elapsed_time(struct timeval *begin, struct timeval *end)
{
  return (end->tv_sec - begin->tv_sec) * 1000000
    + (end->tv_usec - begin->tv_usec);
}

int
init(float* data, size_t NX, size_t NY)
{
  size_t cx = NX/2, cy = 0; /* center of ink */
  size_t rad = (NX+NY)/8; /* radius of ink */
  for(size_t y = 0; y < NY; y++) {
    for(size_t x = 0; x < NX; x++) {
      float v;
      if (((x-cx)*(x-cx)+(y-cy)*(y-cy)) < rad*rad) {
        v = 1.0;
      } else {
        v = 0.0;
      }
      data[y*NX+x] = v;
      data[(NY+y)*NX+x] = v;
    }
  }
  return 0;
}


int  main(int argc, char *argv[])
{
  struct timeval t1, t2;
  int n_steps;
  size_t NX, NY;

  if (process_args(argc, argv, n_steps, NX, NY)) {
    return 1;
  }

  // Allocate data, 2* is for `to` and `from`
  // float* data = (float*)malloc(2 * NX * NY * sizeof(float));
  // Align to 32bit for faster loads
  //void* data_al;
  //posix_memalign(&data_al, (size_t)32, 2*NX*NY*sizeof(float));
  //float* data = (float*)data_al;

  float *data = alloc_mem(NX, NY);

  if (init(data, NX, NY)) {
    return 1;
  }

  gettimeofday(&t1, NULL);

  if (calc(data, NX, NY, n_steps)) {
    return 1;
  }

  gettimeofday(&t2, NULL);

  if (free_mem(data)) {
    return 1;
  }

  {
      double us;
      double gflops;
      int op_per_point = 5; // 4 add & 1 multiply per point

      us = get_elapsed_time(&t1, &t2);
      printf("Elapsed time: %.3lf sec\n", us/1000000.0);
      gflops = ((double)NX * NY * n_steps * op_per_point) / us / 1000.0;
      printf("Speed: %.3lf GFlops\n", gflops);
  }

  return 0;
}
