# Project: Steam Jailer

An ambitious two part project

The `steam_installer.sh` is fully functioning as of Wed Feb 12 18:34:52 PST 2025, I have yet to test `steam_jailer`'s functions with it, this little shell script was quite the undertaking, it install's ezjail as per the FreeBSD Foundation,

    zroot/jails            96K  1.44T    96K  /zroot/jails
    
sets up the zfs stuff required, identifies as shell based software, will identify what your install is on, restarts services devfs, ezjail, following the successful installion of ezjail and the setup of the jail the project then takes control of your computer, don't be concerned, control will return momentarily. The project then installs wine-proton winetricks and modules via a commented shell script where you can add or alter what the script installs in the jail

This is the jail created on my computer

    ronf@bsdbox:~/git/jailer/v1 $ sudo ezjail-admin list
STA JID  IP              Hostname                       Root Directory
--- ---- --------------- ------------------------------ ------------------------
DS  N/A  127.0.1.1       steamjailer_1739358384         /usr/jails/steamjailer_1739358384
DSN N/A  -                                              
ronf@bsdbox:~/git/jailer/v1 $ 

# Intention
This projects purpose is to automate the installation of Steam within a jail on FreeBSD, including the installation of wine-proton both system-wide and within the jail. The project will also provide options to install necessary patches and enable ezjail in the system's rc.conf. Finally, it will offer the ability to link the launcher to the user's $HOME/bin folder with a default program icon for FreeBSD. The project `steamjailer` provides an interface allowing control of Steam with-in the jail

It is beyond the scope of this project to provide a wine-proton capable of playing modern games, the current wine-proton in ports and packages is capable of playing SkyrimSE, Dark SoulsIII, Fallout4, etc. This project will grow depending on base package support

    # SteamJailer
     Build steam_jailer: c++ steam_launcher.py -o steam_jailer then ./steam_jailer
    

# Project Structure

Shell Script: To install and configure ezjail, create and start the jail (with the capability to create a new jail if one already exists), and install necessary packages inside the jail. 

C++ Program: To manage the installation and launching of Steam and games within the jail.

Step 1: Shell Script
Step 2: C++ Program
Public code references from 4 repositories

Step 2: Configure ezjail
sh

sudo ezjail-admin install -m
sudo ezjail-admin update -p -i

Step 3: Enable ezjail in rc.conf
sh

echo 'ezjail_enable="YES"' | sudo tee -a /etc/rc.conf

Step 4: Configure ZFS Dataset for Jails

Ensure that the ZFS dataset exists and is configured correctly.
sh

ZFS_POOL="zroot"
if ! zfs list | grep -q "${ZFS_POOL}/jails"; then
    sudo zfs create ${ZFS_POOL}/jails
fi
sudo zfs set jailed=on ${ZFS_POOL}/jails

Step 5: Configure devfs Rules

Ensure the /etc/devfs.rules file exists and configure it to allow jail access to /dev/zfs.
sh

if [ ! -f /etc/devfs.rules ]; then
    sudo touch /etc/devfs.rules
fi
JAIL_NAME="steamjailer"
if ! grep -q "jail_$JAIL_NAME" /etc/devfs.rules; then
    echo "[devfsrules_jail_$JAIL_NAME=10]" | sudo tee -a /etc/devfs.rules
    echo "add path 'zfs' unhide" | sudo tee -a /etc/devfs.rules
fi
sudo service devfs restart

Step 6: Create the Jail

Create and configure the jail to use the ZFS dataset.
sh

sudo ezjail-admin create -f steam "$JAIL_NAME" '127.0.0.1'

Step 7: Configure the Jail to Use the ZFS Dataset
sh

echo "export jail_${JAIL_NAME}_zfs_datasets=\"${ZFS_POOL}/jails\"" | sudo tee -a /usr/local/etc/ezjail/"$JAIL_NAME"

Step 8: Start the Jail
sh

sudo ezjail-admin start "$JAIL_NAME"

Step 9: Install Packages in the Jail
sh

sudo /usr/local/sbin/jexec "$JAIL_NAME" pkg update
sudo /usr/local/sbin/jexec "$JAIL_NAME" pkg install -y wine-proton winetricks zenity

Step 10: Run winetricks to Install Additional Components
sh

sudo /usr/local/sbin/jexec "$JAIL_NAME" WINE=/usr/local/wine-proton/bin/wine winetricks cmd dxvk vkd3d nvapi vcrun2022

Step 11: Download and Install Steam Setup
sh

sudo fetch -o /tmp/SteamSetup.exe https://cdn.akamai.steamstatic.com/client/installer/SteamSetup.exe
sudo /usr/local/sbin/jexec "$JAIL_NAME" WINEPREFIX=~/.steam-games /usr/local/wine-proton/bin/wine /tmp/SteamSetup.exe

Step 12: Apply Registry Patches
sh

sudo /usr/local/sbin/jexec "$JAIL_NAME" WINEPREFIX=~/.steam-games /usr/local/wine-proton/bin/wine reg.exe ADD "HKEY_CURRENT_USER\\Software\\Wine\\DllOverrides" /v "gameoverlayrenderer" /t "REG_SZ" /d "" /f
sudo /usr/local/sbin/jexec "$JAIL_NAME" WINEPREFIX=~/.steam-games /usr/local/wine-proton/bin/wine reg.exe ADD "HKEY_CURRENT_USER\\Software\\Wine\\DllOverrides" /v "gameoverlayrenderer64" /t "REG_SZ" /d "" /f

Step 13: Verify Installation
sh

if sudo /usr/local/sbin/jexec "$JAIL_NAME" pkg info | grep -q "wine-proton"; then
    echo "wine-proton is installed successfully in the jail '$JAIL_NAME'."
else
    echo "Failed to install wine-proton in the jail '$JAIL_NAME'."
    exit 1
fi

Step 14: Initialize Wine
sh

sudo /usr/local/sbin/jexec "$JAIL_NAME" sh -c "env WINEPREFIX=~/.steam-games WINE=/usr/local/wine-proton/bin/wine WINE64=/usr/local/wine-proton/bin/wine WINEARCH=win64 wineboot --init"

Commands to Check Status of the Pool
Check ZFS List
sh

zfs list

Check Zpool Status
sh

zpool status

Troubleshooting

If you encounter issues, ensure that:

    The ZFS pool is online and healthy.
    The ezjail configuration is set up correctly.
    The /etc/devfs.rules file is configured to allow jail access to /dev/zfs.

Refer to the log messages in the script for additional information and debugging steps.
Run the Jailer: This will allow you to install items the other scipt may have missed, launch Steam, launch games, and link the launcher to $HOME/bin

sh

    ./steam_jailer

By following these steps, you can set up a Steam game launcher on FreeBSD using a jail, ensuring that all necessary libraries and dependencies are available for the games, if anything is missing simply add it with wintricks in the script.
