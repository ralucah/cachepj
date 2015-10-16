#!/bin/bash -e
#
# Copyright (C) 2014 Red Hat <contact@redhat.com>
#
# Author: Loic Dachary <loic@dachary.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library Public License for more details.
#


# gcc -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mavx -g -O3 -Wall -Wa,-q -lJerasure -ltiming -lgf_complete -o encoder encoder.c
# gcc -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mavx -g -O3 -Wall -Wa,-q -lJerasure -ltiming -lgf_complete -o decoder decoder.c

TESTS=3
BS=1024
COUNT=1024
W=8
K=3
M=2
PACKETSIZE=0
BUFFERSIZE=0
CODING="reed_sol_van"

trap "rm -fr T Coding"  EXIT


for W in 8 16 32 ; do
	echo "*** W = $W ***"
	for (( i=1; i<=$TESTS; i++ ))  ; do
		echo "Test $i"
		rm -fr T Coding 2&>/dev/null

		dd if=/dev/urandom of=T bs=$BS count=$COUNT
		./encoder T $K $M $CODING $W 0 0
		./decoder T
		echo ""
	done
	echo ""
done

# vary w 8,16,32

#	for (( i=0; i<$TESTS; i++ ))  ; do
#		echo "-----------------------"
#		echo $CT
#		echo "-----------------------"

#		rm -fr T Coding 2&>/dev/null

#		dd if=/dev/urandom of=T bs=$BS count=$COUNT
#		echo "Encode"
#		./encoder T $K $M $CT $W 0 0
#		echo "Decode"
#		./decoder T
#	done