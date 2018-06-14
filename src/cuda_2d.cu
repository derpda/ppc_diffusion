#include <omp.h>
#include <cstdio>
#include <iostream>
#include <algorithm>

float*
alloc_mem(size_t NX, size_t NY)
{
  float *data;
  cudaMallocManaged(&data, 2 * NY * NX * sizeof(float));
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
  cudaFree(data);
  return 0;
}

#define GPU_ERROR_CHECK(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
  if (code != cudaSuccess)
  {
    fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
    if (abort) exit(code);
  }
}

__global__ void
line_kernel(float* data, size_t from, size_t to, size_t min, size_t max, size_t NX)
{
  size_t i = min + blockIdx.x * blockDim.x + threadIdx.x;
  while (i < max) {
    //TODO CONSIDER REMOVING MODULUS (might be slow)
    if ( (i % NX != 0) && (i % NX != NX - 1) ){
      data[to+i] = 0.2 * (
          data[from+i]
          + data[from+i-1]
          + data[from+i+1]
          + data[from+i-NX]
          + data[from+i+NX]);
    }
    i +=gridDim.x*blockDim.x;
  }
}

__global__ void
block_kernel(float* data, size_t from, size_t to, size_t min, size_t max, size_t NX)
{
  size_t i = min + blockIdx.x * blockDim.x + threadIdx.x;
  while (i < max) {
    //TODO CONSIDER REMOVING MODULUS (might be slow)
    if ( (i % NX != 0) && (i % NX != NX - 1) ){
      data[to+i] = 0.2 * (
          data[from+i]
          + data[from+i-1]
          + data[from+i+1]
          + data[from+i-NX]
          + data[from+i+NX]);
    }
    i +=gridDim.x*blockDim.x;
  }
}

int
calc(float* data, size_t NX, size_t NY, int n_steps)
{
  int n_devices;
  cudaGetDeviceCount(&n_devices);
  // Implement error handling
  if (n_devices < 1) return 1;
  // Prepare stream and event pointers
  cudaStream_t* line_streams = (cudaStream_t*)malloc(
      n_devices*sizeof(cudaStream_t));
  cudaStream_t* block_streams = (cudaStream_t*)malloc(
      n_devices*sizeof(cudaStream_t));
  cudaEvent_t *done = (cudaEvent_t*)malloc(
      n_devices*n_steps*sizeof(cudaEvent_t));

  int last_to;

#pragma omp parallel num_threads(n_devices)
  {
    int device_n = omp_get_thread_num();
    cudaSetDevice(device_n);
    // Prepare device streams and events
    cudaStreamCreate(&line_streams[device_n]);
    cudaStreamCreate(&block_streams[device_n]);
    if (n_devices > 1) {
      for (int t=0; t<n_steps; ++t) {
        cudaEventCreate(&done[device_n*n_steps+t]);
      }
    }
    // Also allow peer-to-peer direct access
    if (device_n > 0) {
      cudaDeviceEnablePeerAccess(device_n-1, 0);
    }
    if (device_n < n_devices-1) {
      cudaDeviceEnablePeerAccess(device_n+1, 0);
    }

    // Calculate needed memory
    // -3 since one more line is needed for the from array
    // and the first and last line need not be computed
    size_t Y_min_d = std::max(
        device_n * (NY + n_devices - 3) / n_devices,
        (size_t)1);
    size_t Y_max_d = std::min(
        (device_n + 1) * (NY + n_devices - 3) / n_devices,
        NY-1);
    // +-1 to skip
    size_t min = Y_min_d * NX + 1;
    size_t max = (Y_max_d) * NX - 1;
    size_t Y_range = Y_max_d - Y_min_d;
#if 0
    printf("Device: %llu\tYmin: %llu/%llu\tYmax: %llu/%llu\n",
        device_n, Y_min_d, min, Y_max_d, max);
#endif
    fflush(0);
// Make sure all events and streams are created!
#pragma omp barrier
    for (int t = 0; t < n_steps; t++) {
#if 0
      printf("device %i step %d START\n", device_n, t);
      fflush(0);
#endif
      size_t from = NY*NX*(t%2);
      size_t to = NY*NX*((t+1)%2);
      // Calculate main bulk that this gpu is responsible for if last step is
      // done
      if (t > 0) {
        GPU_ERROR_CHECK(
            cudaStreamWaitEvent(
              block_streams[device_n],
              done[device_n*n_steps+t-1],
              0)
            );
      }
      block_kernel<<<std::max(Y_range*NX/512, (size_t)1),512,0,block_streams[device_n]>>>(
          data,
          from,
          to,
          min+NX,
          max-NX,
          NX);
      // Calculate top row after potentially waiting for updated results
      if (t > 0 && device_n < n_devices - 1) {
        GPU_ERROR_CHECK(
            cudaStreamWaitEvent(
              line_streams[device_n+1],
              done[(device_n+1)*n_steps+t-1],
              0)
            );
      }
      line_kernel<<<std::max(NX/16, (size_t)1),16,0,line_streams[device_n]>>>(
          data,
          from,
          to,
          max-NX,
          max,
          NX);
      // Calculate bottom row after potentially waiting for updated results
      if (t > 0 && device_n > 0) {
        GPU_ERROR_CHECK(
          cudaStreamWaitEvent(
              line_streams[device_n-1],
              done[(device_n-1)*n_steps+t-1],
              0)
          );
      }
      line_kernel<<<std::max(NX/16, (size_t)1),16,0,line_streams[device_n]>>>(
          data,
          from,
          to,
          min,
          min+NX,
          NX);
      // The calculations critical for neighboring blocks are done, set event
      if (n_devices > 1) {
        GPU_ERROR_CHECK(
          cudaEventRecord(
              done[device_n*n_steps+t],
              line_streams[device_n])
        );
      }

      if (device_n == 0 && t == n_steps - 1) last_to = to;
    }
  }
  cudaDeviceSynchronize();
  // Release steams and events
  for (int i = 0; i<n_devices; ++i) {
    cudaStreamDestroy(line_streams[i]);
    cudaStreamDestroy(block_streams[i]);
    for (int j=0; j<n_steps; ++j) {
      cudaEventDestroy(done[i*n_steps + j]);
    }
  }
  free(line_streams);
  free(block_streams);
  free(done);
#if 0
  for (size_t i=0; i<NY; ++i) {
    printf("%f\n", data[last_to + i*NX + NX/2]);
  }
#endif
  return 0;
}
