#!/bin/sh

JAIL_NAME="$1"

# Function to log messages
log_message() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

log_message "Installing packages in the jail '$JAIL_NAME'..."
sudo /usr/local/sbin/jexec "$JAIL_NAME" pkg update || { log_message "Failed to update pkg repository." ; exit 1; }
sudo /usr/local/sbin/jexec "$JAIL_NAME" pkg install -y wine-proton winetricks zenity || { log_message "Failed to install required packages." ; exit 1; }

log_message "Running winetricks to install additional components..."
sudo /usr/local/sbin/jexec "$JAIL_NAME" WINE=/usr/local/wine-proton/bin/wine winetricks cmd dxvk vkd3d nvapi vcrun2022 || { log_message "Failed to run winetricks." ; exit 1; }

log_message "Downloading Steam setup..."
sudo fetch -o /tmp/SteamSetup.exe https://cdn.fastly.steamstatic.com/client/installer/SteamSetup.exe || { log_message "Failed to download Steam setup." ; exit 1; }

log_message "Installing Steam setup..."
/usr/local/sbin/jexec "$JAIL_NAME" WINEPREFIX=~/.steam-games /usr/local/wine-proton/bin/wine /tmp/SteamSetup.exe || { log_message "Failed to install Steam setup." ; exit 1; }

log_message "Applying registry patches..."
/usr/local/sbin/jexec "$JAIL_NAME" WINEPREFIX=~/.steam-games /usr/local/wine-proton/bin/wine reg.exe ADD "HKEY_CURRENT_USER\Software\Wine\DllOverrides" /v "gameoverlayrenderer" /t "REG_SZ" /d "" /f || { log_message "Failed to apply registry patch for gameoverlayrenderer." ; exit 1; }
/usr/local/sbin/jexec "$JAIL_NAME" WINEPREFIX=~/.steam-games /usr/local/wine-proton/bin/wine reg.exe ADD "HKEY_CURRENT_USER\Software\Wine\DllOverrides" /v "gameoverlayrenderer64" /t "REG_SZ" /d "" /f || { log_message "Failed to apply registry patch for gameoverlayrenderer64." ; exit 1; }

log_message "Verifying the installation of wine-proton winetricks steam..."
if sudo /usr/local/sbin/jexec "$JAIL_NAME" pkg info | grep -q "wine-proton"; then
    log_message "wine-proton is installed successfully in the jail '$JAIL_NAME'."
else
    log_message "Failed to install wine-proton in the jail '$JAIL_NAME'."
    exit 1
fi

log_message "Setting up environment variables and initializing Wine..."
sudo /usr/local/sbin/jexec "$JAIL_NAME" sh -c "setenv WINEPATH ~/.steam-games WINE=/usr/local/wine-proton/bin/wine; setenv WINE64 /usr/local/wine-proton/bin/wine; setenv WINEARCH win64; wineboot --init" || { log_message "Failed to set up environment variables and initialize Wine." ; exit 1; }

log_message "Installation and setup completed successfully."
