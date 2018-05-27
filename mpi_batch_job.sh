#!/bin/sh

NODE='f_node'
H_RT='00:10:00'
EXEC='mpi_sync'

if [ $NODE == 'f_node' ]
then
    N_THREADS=56
elif [ $NODE == 'h_node' ]
then
    N_THREADS=28
elif [ $NODE == 'q_node' ]
then
    N_THREADS=14
elif [ $NODE == 'q_core' ]
then
    N_THREADS=8
fi

N_NODES=7
until [ $N_NODES -gt 7 ]
do
    ((N_NODES++))
    N_BASE=100
    TIME=1
    until [ $TIME -gt 1 ]
    do
        N_STEPS=$((${N_BASE}*${TIME}))
        ((TIME++))

        POWER=12
        until [ $POWER -gt 12 ]
        do
            NY=$(echo "2^$POWER" | bc)
            NX=$NY
            ((POWER++))

            JOB="${NODE}.${N_NODES}.${N_STEPS}.${NY}.${N_THREADS}"
            [ -e output_files/mpi/${EXEC}.${JOB} ] && rm -i output_files/mpi/${EXEC}.${JOB}
            awk '{
                gsub("{NODE}","'$NODE'");
                gsub("{H_RT}","'$H_RT'");
                gsub("{JOB}","'$JOB'");
                gsub("{N_STEPS}","'$N_STEPS'");
                gsub("{N_NODES}","'$N_NODES'");
                gsub("{N_THREADS}","'$N_THREADS'");
                gsub("{NX}","'$NX'");
                gsub("{NY}","'$NY'");
                gsub("{EXEC}","'$EXEC'");
                print $0
            }' ./src/blank_mpi_job | qsub -g tga-ppcomp
        done
    done
done
