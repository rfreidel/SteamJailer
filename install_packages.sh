#!/bin/sh

JAIL_NAME="$1"

# Function to log messages
log_message() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

log_message "Installing packages in the jail '$JAIL_NAME'..."
sudo jexec "$JAIL_NAME" pkg update
sudo jexec "$JAIL_NAME" pkg install -y wine wine64 wine-proton winetricks zenity
sudo jexec "$JAIL_NAME" winetricks dxvk dxvk_nvapi steam

log_message "Verifying the installation of wine-proton winetricks steam..."
if sudo jexec "$JAIL_NAME" pkg info | grep -q "wine-proton"; then
    log_message "wine-proton is installed successfully in the jail '$JAIL_NAME'."
else
    log_message "Failed to install wine-proton in the jail '$JAIL_NAME'."
    exit 1
fi

log_message "Setting up environment variables and initializing Wine..."
sudo jexec "$JAIL_NAME" sh -c "setenv WINE /usr/local/wine-proton/bin/wine; setenv WINE64 /usr/local/wine-proton/bin/wine; setenv WINEARCH win64; wineboot --init"

log_message "Installation and setup completed successfully."
