#!/usr/bin/bash
if [ $SUDO_USER ]; then user=$SUDO_USER; else user=`whoami`; fi

sudo rm -r /usr/local/bin/*
echo "CLEANED /usr/local/bin" >> /home/$user/PSO-P/log.txt
