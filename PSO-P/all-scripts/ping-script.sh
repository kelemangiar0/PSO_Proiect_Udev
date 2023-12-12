#!/usr/bin/bash

ip_adress="127.0.0.0"
log_file="/home/$1/PSO-P/log.txt"

echo "PINGING...\n" >> $log_file
ping -c 4 $ip_adress | tail -n 3 >> $log_file
echo "PINGING ENDED.\n" >> $log_file
