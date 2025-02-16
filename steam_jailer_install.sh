#!/bin/sh

# Steam Jailer Installation Script

echo "Starting Steam Jailer installation..."

# Step 1: Set up Steam Jailer
echo "Setting up Steam Jailer..."
./steam_jailer_setup.sh

# Step 2: Install Wine-Proton and Winetricks inside the jail
echo "Running Steam setup..."
c++ steamjailerInstaller.cpp -o jailer
./jailer

echo "Installation complete. Thanks for all the pizza!"
