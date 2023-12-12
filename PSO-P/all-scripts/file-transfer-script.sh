#!/usr/bin/bash
if [ $SUDO_USER ]; then user=$SUDO_USER; else user=`whoami`; fi

log_file="/home/$user/PSO-P/log.txt"
server_ip="192.168.173.216"
us="kele"
backup_folder="/home/$user/PSO-P/backup-folder"

echo "TRANSMITTING DATA OVER $server_ip...." >> "$log_file"
scp -r "$backup_folder" "$us@$server_ip:/home/kele/ssh"
echo "DONE!" >> "$log_file"