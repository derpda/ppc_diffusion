#!/bin/sh

NODE='f_node'
H_RT='00:10:00'
EXEC='3d_omp'

THREAD_CALC=56
until [ $THREAD_CALC -gt 56 ]
do
    N_THREADS=${THREAD_CALC}
    ((THREAD_CALC++))

    N_BASE=1000
    TIME=1
    until [ $TIME -gt 20 ]
    do
        N_STEPS=$((${N_BASE}*${TIME}))
        ((TIME++))

        POWER=13
        until [ $POWER -gt 13 ]
        do
            NX=$(echo "2^$POWER" | bc)
            NY=$NX
            ((POWER++))

            JOB="${NODE}.${N_STEPS}.${NX}.${N_THREADS}"
            [ -e output_files/${EXEC}.${JOB} ] && rm -i output_files/${EXEC}.${JOB}
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
        done
    done
done

