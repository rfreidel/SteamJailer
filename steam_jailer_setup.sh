#!/bin/sh

JAIL_NAME="steam"

# Function to log messages
log_message() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Step 1: Install ezjail and enable it in rc.conf
log_message "Installing ezjail..."
sudo pkg install -y ezjail

log_message "Enabling ezjail in rc.conf..."
echo 'ezjail_enable="YES"' | sudo tee -a /etc/rc.conf

# Step 2: Install the base jail
log_message "Installing the base jail..."
if ! sudo ezjail-admin install; then
    log_message "Failed to install the base jail. Please check your FreeBSD version and network connection."
    exit 1
fi

# Step 3: Start the ezjail service
log_message "Starting the ezjail service..."
if ! sudo service ezjail start; then
    log_message "Failed to start the ezjail service."
    exit 1
fi

# Step 4: Verify if the jail exists
log_message "Checking if the jail '$JAIL_NAME' exists..."
if ! sudo ezjail-admin list | grep -q "$JAIL_NAME"; then
    log_message "Jail '$JAIL_NAME' not found. Creating the jail..."
    if ! sudo ezjail-admin create "$JAIL_NAME" 'lo1|127.0.1.1'; then
        log_message "Failed to create the jail '$JAIL_NAME'."
        exit 1
    fi
    if ! sudo ezjail-admin start "$JAIL_NAME"; then
        log_message "Failed to start the jail '$JAIL_NAME'."
        exit 1
    fi
else
    log_message "Jail '$JAIL_NAME' already exists."
fi

# Step 5: Start the jail if it is not running
log_message "Ensuring the jail '$JAIL_NAME' is running..."
if ! sudo jls | grep -q "$JAIL_NAME"; then
    if ! sudo ezjail-admin start "$JAIL_NAME"; then
        log_message "Failed to start the jail '$JAIL_NAME'."
        exit 1
    fi
    log_message "Jail '$JAIL_NAME' started."
else
    log_message "Jail '$JAIL_NAME' is already running."
fi

# Step 6: Install wine-proton in the jail
log_message "Installing wine-proton in the jail '$JAIL_NAME'..."
sudo jexec "$JAIL_NAME" pkg update
sudo jexec "$JAIL_NAME" pkg install -y wine wine64 wine-proton winetricks zenity

log_message "Verifying the installation of wine-proton..."
if sudo jexec "$JAIL_NAME" pkg info | grep -q "wine-proton"; then
    log_message "wine-proton is installed successfully in the jail '$JAIL_NAME'."
else
    log_message "Failed to install wine-proton in the jail '$JAIL_NAME'."
    exit 1
fi

log_message "Setting up environment variables and initializing Wine..."
sudo jexec "$JAIL_NAME" sh -c "setenv WINE /usr/local/bin/wine; setenv WINE64 /usr/local/bin/wine64; setenv WINEARCH win64; wineboot --init"

log_message "Installation and setup completed successfully."