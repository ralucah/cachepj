#!/bin/bash

### IPs ###
IPS=("172.16.98.7" "172.16.130.34" "172.16.177.12")
TAGS=("Ren" "Sop" "Lux")
###########

TESTS=10
P=2
K=6
FILES=(1 4 8 16 32 64 128 256)

function get_full {
    for F in ${FILES[@]}
    do
        printf "$2 $F "
        for (( i=0; i< $TESTS; i++ ))
        do
            rm *mb* 2>/dev/null
            #OUT1=`TIMEFORMAT="%R"; time ( wget $1/${F}mb ) 2>&1`
            OUT1=`TIMEFORMAT="%R"; time ( aria2c -x $P -s $P http://$1/${F}mb ) 2>&1`
            REAL1=`echo $OUT1 | awk -F: '{print $NF}'`
            printf "${REAL1##* } "
            GETS=`ls -l *mb* | wc -l`
            if [ $(( $GETS )) -eq 0 ] ; then
                echo "$GETS-chunks-only"
                exit
            fi
        done
        printf "\n"
    done
}

##########
get_full ${IPS[0]} ${TAGS[0]} 
get_full ${IPS[1]} ${TAGS[1]} 
get_full ${IPS[2]} ${TAGS[2]} 