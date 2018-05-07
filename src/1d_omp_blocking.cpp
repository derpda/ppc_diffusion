#include <stdio.h>
#include <stdlib.h>

#define Y_BLOCK 512
#define X_BLOCK 512

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

#pragma omp parallel for
    for (size_t y=1; y<NY-Y_BLOCK+2; y+=Y_BLOCK) {
      for (size_t x=1; x<NX-X_BLOCK+2; x+=X_BLOCK) {
        for (size_t y2=0; y2<Y_BLOCK; y2++) {
          if (y + y2 > NY - 2) break;
          for (size_t x2=0; x2<X_BLOCK; x2++) {
            if (x + x2 > NX - 2) break;
            size_t current = (y+y2)*NX + x + x2;
            data[to + current] = 0.2 * (data[from + current]
            + data[from + current - 1]
            + data[from + current + 1]
            + data[from + current - NX]
            + data[from + current + NX]);
          }
        }
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
