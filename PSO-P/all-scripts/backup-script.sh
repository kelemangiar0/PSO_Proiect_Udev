#!/usr/bin/bash
if [ $SUDO_USER ]; then user=$SUDO_USER; else user=`whoami`; fi

echo "STARTING BACKUP..." >> /home/$user/PSO-P/log.txt
mount /dev/sdb1 /home/$user/PSO-P/mount-folder
sleep 2
rsync -az /home/$user/PSO-P/mount-folder /home/$user/PSO-P/backup-folder && umount /dev/sdb1
echo "BACKUP ENDED SUCCESFULLY!" >> /home/$user/PSO-P/log.txt
