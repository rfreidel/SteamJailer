#!/bin/sh

JAIL_NAME="$1"

# Function to log messages
log_message() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

log_message "Installing packages in the jail '$JAIL_NAME'..."
sudo /usr/local/sbin/jexec "$JAIL_NAME" pkg update
sudo /usr/local/sbin/jexec "$JAIL_NAME" pkg install -y wine-proton winetricks zenity
sudo /usr/local/sbin/jexec "$JAIL_NAME" WINE=/usr/local/wine-proton/bin/wine winetricks cmd dxvk vkd3d nvapi vcrun2022
sudo fetch -o "$JAIL_NAME" /tmp/SteamSetup.exe https://cdn.fastly.steamstatic.com/client/installer/SteamSetup.exe
/usr/local/sbin/jexec "$JAIL_NAME" WINEPREFIX=~/.steam-games /usr/local/wine-proton/bin/wine /tmp/SteamSetup.exe

# Patches to make games open in steam (Stolen from Suyimazu/Mizutamari)
/usr/local/sbin/jexec "$JAIL_NAME" WINEPREFIX=~/.steam-games WINE /usr/local/wine-proton/bin/wine $WINEBIN reg.exe ADD "HKEY_CURRENT_USER\Software\Wine\DllOverrides" /v "gameoverlayrenderer" /t "REG_SZ" /d "" /f
/usr/local/sbin/jexec "$JAIL_NAME" WINEPREFIX=~/.steam-games WINE /usr/local/wine-proton/bin/wine $WINEBIN reg.exe ADD "HKEY_CURRENT_USER\Software\Wine\DllOverrides" /v "gameoverlayrenderer64" /t "REG_SZ" /d "" /f

log_message "Verifying the installation of wine-proton winetricks steam..."
if sudo /usr/local/sbin/jexec "$JAIL_NAME" pkg info | grep -q "wine-proton"; then
    log_message "wine-proton is installed successfully in the jail '$JAIL_NAME'."
else
    log_message "Failed to install wine-proton in the jail '$JAIL_NAME'."
    exit 1
fi

log_message "Setting up environment variables and initializing Wine..."
sudo /usr/local/sbin/jexec "$JAIL_NAME" sh -c "setenv WINEPATH ~.steam-games WINE /usr/local/wine-proton/bin/wine; setenv WINE64 /usr/local/wine-proton/bin/wine; setenv WINEARCH win64; wineboot --init"

log_message "Installation and setup completed successfully."
