#!/usr/bin/bash

log_file="/home/$1/PSO-P/log.txt"
scripts="/usr/local/bin/*"
udevrule="/etc/udev/rules.d/80-local.rules"
systemdservice="/home/$1/.config/systemd" #todo: doar main.service si din acel folder tot main.service

sudo rm -r $scripts
sudo rm -r $udevrule
sudo rm -r $systemdservice

echo "CLEANED FILES ASSOCIATED WITH THESE EVENTS!" >> $log_file
