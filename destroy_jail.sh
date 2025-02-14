#!/bin/sh

# Jail name
JAIL_NAME="steamjailer"

# Ensure the jail is stopped
echo "Stopping jail $JAIL_NAME..."
iocage stop $JAIL_NAME

# Unmount file systems
echo "Unmounting filesystems..."
iocage fstab -r $JAIL_NAME

# Destroy the jail
echo "Destroying jail $JAIL_NAME..."
iocage destroy -f $JAIL_NAME

echo "Jail $JAIL_NAME has been destroyed."