#include <stdio.h>
#include <stdlib.h>

int
calc(float* data, size_t NX, size_t NY, int n_steps)
{
  size_t to;
  for (int t = 0; t < n_steps; t++) {
    size_t from = NY*NX*(t%2);
    to = NY*NX*((t+1)%2);

#if 0
    printf("step %d\n", t);
    fflush(0);
#endif

    for (size_t y = 1; y < NY-1; y++) {
      for (size_t x = 1; x < NX-1; x++) {
        size_t current = y*NX + x;
        data[to + current] = 0.2 * (data[from + current]
        + data[from + current - 1]
        + data[from + current + 1]
        + data[from + current - NX]
        + data[from + current + NX]);
      }
    }
  }
#if 0
  for (size_t i=0; i<NY; ++i) {
    printf("%f\n", data[to + i*NX + NX/2]);
  }
#endif

  return 0;
}
