#include <stdlib.h>
#include <algorithm>

float* alloc_mem(size_t NX, size_t y_size) {
  float *data;
  posix_memalign((void**)&data, 32, 2*NX*y_size*sizeof(float));
  return data;
}

int init(float* data, size_t NX, size_t NY, size_t y_size, int size, int rank) {
  // Offset in the actual matrix. size_t*size is bigger than the actuall matrix
  size_t y_off = (y_size - 2)*rank;
  size_t y_min = rank > 0 ? 1 : 0;
  size_t y_max = rank < size - 1 ? y_size - 1 : NY - y_off;

  size_t cx = NX/2, cy = 0; /* center of ink */
  size_t rad = (NX+NY)/8; /* radius of ink */
#pragma omp parallel shared(cx, rad, cy)
  for(size_t y = y_min; y < y_max; y++) {
    for(size_t x = 0; x < NX; x++) {
      float v;
      if (((x-cx)*(x-cx)+(y_off+y-cy)*(y_off+y-cy)) < rad*rad) {
        v = 1.0;
      } else {
        v = 0.0;
      }
      data[y*NX+x] = v;
      data[(y_size+y)*NX+x] = v;
    }
  }

  return 0;
}

int free_mem(float* data, size_t NY) {
  free(data);
  return 0;
}
