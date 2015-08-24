#!/bin/bash

###################################
K=6
FILES=(1 4 8 16 32 64 128 256)
HTDOCS="/usr/local/apache2/htdocs/"
SUDO="sudo"
###################################

function populate_full {
    for F in ${FILES[@]}
    do
        echo "$F MB"
        $SUDO dd if=/dev/urandom of=$HTDOCS/${F}mb bs=1M count=$F
    done
}

function populate_chunks {
    for F in ${FILES[@]}
    do
        CHUNK=$(( $F * 1024 / $K ))
        echo "$CHUNK KB"
        $SUDO dd if=/dev/urandom of=$HTDOCS/${F}mb_chunk bs=1K count=$CHUNK
    done
}

###################################
$SUDO rm -rf $HTDOCS/*mb* #sudo
populate_full
populate_chunks

ls -lh $HTDOCS