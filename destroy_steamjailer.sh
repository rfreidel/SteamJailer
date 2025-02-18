#!/bin/sh

# Current Date and Time (UTC - YYYY-MM-DD HH:MM:SS formatted): 2025-02-18 17:41:44
# Current User's Login: rfreidel
# Do use FreeBSD-commands Posix-syntax
# Don't use iocage

USAGE="Usage: ./steamjailer_destroy.sh -d JAILNAME"
JAILBASE="/jail"

# Function to check if running as root
check_root() {
    if [ "$(/usr/bin/id -u)" -ne 0 ]; then
        echo "This script must be run as root." >&2
        exit 1
    fi
}

# Function to show usage
show_usage() {
    echo "${USAGE}"
    exit 1
}

# Function to destroy jail
destroy_jail() {
    local jailname="$1"
    
    # Stop jail if running
    /usr/sbin/service jail stop "${jailname}" >/dev/null 2>&1
    
    # Remove jail configuration
    /usr/bin/sed -i '' "/^${jailname} {/,/^}/d" /etc/jail.conf >/dev/null 2>&1

    # Unmount any mounted filesystems
    /sbin/umount -f "${JAILBASE}/${jailname}/dev" >/dev/null 2>&1
    
    # Clear schg flags recursively
    /usr/bin/chflags -R noschg "${JAILBASE}/${jailname}" >/dev/null 2>&1
    
    # Clear remaining flags
    /usr/bin/chflags -R nouunlink "${JAILBASE}/${jailname}" >/dev/null 2>&1
    
    # Remove the jail directory
    /bin/rm -rf "${JAILBASE}/${jailname}" >/dev/null 2>&1
}

# Main script execution
check_root

JAILNAME=""

# Process command line arguments
while getopts "d:h" opt; do
    case ${opt} in
        d) JAILNAME="$OPTARG" ;;
        h|?) show_usage ;;
    esac
done

# If no jail name was specified, show usage
[ -z "${JAILNAME}" ] && show_usage

# Execute destroy command
destroy_jail "${JAILNAME}"