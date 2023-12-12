#!/usr/bin/bash
#if [ $SUDO_USER ]; then user=$SUDO_USER; else user=`whoami`; fi #nu merge

#sa incerc sa il transmit ca parametru din systemd?
log_file="/home/$1/PSO-P/log.txt"

echo "##################" >> "$log_file"
echo "##UDEV TRIGGERED##" >> "$log_file"
echo "##################" >> "$log_file"

sudo -u $1 XDG_RUNTIME_DIR=/run/user/$(id -u $1) systemctl --user start main.service
exit 0
