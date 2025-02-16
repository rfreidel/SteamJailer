#!/bin/sh
set -e

# Check for root privileges
if [ "$(/usr/bin/id -u)" != "0" ]; then
    echo "This script must be run as root" >&2
    exit 1
fi

# Enable iocage
/usr/sbin/sysrc -q iocage_enable="YES"

# Start iocage service
/usr/sbin/service iocage onestart || true

# Update package repository
/usr/sbin/pkg update -f

# Install required packages
/usr/sbin/pkg install -y \
    py311-iocage \
    wine-proton \
    winetricks