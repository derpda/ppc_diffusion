#include <stdlib.h>

float***
alloc_mem(size_t NX, size_t NY)
{
  float ***data = (float***)malloc(2*sizeof(float**));
  for (int i=0; i<2; ++i) {
    data[i] = (float**)malloc(NY*sizeof(float*));
    for (size_t y=0; y<NY; ++y) {
      data[i][y] = (float*)malloc(NX*sizeof(float));
    }
  }
  return data;
}

int
init(float*** data, size_t NX, size_t NY)
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
  return 0;
}

int
free_mem(float*** data, size_t NY) {
  for (int i=0; i<2; ++i) {
    for (size_t y=0; y<NY; ++y) {
      free(data[i][y]);
    }
    free(data[i]);
  }
  return 0;
}
