#!/usr/bin/bash

log_file="/home/$1/PSO-P/log.txt"
recieverIP="192.168.173.10"
recieverUSER="student"
toTransmitFolder="/home/$1/PSO-P/backup-folder"
toRecieveFolder="/home/$recieverUSER/testssh"

echo "TRANSMITTING DATA OVER $recieverIP...." >> "$log_file"
scp -r "$toTransmitFolder" "$recieverUSER@$recieverIP:$toRecieveFolder"
echo "DONE!" >> "$log_file"
