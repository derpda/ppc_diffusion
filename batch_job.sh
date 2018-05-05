#!/bin/sh

NODE='h_node'
H_RT='00:10:00'
N_STEPS=5000
EXEC='cuda'

power=5
until [ $power -gt 16 ]
do
    NX=$(echo "2^$power" | bc)
    NY=$NX
    JOB="${NODE}_${N_STEPS}_${power}_$NX"
    awk '{
        gsub("NODE","'$NODE'");
        gsub("H_RT","'$H_RT'");
        gsub("JOB","'$JOB'");
        gsub("N_STEPS","'$N_STEPS'");
        gsub("NX","'$NX'");
        gsub("NY","'$NY'");
        gsub("EXEC","'$EXEC'");
        print $0
    }' ./blank_job | qsub -g tga-ppcomp
    ((power++))
done

