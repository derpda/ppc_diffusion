#!/bin/sh
#$ -cwd
#$ -l q_core=1
#$ -l h_rt=04:00:00
#$ -N diff_plain
#$ -o output_plain.$JOB_ID
#$ -e error_plain.$JOB_ID

./plain 15000
