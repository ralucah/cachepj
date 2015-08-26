#!/bin/bash

###################################
K=6

CHUNKS=(8 64 128) #(1 4 8 16 32 64 128)
HTDOCS="/usr/local/apache2/htdocs/"
SUDO=""
###################################

function populate_full {
    for C in ${CHUNKS[@]}
    do
        FULL=$(( $C * $K ))
        echo "Full: $FULL MB"
        $SUDO dd if=/dev/urandom of=$HTDOCS/$FULL bs=${C}M count=$K
    done
}

function populate_chunks {
    for C in ${CHUNKS[@]}
    do
        echo "$C MB"
        $SUDO dd if=/dev/urandom of=$HTDOCS/${C} bs=1M count=$C
    done
}

###################################
$SUDO rm -rf $HTDOCS/*
populate_full
populate_chunks

ls -lh $HTDOCS