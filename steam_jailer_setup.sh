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

# Step 4: Check if the jail already exists and create a new one if it does
log_message "Checking if the jail '$JAIL_NAME' exists..."
if sudo ezjail-admin list | grep -q "$JAIL_NAME"; then
    log_message "Jail '$JAIL_NAME' already exists. Creating a new jail..."
    NEW_JAIL_NAME="${JAIL_NAME}_$(date +%s)"
    JAIL_NAME="$NEW_JAIL_NAME"
fi

# Step 5: Create and start the jail
log_message "Creating the jail '$JAIL_NAME'..."
if ! sudo ezjail-admin create "$JAIL_NAME" 'lo1|127.0.1.1'; then
    log_message "Failed to create the jail '$JAIL_NAME'."
    exit 1
fi

log_message "Starting the jail '$JAIL_NAME'..."
if ! sudo ezjail-admin start "$JAIL_NAME"; then
    log_message "Failed to start the jail '$JAIL_NAME'."
    exit 1
fi

# Step 6: Install packages in the jail
sh install_packages.sh "$JAIL_NAME"