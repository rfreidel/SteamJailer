#!/bin/sh

# Strict error handling
set -e

# Check if running as root
if [ "$(id -u)" != "0" ]; then
    echo "This script must be run as root" 1>&2
    exit 1
fi

# Update package repository
/usr/sbin/pkg update

# Install required packages
/usr/sbin/pkg install -y \
    py311-iocage \
    wine-proton \
    winetricks

# Compile the installer
make clean
make

# Set permissions
chmod 755 ./installer

# Run the installer
./installer