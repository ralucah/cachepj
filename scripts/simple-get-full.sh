#!/bin/bash

IP1="52.25.139.47"
IP2="52.28.116.209"
IP3="54.64.186.40"

TESTS=5
P=2
K=6

if [ $# -ne 1 ] ; then
    echo "Arg: file size (MB)"
    exit 1
fi
FILE=$1
#####################################



RAMDISK="/media/ramdsk"
mkdir -p $RAMDISK
sudo mount -t tmpfs -o size=1024M tmpfs /media/ramdsk
mount

function get_full() {
    IP=$1
    START=`date +%s%N | cut -b1-13`
    aria2c --dir=$RAMDISK  -x $P -s $P http://$IP/$FULL
    STOP=`date +%s%N | cut -b1-13`
    MS=$(( $STOP - $START ))
    SEC=`bc <<< "scale = 3; $MS / 1000"`
    printf "%0.3f" $SEC
}

echo "SITE1 SITE2 SITE3"
for (( T=1;T<=$TESTS;T++ )) ; do
    get_full $IP1
    printf " "
    get_full $IP2
    printf " "
    get_full $IP3
    printf "\n"
done