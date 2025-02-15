#!/bin/sh

# Steam Setup Script

JAIL_NAME="steamjailer"
STEAM_INSTALLER_URL="https://cdn.fastly.steamstatic.com/client/installer/SteamSetup.exe"
STEAM_INSTALLER_PATH="/tmp/Steam.exe"

# Ensure the jail is running
if ! iocage list | grep -q "^$JAIL_NAME.*up"; then
    echo "Jail $JAIL_NAME is not running! Please start the jail and try again."
    exit 1
fi

# Install Wine-Proton and Winetricks in the jail
echo "Installing Wine-Proton and Winetricks in the jail..."
iocage exec $JAIL_NAME pkg install -y wine-proton winetricks

# Update Winetricks
echo "Updating Winetricks..."
iocage exec $JAIL_NAME sh -c "WINE=/usr/local/wine-proton/bin/wine winetricks --self-update"

# Use Winetricks to install required components for Steam
echo "Installing required components for Steam..."
iocage exec $JAIL_NAME sh -c "WINE=/usr/local/wine-proton/bin/wine winetricks cmd vcrun2022 dxvk nvapi"

# Download Steam installer
echo "Downloading Steam installer..."
iocage exec $JAIL_NAME fetch -o $STEAM_INSTALLER_PATH $STEAM_INSTALLER_URL

# Install Steam
echo "Installing Steam..."
iocage exec $JAIL_NAME sh -c "sh $STEAM_INSTALLER_PATH"

# Wait for Steam installation to complete
echo "Waiting for Steam installation to complete..."
sleep 60

# Kill all Wine processes
echo "Killing all Wine processes..."
iocage exec $JAIL_NAME pkill wine

# Notify user to launch Steam launcher
echo "Steam installation complete. Please launch the Steam launcher `jailer' to start Steam."

# Gracefully exit
echo "Exiting setup script..."
exit 0
