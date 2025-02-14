#!/bin/sh

# Steam Jailer Installation Script

echo "Starting Steam Jailer installation..."

# Step 1: Set up Steam Jailer
echo "Setting up Steam Jailer..."
./jailer_setup

# Step 2: Install Wine-Proton and Winetricks inside the jail
echo "Running Steam setup..."
./steam_setup.sh

echo "Installation complete. Thanks for all the pizza!"