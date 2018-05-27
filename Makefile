CC = g++
MPICC = scorep mpicxx
NVCC = nvcc
CFLAGS = -std=c++11 -O2 -g -fopenmp
CFLAGS_SIMD = -march=core-avx2
CUDAFLAGS = -std=c++11 -gencode=arch=compute_60,code=sm_60 -O3 -Xcompiler -fopenmp
LDFLAGS = -fopenmp
LIBS =

OBJ_DIR=obj
SRC_DIR=src

BASE_1D=${OBJ_DIR}/1d_main.o ${OBJ_DIR}/utils.o ${OBJ_DIR}/1d_utils.o
BASE_3D=${OBJ_DIR}/3d_main.o ${OBJ_DIR}/utils.o ${OBJ_DIR}/3d_utils.o

EXEC=3d_plain 3d_omp 3d_omp_simd\
	 1d_plain 1d_omp 1d_omp_blocking 1d_omp_simd\
	 mpi\
	 cuda

all: ${EXEC}

${OBJ_DIR}/mpi_%.o: ${SRC_DIR}/mpi_%.cpp
	${MPICC} ${CFLAGS} ${CFLAGS_SIMD} -c $< -o ${OBJ_DIR}/mpi_$*.o

${OBJ_DIR}/%_simd.o: ${SRC_DIR}/%_simd.cpp
	${CC} ${CFLAGS} ${CFLAGS_SIMD} -c $< -o ${OBJ_DIR}/$*_simd.o

${OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	${CC} ${CFLAGS} -c $< -o ${OBJ_DIR}/$*.o

${OBJ_DIR}/%.o: ${SRC_DIR}/%.cu
	${NVCC} ${CUDAFLAGS} -c $< -o ${OBJ_DIR}/$*.o

3d_plain: ${BASE_3D} ${OBJ_DIR}/3d_plain.o
	${CC} $^ ${LIB} -o $@ ${LDFLAGS}

3d_omp: ${BASE_3D} ${OBJ_DIR}/3d_omp.o
	${CC} $^ ${LIB} -o $@ ${LDFLAGS}

3d_omp_simd: ${BASE_3D} ${OBJ_DIR}/3d_omp_simd.o
	${CC} $^ ${LIB} -o $@ ${LDFLAGS}

1d_plain: ${BASE_1D} ${OBJ_DIR}/1d_plain.o
	${CC} $^ ${LIBS} -o $@ ${LDFLAGS}

1d_omp: ${BASE_1D} ${OBJ_DIR}/1d_omp.o
	${CC} $^ ${LIBS} -o $@ ${LDFLAGS}

1d_omp_blocking: ${BASE_1D} ${OBJ_DIR}/1d_omp_blocking.o
	${CC} $^ ${LIBS} -o $@ ${LDFLAGS}

1d_omp_simd: ${BASE_1D} ${OBJ_DIR}/1d_omp_simd.o
	${CC} $^ ${LIBS} -o $@ ${LDFLAGS}

mpi: ${OBJ_DIR}/mpi_main.o ${OBJ_DIR}/utils.o ${OBJ_DIR}/mpi_utils.o\
   		${OBJ_DIR}/mpi_kernel_simd.o
	${MPICC} $^ ${LIBS} -o $@ ${LDFLAGS}

cuda: ${OBJ_DIR}/1d_main.o ${OBJ_DIR}/utils.o ${OBJ_DIR}/cuda.o
	${NVCC} ${CUDAFLAGS} $^ ${LIBS} -o $@

clean:
	-rm -f ${EXEC}
	-rm -f ${OBJ_DIR}/*.o
