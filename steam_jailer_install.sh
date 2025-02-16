#!/bin/sh

# Steam Jailer Installation Script

echo "Starting Steam Jailer installation..."

# Step 1: Set up the Steam Jailer
c++ steamjailer_installer.cpp -o jailer


# Step 2: Install Wine-Proton and Winetricks inside the jail
echo "Running Steam setup..."

./jailer

echo "Installation complete. Thanks for all the pizza!"
