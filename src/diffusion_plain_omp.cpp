#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

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

void init(float*** data, size_t NX, size_t NY)
{
  size_t cx = (size_t)NX/2, cy = 0; /* center of ink */
  size_t rad = (size_t)(NX+NY)/8; /* radius of ink */

  for(size_t y = 0; y < NY; y++) {
    for(size_t x = 0; x < NX; x++) {
      float v = 0.0;
      if (((x-cx)*(x-cx)+(y-cy)*(y-cy)) < rad*rad) {
        v = 1.0;
      }
      data[0][y][x] = v;
      data[1][y][x] = v;
    }
  }
  return;
}

void calc(float*** data, size_t NX, size_t NY, int n_steps)
{
  size_t to;
  for (int t = 0; t < n_steps; t++) {
    size_t from = t%2;
    to = (t+1)%2;

#if 0
    printf("step %d\n", t);
    fflush(0);
#endif
#pragma omp parallel for
    for (size_t y = 1; y < NY-1; y++) {
      for (size_t x = 1; x < NX-1; x++) {
        data[to][y][x] = 0.2 * (data[from][y][x]
				+ data[from][y][x-1]
				+ data[from][y][x+1]
				+ data[from][y-1][x]
				+ data[from][y+1][x]);
      }
    }
  }
#if 0
  for (int i=0; i<NY; ++i) {
    printf("%f\n", data[to][i][NX/2]);
  }
#endif

  return;
}

int  main(int argc, char *argv[])
{
  struct timeval t1, t2;
  int n_steps = 20;
  size_t NX, NY;

  if (process_args(argc, argv, n_steps, NX, NY)) {
    return 1;
  }

  float ***data = (float***)malloc(2*sizeof(float**));
  for (int i=0; i<2; ++i) {
    data[i] = (float**)malloc(NY*sizeof(float*));
    for (size_t y=0; y<NY; ++y) {
      data[i][y] = (float*)malloc(NX*sizeof(float));
    }
  }

  init(data, NX, NY);

  gettimeofday(&t1, NULL);

  calc(data, NX, NY, n_steps);

  gettimeofday(&t2, NULL);

  {
      double us;
      double gflops;
      int op_per_point = 5; // 4 add & 1 multiply per point

      us = get_elapsed_time(&t1, &t2);
      printf("Elapsed time: %.3lf sec\n", us/1000000.0);
      gflops = ((double)NX*NY*n_steps*op_per_point)/us/1000.0;
      printf("Speed: %.3lf GFlops\n", gflops);
  }

  return 0;
}
