#!/bin/sh

JAIL_NAME="steamjailer"

# Function to log messages
log_message() {
    $(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Step 1: Install ezjail and enable it in rc.conf
log_message "Installing ezjail..."
sudo pkg install -y ezjail
log_message "ezjail has installed"
sudo ezjail-admin install
log_message "ezjail-admin has installed"
sudo ezjail-admin update -p -i
log_message "ezjail-admin update has completed"

log_message "Steam jail had been built"

log_message "Enabling ezjail in rc.conf..."
echo 'ezjail_enable="YES"' | sudo tee -a /etc/rc.conf

# Step 2: Create a new jail using ezjail-admin
log_message "Creating the jail '$JAIL_NAME'..."
if sudo ezjail-admin create "$JAIL_NAME" 'lo1|127.0.1.1'; then
    log_message "Jail '$JAIL_NAME' created successfully."
else
    log_message "Failed to create the jail '$JAIL_NAME'."
    exit 1
fi

# Step 3: Start the jail
log_message "Starting the jail '$JAIL_NAME'..."
if sudo ezjail-admin start "$JAIL_NAME"; then
    log_message "Jail '$JAIL_NAME' started successfully."
else
    log_message "Failed to start the jail '$JAIL_NAME'."
    exit 1
fi

# Step 4: Install packages in the jail
sh install_packages.sh "$JAIL_NAME"