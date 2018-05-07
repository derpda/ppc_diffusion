#!/bin/sh
#$ -cwd
#$ -l f_node=1
#$ -l h_rt=00:10:00
#$ -N diff_omp
#$ -o output_simd.$JOB_ID
#$ -e error_simd.$JOB_ID

./simd 2000 8192 8192
