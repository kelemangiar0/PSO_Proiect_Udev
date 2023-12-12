#!/usr/bin/bash
if [ $SUDO_USER ]; then user=$SUDO_USER; else user=`whoami`; fi

ip_adress="192.168.173.216"
log_file="/home/$user/PSO-P/log.txt"

echo "PINGING...\n" >> "$log_file"
ping -c 4 "$ip_adress" | tail -n 3 >> "$log_file"
echo "PINGING ENDED.\n" >> "$log_file"
