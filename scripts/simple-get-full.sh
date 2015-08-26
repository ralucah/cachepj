#!/bin/bash

IP1="52.25.139.47"
IP2="52.28.116.209"
IP3="54.64.186.40"

TESTS=5
K=6

if [ $# -ne 1 ] ; then
    echo "Arg: file size (MB)"
    exit 1
fi
FILE=$1
#####################################



RAMDISK="/media/ramdsk"
#mkdir -p $RAMDISK
#sudo mount -t tmpfs -o size=1024M tmpfs $RAMDISK

function get_full() {
    IP=$1
    P=$2
    rm $RAMDISK/* 2>/dev/null
    START=`date +%s%N | cut -b1-13`
    aria2c -q --dir=$RAMDISK  -x $P -s $P http://$IP/$FILE
    STOP=`date +%s%N | cut -b1-13`
    MS=$(( $STOP - $START ))
    SEC=`bc <<< "scale = 3; $MS / 1000"`
    printf "%0.3f" $SEC
}

echo "P2SITE1 P2SITE2 P2SITE3 P1SITE1 P1SITE2 P1SITE3"
for (( T=1;T<=$TESTS;T++ )) ; do
    get_full $IP1 2
    printf " "
    get_full $IP2 2
    printf " "
    get_full $IP3 2
    printf " "

    get_full $IP1 1
    printf " "
    get_full $IP2 1
    printf " "
    get_full $IP3 1
    printf "\n"
done