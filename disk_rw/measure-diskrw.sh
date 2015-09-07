#!/bin/bash

FILENAME=/tmp/test
BLOCKSIZE=1M
BLOCKCOUNT=1024
TESTS=5

for (( I=1;I<=$TESTS;I++ )) ; do
    if [ ! -f $FILENAME ] ; then
        echo '--- Write ---'
        dd if=/dev/urandom of=$FILENAME bs=$BLOCKSIZE count=$BLOCKCOUNT conv=fsync
    fi

    sync && sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"

    echo '---Read ---'
    dd if=$FILENAME of=/dev/null bs=$BLOCKSIZE count=$BLOCKCOUNT
    #scp $FILENAME localhost:/dev/null

    #rm -f $FILENAME
done