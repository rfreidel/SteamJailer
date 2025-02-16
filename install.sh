#!/bin/sh

# Define FreeBSD POSIX paths and commands
JLS="/usr/sbin/jls"
PKG="/usr/sbin/pkg"
IOCAGE="/usr/local/bin/iocage"
SYSRC="/usr/sbin/sysrc"
SERVICE="/usr/sbin/service"
PW="/usr/sbin/pw"
FREEBSD_VERSION="/bin/freebsd-version"
DATE="/bin/date"
ID="/usr/bin/id"
CHMOD="/bin/chmod"
CHOWN="/usr/sbin/chown"
FETCH="/usr/bin/fetch"

# Get proper FreeBSD release version
RELEASE=$($FREEBSD_VERSION -k | sed 's/-[Rp].*//') # Strip -RELEASE or -pX suffix
echo "Detected FreeBSD version: ${RELEASE}"

# Check if running as root
if [ "$($ID -u)" != "0" ]; then
    echo "This script must be run as root" >&2
    exit 1
fi

# Enable and start iocage
$SYSRC iocage_enable="YES"
$SERVICE iocage onestart

# Create base jail if it doesn't exist
if ! $IOCAGE list | grep -q "^steamjail"; then
    echo "Creating base jail..."
    $IOCAGE fetch -r "${RELEASE}"
    
    echo "Creating steamjail..."
    $IOCAGE create \
        -n steamjail \
        -r "${RELEASE}" \
        allow_raw_sockets=1 \
        boot=on \
        allow_mlock=1 \
        sysvmsg=new \
        sysvsem=new \
        sysvshm=new \
        ip4_addr="lo0|127.0.1.1/32"

    # Mount devfs in jail
    $IOCAGE fstab -a steamjail devfs /dev devfs rw 0 0
fi

# Start jail if not running
if ! $JLS | grep -q steamjail; then
    echo "Starting steamjail..."
    $IOCAGE start steamjail
fi

# Configure DNS in jail
cp /etc/resolv.conf /zroot/iocage/jails/steamjail/root/etc/resolv.conf

# Update packages in jail
echo "Updating package repository in jail..."
$IOCAGE exec steamjail $PKG update -f

# Install required packages
echo "Installing required packages in jail..."
$IOCAGE exec steamjail $PKG install -y \
    wine-proton \
    winetricks \
    ca_root_nss

# Create steam user in jail if doesn't exist
if ! $IOCAGE exec steamjail $PW usershow steam >/dev/null 2>&1; then
    echo "Creating steam user in jail..."
    $IOCAGE exec steamjail $PW useradd steam -m -G wheel
fi

# Initialize Wine
echo "Initializing Wine..."
$IOCAGE exec -U steam steamjail /usr/bin/env \
    HOME=/home/steam \
    WINEPREFIX=/home/steam/.wine \
    /usr/local/wine-proton/bin/wine wineboot --init

# Download Steam installer
echo "Downloading Steam installer..."
$IOCAGE exec -U steam steamjail $FETCH \
    -o /home/steam/SteamSetup.exe \
    https://cdn.cloudflare.steamstatic.com/client/installer/SteamSetup.exe

if [ -f "/zroot/iocage/jails/steamjail/root/home/steam/SteamSetup.exe" ]; then
    echo "Setting permissions for Steam installer..."
    $CHMOD 755 /zroot/iocage/jails/steamjail/root/home/steam/SteamSetup.exe
    $CHOWN steam:wheel /zroot/iocage/jails/steamjail/root/home/steam/SteamSetup.exe

    echo "Installing Steam..."
    $IOCAGE exec -U steam steamjail /usr/bin/env \
        HOME=/home/steam \
        WINEPREFIX=/home/steam/.wine \
        /usr/local/wine-proton/bin/wine /home/steam/SteamSetup.exe /S

    # Cleanup
    rm -f /zroot/iocage/jails/steamjail/root/home/steam/SteamSetup.exe
fi

echo "\nInstallation completed!"
echo "\nTo launch Steam, use:"
echo "$IOCAGE exec -U steam steamjail /usr/bin/env \\"
echo "    HOME=/home/steam \\"
echo "    WINEPREFIX=/home/steam/.wine \\"
echo "    /usr/local/wine-proton/bin/wine ~/.wine/drive_c/Program\\ Files\\ \\(x86\\)/Steam/Steam.exe"