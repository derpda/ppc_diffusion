#!/bin/sh
#$ -cwd
#$ -l q_node=2
#$ -l h_rt=00:10:00
#$ -N diff_mpi
#$ -o output_mpi.$JOB_ID
#$ -e error_mpi.$JOB_ID

. /etc/profile.d/modules.sh
module load cuda/8.0.61
module load openmpi/2.1.2

export OMP_NUM_THREADS=14

mpirun -npernode 1 -n 2 -bind-to none -x LD_LIBRARY_PATH ./mpi 100 8192 8192
