#!/bin/bash

log_file="/home/$1/PSO-P/log.txt"

interface=$(ip route | grep '^default' | awk '{print $5}' | head -n 1)

if [ -z "$interface" ]; then
    echo "No active network interface found."
    exit 1
fi

echo "Interface found: $interface. Starting pinging on this interface..." >> $log_file
echo $interface | xargs network_ping $1
