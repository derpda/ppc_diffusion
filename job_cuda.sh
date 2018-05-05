#!/bin/sh
#$ -cwd
#$ -l f_node=1
#$ -l h_rt=10:00:00
#$ -N diff_cuda
#$ -o output_cuda.$JOB_ID
#$ -e error_cuda.$JOB_ID

. /etc/profile.d/modules.sh
module load cuda/8.0.61

./cuda 1000 131072 131072

