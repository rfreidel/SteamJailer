#!/bin/sh

# Exit on error
set -e

# FreeBSD POSIX paths
FREEBSD_VERSION="/bin/freebsd-version"
RESOLV_CONF="/etc/resolv.conf"
IOCAGE="/usr/local/bin/iocage"
PKG="/usr/sbin/pkg"
PW="/usr/sbin/pw"
FETCH="/usr/bin/fetch"
CHMOD="/bin/chmod"
CHOWN="/usr/sbin/chown"

# Get FreeBSD release version properly
RELEASE=$($FREEBSD_VERSION | cut -d'-' -f1)

# Check if jail exists
check_jail() {
    $IOCAGE list | grep -q "^steamjail" || return 1
    return 0
}

# Configure networking in jail
setup_networking() {
    # Copy host's DNS configuration to jail
    cp $RESOLV_CONF /zroot/iocage/jails/steamjail/root/etc/resolv.conf
    
    # Configure pkg in jail
    mkdir -p /zroot/iocage/jails/steamjail/root/usr/local/etc/pkg/repos
    cat > /zroot/iocage/jails/steamjail/root/usr/local/etc/pkg/repos/FreeBSD.conf << EOF
FreeBSD: {
    url: "pkg+http://pkg.FreeBSD.org/\${ABI}/quarterly",
    mirror_type: "srv",
    signature_type: "fingerprints",
    fingerprints: "/usr/share/keys/pkg",
    enabled: yes
}
EOF
}

# Create jail if it doesn't exist
if ! check_jail; then
    echo "Creating jail: steamjail"
    $IOCAGE create \
        -r "${RELEASE}" \
        -n steamjail \
        allow_raw_sockets=1 \
        boot=on \
        allow_mlock=1 \
        sysvmsg=new \
        sysvsem=new \
        sysvshm=new \
        ip4_addr="lo0|127.0.1.1/32"
    
    # Mount devfs
    $IOCAGE fstab -a steamjail devfs /dev devfs rw 0 0 || true
fi

# Start jail if not running
if ! $IOCAGE list | grep -q "^steamjail.*up"; then
    echo "Starting jail: steamjail"
    $IOCAGE start steamjail
fi

# Setup networking
echo "Configuring networking..."
setup_networking

# Update package repository in jail
echo "Updating package repository..."
$IOCAGE exec steamjail $PKG update -f || true

# Install required packages
echo "Installing required packages..."
$IOCAGE exec steamjail $PKG install -y \
    wine-proton \
    winetricks \
    ca_root_nss || true

# Create steam user if doesn't exist
if ! $IOCAGE exec steamjail $PW usershow steam >/dev/null 2>&1; then
    echo "Creating steam user..."
    $IOCAGE exec steamjail $PW useradd steam -m -G wheel || true
fi

# Initialize Wine environment
if [ -x "/zroot/iocage/jails/steamjail/root/usr/local/wine-proton/bin/wine" ]; then
    echo "Initializing Wine..."
    $IOCAGE exec -U steam steamjail /usr/bin/env \
        HOME=/home/steam \
        WINEPREFIX=/home/steam/.wine \
        /usr/local/wine-proton/bin/wine wineboot --init || true
fi

# Download and install Steam
if [ -d "/zroot/iocage/jails/steamjail/root/home/steam" ]; then
    echo "Downloading Steam..."
    $IOCAGE exec -U steam steamjail $FETCH \
        -o /home/steam/SteamSetup.exe \
        https://cdn.cloudflare.steamstatic.com/client/installer/SteamSetup.exe || true

    if [ -f "/zroot/iocage/jails/steamjail/root/home/steam/SteamSetup.exe" ]; then
        echo "Installing Steam..."
        $CHMOD 755 /zroot/iocage/jails/steamjail/root/home/steam/SteamSetup.exe
        $CHOWN steam:wheel /zroot/iocage/jails/steamjail/root/home/steam/SteamSetup.exe
        
        $IOCAGE exec -U steam steamjail /usr/bin/env \
            HOME=/home/steam \
            WINEPREFIX=/home/steam/.wine \
            /usr/local/wine-proton/bin/wine /home/steam/SteamSetup.exe /S || true

        # Cleanup
        rm -f /zroot/iocage/jails/steamjail/root/home/steam/SteamSetup.exe
    fi
fi

echo "\nInstallation completed!"
echo "\nTo launch Steam, use:"
echo "$IOCAGE exec -U steam steamjail /usr/bin/env \\"
echo "    HOME=/home/steam \\"
echo "    WINEPREFIX=/home/steam/.wine \\"
echo "    /usr/local/wine-proton/bin/wine ~/.wine/drive_c/Program\\ Files\\ \\(x86\\)/Steam/Steam.exe"