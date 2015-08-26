#!/bin/bash

HTTPD="/home/ubuntu/httpd-2.4.16"
SUDO="sudo"

#$SUDO apt-get install wget
wget http://apache.crihan.fr/dist//httpd/httpd-2.4.16.tar.gz
tar xvfz httpd-2.4.16.tar.gz

$SUDO apt-get update
$SUDO apt-get install bc libapr1-dev libaprutil1-dev make curl wget aria2 parallel #sudo
cd $HTTPD
pwd
./configure
make
$SUDO make install

# sudo nano /usr/local/apache2/conf/httpd.conf
# search for Listen

#sudo /usr/local/apache2/bin/apachectl -k start