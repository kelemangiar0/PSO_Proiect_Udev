#!/usr/bin/bash

log_file="/home/$1/PSO-P/log.txt"

echo "STARTING ALL SCRIPTS..." >> $log_file
ping-script.sh $1
backup-script.sh $1
file-transfer-script.sh $1
cleanup-script.sh $1

echo "##################" >> $log_file
echo "####UDEV ENDED####" >> $log_file
echo "##################" >> $log_file
