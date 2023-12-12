#!/usr/bin/bash
if [ $SUDO_USER ]; then user=$SUDO_USER; else user=`whoami`; fi

log_file="/home/$user/PSO-P/log.txt"

echo "STARTING ALL SCRIPTS..." >> "$log_file"
ping-script.sh
backup-script.sh
#file-transfer-script.sh
cleanup-script.sh

echo "##################" >> "$log_file"
echo "####UDEV ENDED####" >> "$log_file"
echo "##################" >> "$log_file"
