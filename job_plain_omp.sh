#!/bin/sh
#$ -cwd
#$ -l f_node=1
#$ -l h_rt=00:10:00
#$ -N diff_plain_omp
#$ -o output_plain_omp.$JOB_ID
#$ -e error_plain_omp.$JOB_ID

./plain_omp 2000 8192 8192
