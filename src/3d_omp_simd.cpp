#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <stdint.h>

int
calc(float*** data, size_t NX, size_t NY, int n_steps)
{
  size_t to;
  for (int t = 0; t < n_steps; t++) {
    size_t from = (t%2);
    to = ((t+1)%2);

#if 0
    printf("step %d\n", t);
    fflush(0);
#endif

    const __m256 scalar = _mm256_set_ps(0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2);

    #pragma omp parallel for
    for (size_t y = 1; y < NY-1; y++) {
      for (size_t x = 0; x < NX-7; x+=8) {
        __m256 from_current = _mm256_loadu_ps(&data[from][y][x]);
        const __m256 from_x_minus = _mm256_loadu_ps(&data[from][y][x-1]);
        const __m256 from_x_plus = _mm256_loadu_ps(&data[from][y][x+1]);
        const __m256 from_y_minus = _mm256_loadu_ps(&data[from][y-1][x]);
        const __m256 from_y_plus = _mm256_loadu_ps(&data[from][y+1][x]);

        from_current = _mm256_add_ps(from_current, from_x_minus);
        from_current = _mm256_add_ps(from_current, from_x_plus);
        from_current = _mm256_add_ps(from_current, from_y_minus);
        from_current = _mm256_add_ps(from_current, from_y_plus);
        from_current = _mm256_mul_ps(from_current, scalar);
        _mm256_storeu_ps(&data[to][y][x], from_current);
      }
      // reset beginning and end of line to 0
      data[to][y][0] = 0.0;
      data[to][y][NX-1] = 0.0;
    }
  }
#if 0
  for (size_t i=0; i<NY; ++i) {
    printf("%f\n", data[to][i][NX/2]);
  }
#endif

  return 0;
}
