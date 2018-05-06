#!/bin/sh
#$ -cwd
#$ -l f_node=1
#$ -l h_rt=00:10:00
#$ -N diff_plain
#$ -o output_plain.$JOB_ID
#$ -e error_plain.$JOB_ID

./plain 200
