#!/bin/bash

IPS=("172.16.98.7" "172.16.130.34" "172.16.177.12")
TESTS=10

K=6
P=2
PARRAY=($P $P $P)
FILES=(1 4 8 16 32 64 128 256)

#function sleepy() { echo "$1 $2 $3 sleepy" ;  sleep 5s ; }
#export -f sleepy

#function wgetbug() { echo $1 ; wget $1 ; }
#export -f wgetbug

# args: url, k, p
function get_chunks_parallel() {
    if [ $2 -ne 0 ] ; then
        parallel --gnu -n0 -P$3 wget $1 ::: `seq 1 $2` 1>&/dev/null
    fi
}
export -f get_chunks_parallel

# args: karray
function get_chunks() {
    if [ $# -ne 3 ] ; then echo "get_chunks expects 1 arg: karray" ; exit 1 ; fi
    KARRAY=$1
    TAG="Ren${KARRAY[0]}_Sop${KARRAY[1]}_Lux${KARRAY[2]}"

    for F in ${FILES[@]} ; do
        printf "$TAG ${F}mb_chunk "

        for (( I=0;I<${#IPS[@]};I++ )) ; do
            URLS[$I]="http://${IPS[$I]}/${F}mb_chunk"
        done
        if [ ${#URLS[@]} -ne 3 ] ; then echo ${URLS[@]} ; exit 1 ; fi

        for (( T=1; T<=${TESTS}; T++ ))
        do
            rm *mb* 2>/dev/null

            #echo "parallel -P${#URLS[@]} --gnu --xapply get_chunks_parallel ::: ${URLS[@]} ::: ${KARRAY[@]} ::: ${PARRAY[@]}"
            #parallel -P${#URLS[@]} --gnu --xapply get_chunks_parallel ::: ${URLS[@]} ::: ${KARRAY[@]} ::: ${PARRAY[@]}
            TIME=`TIMEFORMAT="%R"; time ( parallel -P${#URLS[@]} --gnu --xapply get_chunks_parallel ::: ${URLS[@]} ::: ${KARRAY[@]} ::: ${PARRAY[@]}) 2>&1`
            printf "$TIME "

            GETS=`ls -l *mb* | wc -l`
            if [ $(( $GETS )) -eq 0 ] ; then
                echo "$GETS-chunks-only"
                exit
            fi
        done
        printf "\n"
    done
}
export -f get_chunks

KARRAY=(6 0 0)
get_chunks ${KARRAY[@]}
#echo ""

KARRAY=(0 6 0)
get_chunks ${KARRAY[@]}
#echo ""

KARRAY=(0 0 6)
get_chunks ${KARRAY[@]}
#echo ""

KARRAY=(2 2 2)
get_chunks ${KARRAY[@]}