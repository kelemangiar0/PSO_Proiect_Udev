#!/usr/bin/bash
if [ $SUDO_USER ]; then user=$SUDO_USER; else user=`whoami`; fi

echo "STARTING ALL SCRIPTS..." >> /home/$user/PSO-P/log.txt
ping-script.sh
#backup-script.sh
cleanup-script.sh
echo "##################" >> /home/$user/PSO-P/log.txt
echo "####UDEV ENDED####" >> /home/$user/PSO-P/log.txt
echo "##################" >> /home/$user/PSO-P/log.txt
