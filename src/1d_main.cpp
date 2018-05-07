#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <immintrin.h>
#include <malloc.h>


float* alloc_mem(size_t NX, size_t NY);

int init(float* data, size_t NX, size_t NY);

int calc(float* data, size_t NX, size_t NY, int n_steps);

int free_mem(float*, size_t NY);

int process_args(int argc, char *argv[], size_t &NX, size_t &NY, int &n_steps);

int print_gflops(struct timeval t1, struct timeval t2, size_t NX, size_t NY, int n_steps);


int  main(int argc, char *argv[])
{
  struct timeval t1, t2;
  int n_steps;
  size_t NX, NY;

  if (process_args(argc, argv, NX, NY, n_steps)) {
    return 1;
  }

  float* data = alloc_mem(NX, NY);

  if (init(data, NX, NY)) {
    return 1;
  }

  gettimeofday(&t1, NULL);

  if (calc(data, NX, NY, n_steps)) {
    return 1;
  }

  gettimeofday(&t2, NULL);

  if (free_mem(data, NY)) {
    return 1;
  }

  if (print_gflops(t1, t2, NX, NY, n_steps)) {
    return 1;
  }

  return 0;
}
