#!/bin/sh

JAIL_NAME="steamjailer_$(date +%s)"  # Adding a timestamp to avoid name conflicts
ZFS_POOL="zroot"

# Function to log messages
log_message() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') $1"
}

# Step 1: Install ezjail and enable it in rc.conf
log_message "Installing ezjail..."
sudo pkg install -y ezjail

log_message "ezjail has installed"
sudo ezjail-admin install -m

log_message "ezjail-admin has installed"
sudo ezjail-admin update -p -i

log_message "ezjail-admin update has completed"

# Creating the Steam jail
log_message "Creating the Steam jail..."
sudo ezjail-admin create -f steam "$JAIL_NAME" '127.0.0.1'

log_message "Steam jail has been built"

log_message "Enabling ezjail in rc.conf..."
echo 'ezjail_enable="YES"' | sudo tee -a /etc/rc.conf

# Check if ZFS dataset exists
log_message "Checking ZFS dataset status..."
if ! zfs list | grep -q "${ZFS_POOL}/jails"; then
    log_message "ZFS dataset '${ZFS_POOL}/jails' does not exist. Creating ZFS dataset..."
    sudo zfs create ${ZFS_POOL}/jails || { log_message "Failed to create ZFS dataset '${ZFS_POOL}/jails'." ; exit 1; }
fi

# Set the jailed property for the dataset
log_message "Setting jailed property for the dataset..."
sudo zfs set jailed=on ${ZFS_POOL}/jails || { log_message "Failed to set jailed property for the dataset." ; exit 1; }

# Ensure /etc/devfs.rules exists
if [ ! -f /etc/devfs.rules ]; then
    log_message "/etc/devfs.rules does not exist. Creating it..."
    sudo touch /etc/devfs.rules
fi

# Allow the jail access to /dev/zfs
log_message "Allowing jail access to /dev/zfs..."
if ! grep -q "jail_$JAIL_NAME" /etc/devfs.rules; then
    echo "[devfsrules_jail_$JAIL_NAME=10]" | sudo tee -a /etc/devfs.rules
    echo "add path 'zfs' unhide" | sudo tee -a /etc/devfs.rules
fi

# Restart devfs service
log_message "Restarting devfs service..."
sudo service devfs restart

# Configure ezjail to use ZFS
log_message "Configuring ezjail to use the ZFS dataset..."
echo "export jail_${JAIL_NAME}_zfs=\"YES\"" | sudo tee -a /usr/local/etc/ezjail.conf
echo "export jail_${JAIL_NAME}_zfs_mountpoint=\"${ZFS_POOL}/jails/${JAIL_NAME}\"" | sudo tee -a /usr/local/etc/ezjail.conf

# Step 2: Create a new jail using ezjail-admin
log_message "Creating the jail '$JAIL_NAME'..."
if sudo ezjail-admin create -c zfs -s 2G "$JAIL_NAME" '127.0.0.1'; then
    log_message "Jail '$JAIL_NAME' created successfully."
else
    log_message "Failed to create the jail '$JAIL_NAME'."
    log_message "zfs list output:"
    zfs list
    log_message "zpool status output:"
    zpool status
    exit 1
fi

# Step 3: Configure the jail to use the ZFS dataset
log_message "Configuring the jail to use the ZFS dataset..."
echo "export jail_${JAIL_NAME}_zfs_datasets=\"${ZFS_POOL}/jails\"" | sudo tee -a /usr/local/etc/ezjail/"$JAIL_NAME"

# Step 4: Start the jail
log_message "Starting the jail '$JAIL_NAME'..."
if sudo ezjail-admin start "$JAIL_NAME"; then
    log_message "Jail '$JAIL_NAME' started successfully."
else
    log_message "Failed to start the jail '$JAIL_NAME'."
    log_message "zfs list output:"
    zfs list
    log_message "zpool status output:"
    zpool status
    exit 1
fi

# Step 5: Install packages in the jail
log_message "Installing packages in the jail '$JAIL_NAME'..."
sudo /usr/local/sbin/jexec "$JAIL_NAME" pkg update || { log_message "Failed to update pkg repository." ; exit 1; }
sudo /usr/local/sbin/jexec "$JAIL_NAME" pkg install -y wine-proton winetricks zenity || { log_message "Failed to install required packages." ; exit 1; }

log_message "Running winetricks to install additional components..."
sudo /usr/local/sbin/jexec "$JAIL_NAME" WINE=/usr/local/wine-proton/bin/wine winetricks cmd dxvk vkd3d nvapi vcrun2022 || { log_message "Failed to run winetricks." ; exit 1; }

log_message "Downloading Steam setup..."
sudo fetch -o /tmp/SteamSetup.exe https://cdn.akamai.steamstatic.com/client/installer/SteamSetup.exe || { log_message "Failed to download Steam setup." ; exit 1; }

log_message "Installing Steam setup..."
sudo /usr/local/sbin/jexec "$JAIL_NAME" WINEPREFIX=~/.steam-games /usr/local/wine-proton/bin/wine /tmp/SteamSetup.exe || { log_message "Failed to install Steam setup." ; exit 1; }

log_message "Applying registry patches..."
sudo /usr/local/sbin/jexec "$JAIL_NAME" WINEPREFIX=~/.steam-games /usr/local/wine-proton/bin/wine reg.exe ADD "HKEY_CURRENT_USER\\Software\\Wine\\DllOverrides" /v "gameoverlayrenderer" /t "REG_SZ" /d "" /f || { log_message "Failed to apply registry patch for gameoverlayrenderer." ; exit 1; }
sudo /usr/local/sbin/jexec "$JAIL_NAME" WINEPREFIX=~/.steam-games /usr/local/wine-proton/bin/wine reg.exe ADD "HKEY_CURRENT_USER\\Software\\Wine\\DllOverrides" /v "gameoverlayrenderer64" /t "REG_SZ" /d "" /f || { log_message "Failed to apply registry patch for gameoverlayrenderer64." ; exit 1; }

log_message "Verifying the installation of wine-proton, winetricks, and Steam..."
if sudo /usr/local/sbin/jexec "$JAIL_NAME" pkg info | grep -q "wine-proton"; then
    log_message "wine-proton is installed successfully in the jail '$JAIL_NAME'."
else
    log_message "Failed to install wine-proton in the jail '$JAIL_NAME'."
    exit 1
fi

log_message "Setting up environment variables and initializing Wine..."
sudo /usr/local/sbin/jexec "$JAIL_NAME" sh -c "env WINEPREFIX=~/.steam-games WINE=/usr/local/wine-proton/bin/wine WINE64=/usr/local/wine-proton/bin/wine WINEARCH=win64 wineboot --init" || { log_message "Failed to set up environment variables and initialize Wine." ; exit 1; }

log_message "Installation and setup completed successfully."