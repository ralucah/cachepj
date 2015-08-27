#!/bin/bash

trap "rm -fr T Coding"  EXIT

FILES=(1 4 8 16 32 64 128 256)
K=6
M=2

for F in ${FILES[@]}
do
    echo "$F MB"
    dd if=/dev/urandom of=T bs=1M count=$F #sudo
    ./encoder T $K $M reed_sol_van 8 0 0
    rm Coding/T_m1 #T_k5 #
    rm Coding/T_m2 #T_k6 #
    ./decoder T
done
