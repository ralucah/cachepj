#!/bin/bash

### IPs ###
IPS=("192.168.224.141" "192.168.224.141" "192.168.224.141")
TAGS=("Ren" "Sop" "Lux")
###########

TESTS=10
P=2
K=6
FILES=(1 4 8 16 32 64 128 256)
# mkdir -p /media/ramdsk
# mount -t tmpfs -o size=1024M tmpfs /media/ramdsk
# mount
RAMDISK="/media/ramdsk"

function get_full {
    for F in ${FILES[@]}
    do
        printf "$2 $F "
        for (( i=0; i< $TESTS; i++ ))
        do
            rm $RAMDISK/*mb* 2>/dev/null
            OUT1=`TIMEFORMAT="%R"; time ( aria2c --dir=$RAMDISK  -x $P -s $P http://$1/${F}mb ) 2>&1`
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