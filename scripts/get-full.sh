#!/bin/bash

### IPs ###
IPS=("172.16.17.9" "172.16.35.8" "172.16.97.18")
TAGS=("G" "L" "R")
###########

TESTS=10
P=2
K=6
CHUNKS=(1 4 8 16 32 64 128)
# mkdir -p /media/ramdsk
# mount -t tmpfs -o size=1024M tmpfs /media/ramdsk
# mount
RAMDISK="/media/ramdsk"

function get_full {
    for C in ${CHUNKS[@]}
    do
        FULL=$(( $C * $K))
        printf "$2 $FULL "
        for (( i=0; i< $TESTS; i++ ))
        do
            rm $RAMDISK/*mb* 2>/dev/null
            OUT1=`TIMEFORMAT="%R"; time ( aria2c --dir=$RAMDISK  -x $P -s $P http://$1/$FULL ) 2>&1`
            REAL1=`echo $OUT1 | awk -F: '{print $NF}'`
            printf " ${REAL1##* }"
        done
        printf "\n"
    done
}

##########
get_full ${IPS[0]} ${TAGS[0]} 
get_full ${IPS[1]} ${TAGS[1]} 
get_full ${IPS[2]} ${TAGS[2]} 