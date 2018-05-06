#!/bin/sh

NODE='f_node'
H_RT='00:10:00'
EXEC='omp_blocking'
N_THREADS=16

N_BASE=100
TIME=1
until [ $TIME -gt 20 ]
do
    N_STEPS=$((${N_BASE}*${TIME}))
    POWER=6
    until [ $POWER -gt 13 ]
    do
        NX=$(echo "2^$POWER" | bc)
        NY=$NX
        JOB="${NODE}.${N_STEPS}.$NX"
        awk '{
            gsub("NODE","'$NODE'");
            gsub("H_RT","'$H_RT'");
            gsub("JOB","'$JOB'");
            gsub("N_STEPS","'$N_STEPS'");
            gsub("NX","'$NX'");
            gsub("NY","'$NY'");
            gsub("EXEC","'$EXEC'");
            print $0
        }' ./src/blank_job | qsub #-g tga-ppcomp
        ((POWER++))
    done
    ((TIME++))
done

