#include <stdio.h>
#include <stdlib.h>


int
calc(float ***data, size_t NX, size_t NY, int n_steps)
{
  size_t to;
  for (int t = 0; t < n_steps; t++) {
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

  return 0;
}
