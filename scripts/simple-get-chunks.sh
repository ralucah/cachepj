#!/bin/bash

IP1="52.25.139.47"
IP2="52.28.116.209"
IP3="54.64.186.40"

K=6
TESTS=5
if [ $# -ne 1 ] ; then
    echo "Arg: file size (MB)"
    exit 1
fi
FILE=$1
#####################################

function clean() {
    rm *done* 2>/dev/null
}
export -f clean

# get from one site; p=2
function one_site_p2() {
    clean
    IP=$1
    START=`date +%s%N | cut -b1-13`

    (wget -qO - http://$IP/$FILE &> /dev/null && touch done1) &
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done2)
    busy_wait 2

    (wget -qO - http://$IP/$FILE &> /dev/null && touch done3) &
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done4)
    busy_wait 2

    (wget -qO - http://$IP/$FILE &> /dev/null && touch done5) &
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done6)
    busy_wait 2

    STOP=`date +%s%N | cut -b1-13`
    MS=$(( $STOP - $START ))
    SEC=`bc <<< "scale = 3; $MS / 1000"`
    printf "%0.3f" $SEC
}

# get from all sites; p=2
function all_sites_p2() {
    clean
    START=`date +%s%N | cut -b1-13`

    (wget -qO - http://$IP1/$FILE &> /dev/null && touch done1) &
    (wget -qO - http://$IP1/$FILE &> /dev/null && touch done2) &
    (wget -qO - http://$IP2/$FILE &> /dev/null && touch done3) &
    (wget -qO - http://$IP2/$FILE &> /dev/null && touch done4) &
    (wget -qO - http://$IP3/$FILE &> /dev/null && touch done5) &
    (wget -qO - http://$IP3/$FILE &> /dev/null && touch done6) &

    busy_wait 6
    STOP=`date +%s%N | cut -b1-13`
    MS=$(( $STOP - $START ))
    SEC=`bc <<< "scale = 3; $MS / 1000"`
    printf "%0.3f" $SEC
}

# get from one site; p=1
function one_site_p1() {
    clean
    IP=$1
    START=`date +%s%N | cut -b1-13`

    (wget -qO - http://$IP/$FILE &> /dev/null)
    (wget -qO - http://$IP/$FILE &> /dev/null)
    (wget -qO - http://$IP/$FILE &> /dev/null)
    (wget -qO - http://$IP/$FILE &> /dev/null)
    (wget -qO - http://$IP/$FILE &> /dev/null)
    (wget -qO - http://$IP/$FILE &> /dev/null)

    STOP=`date +%s%N | cut -b1-13`
    MS=$(( $STOP - $START ))
    SEC=`bc <<< "scale = 3; $MS / 1000"`
    printf "%0.3f" $SEC
}

# get from all sites; p=1
function all_sites_p1() {
    clean
    START=`date +%s%N | cut -b1-13`

    (wget -qO - http://$IP1/$FILE &> /dev/null && touch done1) &
    (wget -qO - http://$IP2/$FILE &> /dev/null && touch done2) &
    (wget -qO - http://$IP3/$FILE &> /dev/null && touch done3)
    busy_wait 3

    (wget -qO - http://$IP1/$FILE &> /dev/null && touch done4) &
    (wget -qO - http://$IP2/$FILE &> /dev/null && touch done5) &
    (wget -qO - http://$IP3/$FILE &> /dev/null && touch done6)
    busy_wait 3

    STOP=`date +%s%N | cut -b1-13`
    MS=$(( $STOP - $START ))
    SEC=`bc <<< "scale = 3; $MS / 1000"`
    printf "%0.3f" $SEC
}

function busy_wait() {
    NUM=$1
    DONE=`ls -l | grep done | wc -l`
    while [ $DONE -ne $NUM ] ; do
        DONE=`ls -l | grep done | wc -l`
    done
    clean
}
export -f busy_wait

#####################################

echo "P2SITE1 P2SITE2 P2SITE3 P2ALL P1SITE1 P1SITE2 P1SITE3 P1ALL"
for (( T=1;T<=$TESTS;T++ )) ; do
    clean

    one_site_p2 $IP1
    printf " "
    one_site_p2 $IP2
    printf " "
    one_site_p2 $IP3
    printf " "
    all_sites_p2
    printf " "

    one_site_p1 $IP1
    printf " "
    one_site_p1 $IP2
    printf " "
    one_site_p1 $IP3
    printf " "
    all_sites_p1
    printf "\n"
done