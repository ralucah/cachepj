#!/bin/bash

IPS=("172.16.17.9" "172.16.35.8" "172.16.97.18")
TAGS=("Grenoble" "Lille" "Rennes")
TESTS=10

K=6
P=1
PARRAY=($P $P $P)
CHUNKS=(1 4 8 16 32 64 128)

#function sleepy() { echo "$1 $2 $3 sleepy" ;  sleep 5s ; }
#export -f sleepy

#function wgetbug() { echo $1 ; wget $1 ; }
#export -f wgetbug

# args: url, k, p
function get_chunks_parallel() {
    if [ $2 -ne 0 ] ; then
        parallel --gnu -n0 -P$3 wget -qO- $1 &> /dev/null ::: `seq 1 $2` #1>&/dev/null
    fi
}
export -f get_chunks_parallel

# args: karray
function get_chunks() {
    if [ $# -ne 3 ] ; then echo "get_chunks expects 1 arg: karray" ; exit 1 ; fi
    KARRAY=$1
    TAG="${TAGS[0]}${KARRAY[0]}_${TAGS[1]}${KARRAY[1]}_${TAGS[2]}${KARRAY[2]}"

    for C in ${CHUNKS[@]} ; do
        printf "$TAG $C"

        for (( I=0;I<${#IPS[@]};I++ )) ; do
            URLS[$I]="http://${IPS[$I]}/$C"
        done
        if [ ${#URLS[@]} -ne 3 ] ; then echo ${URLS[@]} ; exit 1 ; fi

        for (( T=1; T<=${TESTS}; T++ ))
        do
            #rm *mb* 2>/dev/null
            TIME=`TIMEFORMAT="%R"; time ( parallel -P${#URLS[@]} --gnu --xapply get_chunks_parallel ::: ${URLS[@]} ::: ${KARRAY[@]} ::: ${PARRAY[@]}) 2>&1`
            printf " $TIME"
        done
        printf "\n"
    done
}
export -f get_chunks

KARRAY=($K 0 0)
get_chunks ${KARRAY[@]}
echo ""

KARRAY=(0 $K 0)
get_chunks ${KARRAY[@]}
echo ""

KARRAY=(0 0 $K)
get_chunks ${KARRAY[@]}
echo ""

DIV=$(( $K / 3))
KARRAY=($DIV $DIV $DIV)
get_chunks ${KARRAY[@]}