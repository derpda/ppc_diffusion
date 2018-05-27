#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <mpi.h>

#include "./utils.h"
#include "./mpi_utils.h"

int calc(
    float* data,
    size_t NX, size_t NY, size_t y_size,
    int n_steps,
    int size, int rank);

int main(int argc, char *argv[])
{
  double start, end;
  int n_steps;
  int provided, size, rank;
  size_t NX, NY;

  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (process_args(argc, argv, NX, NY, n_steps)) {
    return 1;
  }

  // Divide region in y direction (uses integer math) + 2 for boundary from
  // neighbor processes
  size_t y_size = (NY+size-1)/size + 2;

  float* data = alloc_mem(NX, y_size);

  if (init(data, NX, NY, y_size, size, rank)) {
    return 1;
  }

  MPI_Barrier(MPI_COMM_WORLD);
  start = MPI_Wtime();

  if (calc(data, NX, NY, y_size, n_steps, size, rank)) {
    return 1;
  }

  MPI_Barrier(MPI_COMM_WORLD);
  end = MPI_Wtime();

  if (free_mem(data, NY)) {
    return 1;
  }

  if (rank == size - 1) {
    if (print_gflops(start, end, NX, NY, n_steps)) {
      return 1;
    }
  }

  MPI_Finalize();

  return 0;
}

