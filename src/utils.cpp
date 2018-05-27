#include <stdlib.h>
#include <iostream>
#include <sys/time.h>

int
process_args(int argc, char *argv[], size_t &NX, size_t &NY, int &n_steps)
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
print_gflops(struct timeval t1, struct timeval t2, size_t NX, size_t NY, int n_steps) {
  double us;
  double gflops;
  int op_per_point = 5; // 4 add & 1 multiply per point

  us = get_elapsed_time(&t1, &t2);
  printf("Elapsed time: %.3lf sec\n", us/1000000.0);
  gflops = ((double)NX * NY * n_steps * op_per_point) / 1000.0 / us;
  printf("Speed: %.3lf GFlops\n", gflops);
  return 0;
}

int
print_gflops(double start, double end, size_t NX, size_t NY, int n_steps) {
  double s;
  double gflops;
  int op_per_point = 5; // 4 add & 1 multiply per point

  s = end - start;
  printf("Elapsed time: %.3lf sec\n", s);
  gflops = ((double)NX * NY * n_steps * op_per_point) / 1000.0 / s / 1000000.0;
  printf("Speed: %.3lf GFlops\n", gflops);
  return 0;
}
