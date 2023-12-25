#!/usr/bin/bash

log_file="/home/$1/PSO-P/log.txt"


echo "STARTING ALL SCRIPTS..." >> $log_file

echo "===ping-script.sh===" >> $log_file
ping-script.sh $1
echo "===backup-script.sh===" >> $log_file
backup-script.sh $1
echo "===client sending data...===" >> $log_file
client $1 "192.168.1.9"
#file-transfer-script.sh $1
echo "===cleanup-script.sh===" >> $log_file
cleanup-script.sh $1

echo "##################" >> $log_file
echo "####UDEV ENDED####" >> $log_file
echo "##################" >> $log_file
