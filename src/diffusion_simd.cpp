#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <stdint.h>

float*
alloc_mem(size_t NX, size_t NY)
{
  void* data_al;
  posix_memalign(&data_al, (size_t)32, 2*NX*NY*sizeof(float));
  float *data = (float*)data_al;
  return data;
}

int
free_mem(float *data)
{
  free(data);
  return 0;
}

// Uses 64bit pdep / pext to save a step in unpacking.
__m256 compress256(__m256 src, unsigned int mask /*  from movmskps */)
{
  uint64_t expanded_mask = _pdep_u64(mask, 0x0101010101010101);  // unpack each bit to a byte
  expanded_mask *= 0xFF;    // mask |= mask<<1 | mask<<2 | ... | mask<<7;
  // ABC... -> AAAAAAAABBBBBBBBCCCCCCCC...: replicate each bit to fill its byte

  const uint64_t identity_indices = 0x0706050403020100;    // the identity shuffle for vpermps, packed to one index per byte
  uint64_t wanted_indices = _pext_u64(identity_indices, expanded_mask);

  __m128i bytevec = _mm_cvtsi64_si128(wanted_indices);
  __m256i shufmask = _mm256_cvtepu8_epi32(bytevec);

 return _mm256_permutevar8x32_ps(src, shufmask);
}


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

    const __m256i forward_perm = _mm256_set_epi32(0, 7, 6, 5, 4, 3, 2, 1);
    const __m256i back_perm = _mm256_set_epi32(6, 5, 4, 3, 2, 1, 0, 7);
    const __m256 scalar = _mm256_set_ps(0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 1.0, 1.0);

    #pragma omp parallel for
    for (size_t y = 1; y < NY-1; y++) {
      for (size_t x = 0; x < NX-7; x+=8) {
        size_t current = y*NX + x;
        __m256 from_current = _mm256_load_ps(&data[from + current]);
        const __m256 from_x_minus = compress256(from_current, 0x11111100);
        const __m256 from_x_plus = compress256(from_current, 0x00111111);
        const __m256 from_y_minus = compress256(
            _mm256_load_ps(&data[from + current - NX]),
            0x01111110
            );
        const __m256 from_y_plus = compress256(
            _mm256_load_ps(&data[from + current + NX]),
            0x01111110
            );
        // Shift forward
        from_current = _mm256_permutevar8x32_ps(from_current, forward_perm);

        from_current = _mm256_add_ps(from_current, from_x_minus);
        from_current = _mm256_add_ps(from_current, from_x_plus);
        from_current = _mm256_add_ps(from_current, from_y_minus);
        from_current = _mm256_add_ps(from_current, from_y_plus);
        from_current = _mm256_mul_ps(from_current, scalar);
        // Shift backward
        from_current = _mm256_permutevar8x32_ps(from_current, back_perm);
        _mm256_store_ps(&data[to + current], from_current);
      }
      for (size_t x=7; x<NX-7; x+=8) {
          size_t current = y*NX + x;
          data[to + current] = 0.2 * (data[from + current]
          + data[from + current - 1]
          + data[from + current + 1]
          + data[from + current - NX]
          + data[from + current + NX]);
      }
      for (size_t x=8; x<NX-6; x+=8) {
          size_t current = y*NX + x;
          data[to + current] = 0.2 * (data[from + current]
          + data[from + current - 1]
          + data[from + current + 1]
          + data[from + current - NX]
          + data[from + current + NX]);
      }
    }
  }
#if 1
  for (size_t i=0; i<NY; ++i) {
    printf("%f\n", data[to + i*NX + NX/2-3]);
  }
#endif

  return 0;
}
