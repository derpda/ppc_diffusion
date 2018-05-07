#!/bin/sh

NODE='f_node'
H_RT='00:10:00'
EXEC='3d_omp'

N_THREADS=1
until [ $N_THREADS -gt 6 ]
do
    N_BASE=100
    TIME=1
    until [ $TIME -gt 20 ]
    do
        N_STEPS=$((${N_BASE}*${TIME}))
        POWER=8
        until [ $POWER -gt 13 ]
        do
            NX=$(echo "2^$POWER" | bc)
            NY=$NX
            JOB="${NODE}.${N_STEPS}.${NX}.${N_THREADS}"
            awk '{
                gsub("{NODE}","'$NODE'");
                gsub("{H_RT}","'$H_RT'");
                gsub("{JOB}","'$JOB'");
                gsub("{N_STEPS}","'$N_STEPS'");
                gsub("{N_THREADS}","'$N_THREADS'");
                gsub("{NX}","'$NX'");
                gsub("{NY}","'$NY'");
                gsub("{EXEC}","'$EXEC'");
                print $0
            }' ./src/blank_job | qsub #-g tga-ppcomp
            ((POWER++))
        done
        ((TIME++))
    done
    ((N_THREADS++))
done

