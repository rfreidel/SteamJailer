#!/bin/sh

# wine_setup.sh - Wine-Proton setup script for FreeBSD jail
# This script must be run inside the jail

set -e

# Install required packages
pkg install -y wine-proton winetricks

# Set up Wine environment
WINE=/usr/local/wine-proton/bin/wine
WINEPREFIX=/home/wine/.wine

# Install Winetricks components
winetricks dxvk dxvk_nvapi d3dcompiler_47 dotnet7 faudio openal corefonts vcrun2022

# Wait for Winetricks to complete
sleep 5

# Configure Wine
$WINE config

# Create Steam installation directory
mkdir -p /home/wine/Steam

# Download Steam installer
fetch -o /home/wine/Steam/SteamSetup.exe https://cdn.akamai.steamstatic.com/client/installer/SteamSetup.exe

# Install Steam
$WINE /home/wine/Steam/SteamSetup.exe /S

echo "Wine and Steam setup completed successfully"