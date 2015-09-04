FILENAME=/tmp/test
BLOCKSIZE=1M
BLOCKCOUNT=100
TESTS=5

for (( I=1;I<=$TESTS;I++ )) ; do
    echo '--- Write ---'
    dd if=/dev/urandom of=$FILENAME bs=$BLOCKSIZE count=$BLOCKCOUNT conv=fsync

    sync && sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"
    #ls -lh $FILENAME

    echo '---Read ---'
    dd if=$FILENAME of=/dev/null bs=$BLOCKSIZE count=$BLOCKCOUNT

    rm -f $FILENAME
done
