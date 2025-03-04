## This file requires adjustment to your network interface

#!/bin/sh

# jail_setup.sh - FreeBSD jail setup script
# This script must be run as root

set -e

JAIL_NAME="$1"
JAIL_PATH="/usr/local/jail/${JAIL_NAME}"
JAIL_IP="192.168.0.10"

# Check if running as root
if [ "$(id -u)" != "0" ]; then
    echo "This script must be run as root" 1>&2
    exit 1
fi

# Create ZFS dataset for jail
zfs create zroot/jails/${JAIL_NAME}

# Create jail directory structure
mkdir -p "${JAIL_PATH}"

# Install base system
fetch -o /tmp/base.txz http://ftp.freebsd.org/pub/FreeBSD/releases/$(uname -m)/$(uname -r | cut -d- -f1-2)/base.txz
tar -xf /tmp/base.txz -C "${JAIL_PATH}"

# Configure jail
cat > /etc/jail.conf <<EOF
${JAIL_NAME} {
    path = "${JAIL_PATH}";
    host.hostname = "${JAIL_NAME}";
    ip4.addr = "${JAIL_IP}";
    interface = "em0";
    exec.start = "/bin/sh /etc/rc";
    exec.stop = "/bin/sh /etc/rc.shutdown";
    exec.clean;
    mount.devfs;
    allow.raw_sockets;
    allow.socket_af;
    vnet;
    vnet.interface = "epair0b";
}
EOF

# Create VNET interface
kldload if_epair || true
ifconfig epair0 create
ifconfig epair0a inet ${JAIL_IP} netmask 255.255.255.0 up

# Set up NAT
sysrc gateway_enable="YES"
sysrc pf_enable="YES"
sysrc pflog_enable="YES"

cat > /etc/pf.conf <<EOF
nat on em0 from ${JAIL_IP}/24 to any -> (em0)
pass all
EOF

service pf restart

echo "Jail setup completed successfully"
