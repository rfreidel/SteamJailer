#!/bin/sh

# FreeBSD POSIX paths
UNAME="/usr/bin/uname"
IOCAGE="/usr/local/bin/iocage"
PKG="/usr/sbin/pkg"
SERVICE="/usr/sbin/service"
SED="/usr/bin/sed"
ECHO="/bin/echo"

# Get FreeBSD version without -p1 suffix for iocage
FBSD_VERSION=$("${UNAME}" -r | "${SED}" -e 's/-p[0-9]$//' -e 's/-P[0-9]$//')
"${ECHO}" "Using FreeBSD version: ${FBSD_VERSION}"

# Enable iocage service
"${SERVICE}" iocage onestart

# Create jail with proper networking
"${IOCAGE}" create \
    -n steamjail \
    -r "${FBSD_VERSION}" \
    --thickjail \
    allow_raw_sockets=1 \
    boot=on \
    allow_mlock=1 \
    sysvmsg=new \
    sysvsem=new \
    sysvshm=new || exit 1

# Update pkg repository to latest
"${PKG}" -j steamjail config \
    REPOSITORY_URL "pkg+https://pkg.FreeBSD.org/FreeBSD:14:amd64/latest" || exit 1

# Install required packages from packages.json
if [ -f "packages.json" ]; then
    for pkg in $(jq -r '.pkgs[]' packages.json); do
        "${PKG}" -j steamjail install -y "${pkg}" || exit 1
    done
fi

"${ECHO}" "Jail setup completed!"
"${ECHO}" "To access the jail:"
"${ECHO}" "${IOCAGE} console steamjail"