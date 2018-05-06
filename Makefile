CC = g++
NVCC = nvcc
CFLAGS = -std=c++11 -O3 -g -fopenmp -march=core-avx2 -mbmi2 -ffast-math
CUDAFLAGS = -std=c++11 -gencode=arch=compute_60,code=sm_60 -O3 -Xcompiler -fopenmp
LDFLAGS = -fopenmp
LIBS =

OBJ_DIR=obj
SRC_DIR=src

${OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	${CC} ${CFLAGS} -c $< -o ${OBJ_DIR}/$*.o

${OBJ_DIR}/%.o: ${SRC_DIR}/%.cu
	${NVCC} ${CUDAFLAGS} -c $< -o ${OBJ_DIR}/$*.o

plain: ${OBJ_DIR}/diffusion_plain.o
	${CC} $^ ${LIB} -o $@ ${LDFLAGS}

plain_omp: ${OBJ_DIR}/diffusion_plain_omp.o
	${CC} $^ ${LIB} -o $@ ${LDFLAGS}

omp: ${OBJ_DIR}/main.o ${OBJ_DIR}/diffusion_omp.o
	${CC} $^ ${LIBS} -o $@ ${LDFLAGS}

omp_blocking: ${OBJ_DIR}/main.o ${OBJ_DIR}/diffusion_omp_blocking.o
	${CC} $^ ${LIBS} -o $@ ${LDFLAGS}

simd: ${OBJ_DIR}/main.o ${OBJ_DIR}/diffusion_simd.o
	${CC} $^ ${LIBS} -o $@ ${LDFLAGS}

cuda: ${OBJ_DIR}/main.o ${OBJ_DIR}/diffusion_cuda.o
	${NVCC} ${CUDAFLAGS} $^ ${LIBS} -o $@

clean:
	-rm -f diffusion
	-rm -f plain
	-rm -f plain_omp
	-rm -f omp
	-rm -f omp_blocking
	-rm -f simd
	-rm -f cuda
	-rm -f ${OBJ_DIR}/*.o
