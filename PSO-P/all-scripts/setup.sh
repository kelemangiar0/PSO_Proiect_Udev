#!/usr/bin/bash
if [ $SUDO_USER ]; then user=$SUDO_USER; else user=`whoami`; fi


sudo cp * /usr/local/bin
sudo rm /home/$user/PSO-P/log.txt
sudo touch /home/$user/PSO-P/log.txt
sudo chown $user /home/$user/PSO-P/log.txt

#regula udev
sudo touch /etc/udev/rules.d/80-local.rules
sudo echo "SUBSYSTEM==\"block\", ACTION==\"add\", RUN+=\"/usr/local/bin/trigger.sh\"" | sudo tee /etc/udev/rules.d/80-local.rules > /dev/null
sudo udevadm control --reload

#serviciu systemd
mkdir -p /home/$user/.config/systemd/user
sudo touch /home/$user/.config/systemd/user/main.service
echo "[Unit]
Description=Service called when the HD with idVendor=0480 and idProduct=a00d is plugged in

[Service]
Type=simple
ExecStart=/usr/bin/gnome-terminal -- sudo /usr/local/bin/main-script.sh

[Install]
WantedBy=default.target" | sudo tee /home/$user/.config/systemd/user/main.service > /dev/null
systemctl --user daemon-reload
systemctl --user enable main.service
