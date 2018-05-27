#include <stdlib.h>

float* alloc_mem(size_t NX, size_t NY);

int init(float* data, size_t NX, size_t NY, size_t y_size, int size, int rank);

int free_mem(float* data, size_t NY);
