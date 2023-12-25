#!/bin/bash

# Get the default network interface
interface=$(ip route | grep '^default' | awk '{print $5}' | head -n 1)

if [ -z "$interface" ]; then
    echo "No active network interface found."
    exit 1
fi

echo $interface | xargs ./network_ping kele
