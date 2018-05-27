#include <stdlib.h>
#include <algorithm>
#include <immintrin.h>
#include <mpi.h>
#include <omp.h>

void main_kernel(
    float* from, float* to, size_t NX,
    size_t y_min, size_t y_max) {
  register const __m256 scalar = _mm256_set_ps(
      0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2);

  // Not that y=1 and y=y_max-2 are calculated separately to overlap
  // calculation and messaging
  #pragma omp parallel for
  for (size_t y = y_min; y < y_max; y++) {
    for (size_t x = 0; x < NX-7; x+=8) {
      size_t current = y*NX + x;
      __m256 from_current = _mm256_load_ps(&from[current]);
      const __m256 from_x_minus = _mm256_loadu_ps(&from[current - 1]);
      const __m256 from_x_plus = _mm256_loadu_ps(&from[current + 1]);
      const __m256 from_y_minus = _mm256_load_ps(&from[current - NX]);
      const __m256 from_y_plus = _mm256_load_ps(&from[current + NX]);

      from_current = _mm256_add_ps(from_current, from_x_minus);
      from_current = _mm256_add_ps(from_current, from_x_plus);
      from_current = _mm256_add_ps(from_current, from_y_minus);
      from_current = _mm256_add_ps(from_current, from_y_plus);
      from_current = _mm256_mul_ps(from_current, scalar);
      _mm256_store_ps(&to[current], from_current);
    }
    // reset beginning and end of line to 0
    to[y*NX] = 0.0;
    to[(y+1)*NX-1] = 0.0;
  }
}

int calc(
    float* data,
    size_t NX, size_t NY, size_t y_size,
    int n_steps,
    int size, int rank) {
  size_t y_max = std::min(y_size, NY-(y_size-2)*rank);
  size_t to;
  for (int t = 0; t < n_steps; t++) {
    size_t from = y_size*NX*(t%2);
    to = y_size*NX*((t+1)%2);
    // Send lower / receive upper boundary
    if (rank > 0) {
      MPI_Send(
          &data[from + NX + 1], NX-2, MPI_FLOAT,
          rank-1, 0, MPI_COMM_WORLD);
    }
    if (rank < size - 1) {
      MPI_Status status;
      MPI_Recv(
          &data[from + (y_size-1)*NX + 1], NX-2, MPI_FLOAT,
          rank+1, 0, MPI_COMM_WORLD, &status);
    }
    // Send upper / receive lower boundary
    if (rank < size - 1) {
      MPI_Send(
          &data[from + (y_size-2)*NX + 1], NX-2, MPI_FLOAT,
          rank+1, 1, MPI_COMM_WORLD);
    }
    if (rank > 0) {
      MPI_Status status;
      MPI_Recv(
          &data[from + 1], NX-2, MPI_FLOAT,
          rank-1, 1, MPI_COMM_WORLD, &status);
    }

    // Calculate results from this round
    main_kernel(&data[from], &data[to], NX, 1, y_max-1);
  }
#if 0
  if (rank == size - 1) {
    for (size_t i=0; i<y_max; ++i) {
      printf("%f\n", data[to + i*NX + NX/2]);
    }
  }
#endif

  return 0;
}
