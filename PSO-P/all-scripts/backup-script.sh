#!/usr/bin/bash

log_file="/home/$1/PSO-P/log.txt"
mount_folder="/home/$1/PSO-P/mount-folder"
backup_folder="/home/$1/PSO-P/backup-folder"
mount_device="/dev/$(lsblk -o NAME,TYPE,MOUNTPOINT | grep 'disk\|part' | tail -n 1 | cut -c 7-10)"


echo "STARTING BACKUP..." >> "$log_file"
mount "$mount_device" "$mount_folder" 
sleep 2
rsync -az "$mount_folder/" "$backup_folder" && umount "$mount_device"
echo "BACKUP ENDED SUCCESFULLY!" >> "$log_file"

