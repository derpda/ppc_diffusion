#!/bin/sh
#$ -cwd
#$ -l q_node=4
#$ -l h_rt=00:10:00
#$ -N diff_mpi
#$ -o output_mpi.$JOB_ID
#$ -e error_mpi.$JOB_ID

. /etc/profile.d/modules.sh
module load cuda/8.0.61
module load openmpi/2.1.2

# Visual profiler settings
export SCOREP_ENABLE_TRACING=true
export SCOREP_EXPERIMENT_DIRECTORY=scorep

export OMP_NUM_THREADS=7

mpirun -npernode 1 -n 4 -x LD_LIBRARY_PATH ./mpi 5000 256 256
