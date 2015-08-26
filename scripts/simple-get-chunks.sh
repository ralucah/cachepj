#!/bin/bash

IP1="192.168.224.231"
IP2="192.168.224.231"
IP3="192.168.224.231"
FILE=128
K=6
#####################################

# get from one site; p=2
function one_site_p2() {
    IP=$1
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done1) &
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done2)
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done3) &
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done4)
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done5) &
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done6)
}

# get from all sites; p=2
function all_sites_p2() {
    (wget -qO - http://$IP1/$FILE &> /dev/null && touch done1) &
    (wget -qO - http://$IP1/$FILE &> /dev/null && touch done2) &
    (wget -qO - http://$IP2/$FILE &> /dev/null && touch done3) &
    (wget -qO - http://$IP2/$FILE &> /dev/null && touch done4) &
    (wget -qO - http://$IP3/$FILE &> /dev/null && touch done5) &
    (wget -qO - http://$IP3/$FILE &> /dev/null && touch done6) &
}

# get from one site; p=1
function one_site_p1() {
    IP=$1
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done1)
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done2)
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done3)
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done4)
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done5)
    (wget -qO - http://$IP/$FILE &> /dev/null && touch done6)
}

# get from all sites; p=1
function all_sites_p1() {
    (wget -qO - http://$IP1/$FILE &> /dev/null && touch done1) &
    (wget -qO - http://$IP2/$FILE &> /dev/null && touch done2) &
    (wget -qO - http://$IP3/$FILE &> /dev/null && touch done3)
    (wget -qO - http://$IP1/$FILE &> /dev/null && touch done4) &
    (wget -qO - http://$IP2/$FILE &> /dev/null && touch done5) &
    (wget -qO - http://$IP3/$FILE &> /dev/null && touch done6)
}

#####################################

rm *$FILE* 2>/dev/null
rm *done* 2>/dev/null

# start timer
START=`date +%s%N | cut -b1-13`

# call function
#one_site_p2 $IP1
all_sites_p2
#one_site_p1 $IP1
#all_sites_p1

# busy waiting
DONE=`ls -l | grep done | wc -l`
while [ $DONE -ne $K ] ; do
    DONE=`ls -l | grep done | wc -l`
done
STOP=`date +%s%N | cut -b1-13`
MS=$(( $STOP - $START ))
SEC=`bc <<< "scale = 3; $MS / 1000"`
#printf "$MS "
printf "%0.3f\n" $SEC