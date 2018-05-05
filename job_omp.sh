#!/bin/sh
#$ -cwd
#$ -l h_node=1
#$ -l h_rt=01:00:00
#$ -N diff_omp
#$ -o output_omp.$JOB_ID
#$ -e error_omp.$JOB_ID

./omp 15000 16384 16384
