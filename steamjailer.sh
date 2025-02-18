#!/bin/sh

# Current Date and Time (UTC - YYYY-MM-DD HH:MM:SS formatted): 2025-02-18 21:30:59
# Current User's Login: rfreidel
# Do use FreeBSD-commands Posix-syntax
# Don't use iocage

USAGE="Usage: ./steamjailer.sh [-c create] [-d destroy] [-s start] [-S stop] [-h help] [-i install] [-u update] JAILNAME"
JAILBASE="/jail"
STEAM_JAIL="steam0"
WINE="/usr/local/wine-proton/bin/wine"

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

# Function to handle create command
do_create() {
    local jailname="$1"
    echo "Creating jail: ${jailname}"
    echo "You will be prompted to set the jail's root password (not host password)"
    
    # Create jail directory
    /bin/mkdir -p "${JAILBASE}/${jailname}"
    
    # Install base system
    /usr/sbin/bsdinstall jail "${JAILBASE}/${jailname}"
    
    if [ $? -eq 0 ]; then
        echo "Jail base system installed successfully"
        
        # Basic jail configuration
        /bin/cat > "${JAILBASE}/${jailname}/etc/rc.conf" << EOF
sendmail_enable="NONE"
syslogd_flags="-ss"
clear_tmp_enable="YES"
dbus_enable="YES"
EOF

        # Create jail configuration
        /bin/cat >> /etc/jail.conf << EOF

${jailname} {
    path = "${JAILBASE}/${jailname}";
    mount.devfs;
    allow.raw_sockets;
    allow.socket_af;
    allow.sysvipc;
    exec.clean;
    host.hostname = "${jailname}.jail";
    exec.start = "/bin/sh /etc/rc";
    exec.stop = "/bin/sh /etc/rc.shutdown";
    persist;
}
EOF
        echo "Jail ${jailname} created and configured successfully"
    else
        echo "Error: Jail creation failed"
        /bin/rm -rf "${JAILBASE}/${jailname}"
        exit 1
    fi
}

# Function to handle destroy command
do_destroy() {
    local jailname="$1"
    echo "Destroying jail: ${jailname}"
    
    # Stop jail if running
    /usr/sbin/service jail stop "${jailname}"
    
    # Remove jail configuration
    /usr/bin/sed -i '' "/^${jailname} {/,/^}/d" /etc/jail.conf

    # Unmount any mounted filesystems
    /sbin/umount -f "${JAILBASE}/${jailname}/dev" 2>/dev/null || true
    
    # Clear system immutable flags
    /usr/bin/chflags -R noschg "${JAILBASE}/${jailname}"
    
    # Clear user immutable flags
    /usr/bin/chflags -R nouunlink "${JAILBASE}/${jailname}"
    
    # Remove the jail directory
    /bin/rm -rf "${JAILBASE}/${jailname}"
    
    if [ -d "${JAILBASE}/${jailname}" ]; then
        echo "Warning: Could not completely remove jail directory. Manual cleanup may be needed."
    else
        echo "Jail ${jailname} destroyed successfully"
    fi
}

# Function to handle start command
do_start() {
    local jailname="$1"
    echo "Starting jail: ${jailname}"
    /usr/sbin/service jail start "${jailname}"
}

# Function to handle stop command
do_stop() {
    local jailname="$1"
    echo "Stopping jail: ${jailname}"
    /usr/sbin/service jail stop "${jailname}"
}

# Function to handle install command
do_install() {
    local jailname="$1"
    echo "Installing Steam environment in jail: ${jailname}"
    
    # Update package repository
    /usr/sbin/jexec "${jailname}" /usr/sbin/pkg update
    
    # Install required packages
    /usr/sbin/jexec "${jailname}" /usr/sbin/pkg install -y \
        wine-proton \
        winetricks \
        vulkan-tools \
        mesa-dri \
        mesa-libs \
        libglvnd \
        dbus \
        ca_root_nss
    
    # Set wine-proton environment
    /bin/cat >> "${JAILBASE}/${jailname}/etc/profile" << EOF
export WINE="${WINE}"
export WINEPREFIX=/home/steam/.wine
export DISPLAY=:0
EOF

    # Create steam user
    /usr/sbin/jexec "${jailname}" /usr/sbin/pw user add steam -m -G wheel

    # Install Steam using wine-proton
    echo "Installing Steam..."
    /usr/sbin/jexec -U steam "${jailname}" "${WINE}" steam
}

# Function to handle update command
do_update() {
    local jailname="$1"
    echo "Updating jail: ${jailname}"
    
    # Update FreeBSD base
    /usr/sbin/freebsd-update -b "${JAILBASE}/${jailname}" fetch install
    
    # Update packages
    /usr/sbin/jexec "${jailname}" /usr/sbin/pkg upgrade -y
}

# Main script execution
check_root

CMD=""
JAILNAME=""

# Process command line arguments
while getopts "c:d:s:S:i:u:h" opt; do
    case ${opt} in
        c) CMD="create"; JAILNAME="$OPTARG" ;;
        d) CMD="destroy"; JAILNAME="$OPTARG" ;;
        s) CMD="start"; JAILNAME="$OPTARG" ;;
        S) CMD="stop"; JAILNAME="$OPTARG" ;;
        i) CMD="install"; JAILNAME="$OPTARG" ;;
        u) CMD="update"; JAILNAME="$OPTARG" ;;
        h|?) show_usage ;;
    esac
done

# If no command was specified, show usage
[ -z "${CMD}" ] && show_usage

# Execute the appropriate command
case ${CMD} in
    create)  do_create "${JAILNAME}" ;;
    destroy) do_destroy "${JAILNAME}" ;;
    start)   do_start "${JAILNAME}" ;;
    stop)    do_stop "${JAILNAME}" ;;
    install) do_install "${JAILNAME}" ;;
    update)  do_update "${JAILNAME}" ;;
    *) show_usage ;;
esac