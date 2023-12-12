#!/usr/bin/bash
#if [ $SUDO_USER ]; then user=$SUDO_USER; else user=`whoami`; fi #nu merge

#sa incerc sa il transmit ca parametru din systemd?
user="student"
log_file="/home/$user/PSO-P/log.txt"


echo $user | sudo tee home/$user/PSO-P/log.txt
echo "##################" >> "$log_file"
echo "##UDEV TRIGGERED##" >> "$log_file"
echo "##################" >> "$log_file"

sudo -u $user XDG_RUNTIME_DIR=/run/user/$(id -u $user) systemctl --user start main.service
exit 0
