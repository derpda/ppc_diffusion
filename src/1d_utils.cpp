#include <stdlib.h>

float*
alloc_mem(size_t NX, size_t NY)
{
  float *data = (float*)malloc(2*NY*NX*sizeof(float));
  return data;
}

int
init(float* data, size_t NX, size_t NY)
{
  size_t cx = NX/2, cy = 0; /* center of ink */
  size_t rad = (NX+NY)/8; /* radius of ink */
#pragma omp parallel shared(cx, rad, cy)
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

int
free_mem(float* data, size_t NY)
{
  free(data);
  return 0;
}
