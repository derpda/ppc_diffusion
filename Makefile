CC = g++
NVCC = nvcc
CFLAGS = -std=c++11 -O3 -g -fopenmp -march=core-avx2 -mbmi2 -ffast-math
CUDAFLAGS = -std=c++11 -gencode=arch=compute_60,code=sm_60 -O3 -Xcompiler -fopenmp
LDFLAGS = -fopenmp
LIBS =

%.o: %.cpp
	${CC} ${CFLAGS} -c $< -o $*.o

%.o: %.cu
	${NVCC} ${CUDAFLAGS} -c $< -o $*.o

plain: diffusion_plain.o
	${CC} $^ ${LIB} -o $@ ${LDFLAGS}

omp: main.o diffusion_omp.o
	${CC} $^ ${LIBS} -o $@ ${LDFLAGS}

omp_blocking: main.o diffusion_omp_blocking.o
	${CC} $^ ${LIBS} -o $@ ${LDFLAGS}

simd: main.o diffusion_simd.o
	${CC} $^ ${LIBS} -o $@ ${LDFLAGS}

cuda: main.o diffusion_cuda.o
	${NVCC} ${CUDAFLAGS} $^ ${LIBS} -o $@

clean:
	-rm -f diffusion
	-rm -f plain
	-rm -f omp
	-rm -f omp_blocking
	-rm -f simd
	-rm -f cuda
	-rm -f *.o
