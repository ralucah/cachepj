#!/bin/bash

HTTPD="/root/httpd-2.4.16"

apt-get install wget
tar xvfz httpd-2.4.16.tar.gz
apt-get update #sudo
apt-get install libapr1-dev libaprutil1-dev make curl wget aria2 parallel #sudo
cd $HTTPD
pwd
./configure
make
make install #sudo

#wget http://apache.crihan.fr/dist//httpd/httpd-2.4.16.tar.gz
# sudo nano /usr/local/apache2/conf/httpd.conf
# search for Listen

#sudo /usr/local/apache2/bin/apachectl -k start
#sudo /usr/local/apache2/bin/apachectl -k status