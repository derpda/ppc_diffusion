#ifndef utils_h
#define utils_h
int process_args(int argc, char *argv[], size_t &NX, size_t &NY, int &n_steps);

int print_gflops(struct timeval t1, struct timeval t2, size_t NX, size_t NY, int n_steps);

int print_gflops(double start, double end, size_t NX, size_t NY, int n_steps);

#endif
