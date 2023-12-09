#!/usr/bin/bash
if [ $SUDO_USER ]; then user=$SUDO_USER; else user=`whoami`; fi

echo "PINGING...\n" >> /home/$user/PSO-P/log.txt

echo "PINGING ENDED.\n" >> /home/$user/PSO-P/log.txt
