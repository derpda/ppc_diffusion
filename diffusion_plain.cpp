#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define NX 16384
#define NY 16384

float data[2][NY][NX];

/* in microseconds (us) */
double get_elapsed_time(struct timeval *begin, struct timeval *end)
{
    return (end->tv_sec - begin->tv_sec) * 1000000
            + (end->tv_usec - begin->tv_usec);
}

void init()
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

void calc(int nt)
{
  size_t to;
  for (int t = 0; t < nt; t++) {
    size_t from = t%2;
    to = (t+1)%2;

#if 0
    printf("step %d\n", t);
    fflush(0);
#endif

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
  int nt = 20;

  if (argc >= 2) { /* if an argument is specified */
      nt = atoi(argv[1]);
  }

  init();

  gettimeofday(&t1, NULL);

  calc(nt);

  gettimeofday(&t2, NULL);

  {
      double us;
      double gflops;
      int op_per_point = 5; // 4 add & 1 multiply per point

      us = get_elapsed_time(&t1, &t2);
      printf("Elapsed time: %.3lf sec\n", us/1000000.0);
      gflops = ((double)NX*NY*nt*op_per_point)/us/1000.0;
      printf("Speed: %.3lf GFlops\n", gflops);
  }

  return 0;
}
