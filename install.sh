#!/bin/sh
# FreeBSD Steam Jailer Installation Script

# Set iocage environment variables
export IOCAGE_COLOR=TRUE
export IOCAGE_FORCE=TRUE

# FreeBSD POSIX paths
UNAME="/usr/bin/uname"
DATE="/bin/date"
ID="/usr/bin/id"
IOCAGE="/usr/local/bin/iocage"
PKG="/usr/sbin/pkg"
SED="/usr/bin/sed"
PRINTF="/usr/bin/printf"
SLEEP="/bin/sleep"
GREP="/usr/bin/grep"
CAT="/bin/cat"
MKDIR="/bin/mkdir"
ECHO="/bin/echo"
IFCONFIG="/sbin/ifconfig"

# Display header information
"${PRINTF}" "Current Date and Time (UTC - YYYY-MM-DD HH:MM:SS formatted): %s\n" "$("${DATE}" -u "+%Y-%m-%d %H:%M:%S")"
"${PRINTF}" "Current User's Login: %s\n\n" "$("${ID}" -un)"

# Detect network interface (including wifi)
# First check for active wifi interfaces
WIFI_IF=$("${IFCONFIG}" -l | "${GREP}" -o 'wlan[0-9]\+' | head -1)
if [ -n "${WIFI_IF}" ] && "${IFCONFIG}" "${WIFI_IF}" | "${GREP}" -q "status: associated"; then
    DEFAULT_IF="${WIFI_IF}"
    "${PRINTF}" "Detected active WiFi interface: %s\n" "${DEFAULT_IF}"
else
    # If no wifi, get first active interface excluding loopback
    DEFAULT_IF=$("${IFCONFIG}" -l | "${SED}" -E 's/lo[0-9]+ //g' | cut -d' ' -f1)
    "${PRINTF}" "Using network interface: %s\n" "${DEFAULT_IF}"
fi

# Get IP configuration for detected interface
if [ -n "${DEFAULT_IF}" ]; then
    DEFAULT_IP=$("${IFCONFIG}" "${DEFAULT_IF}" inet | "${GREP}" inet | cut -d' ' -f2)
    DEFAULT_MASK=$("${IFCONFIG}" "${DEFAULT_IF}" inet | "${GREP}" inet | cut -d' ' -f4)
    "${PRINTF}" "Interface %s IP address: %s/%s\n" "${DEFAULT_IF}" "${DEFAULT_IP}" "${DEFAULT_MASK}"
else
    "${PRINTF}" "No active network interface found!\n"
    exit 1
fi

# Get FreeBSD version without -p1 suffix for iocage
FBSD_VERSION=$("${UNAME}" -r | "${SED}" -e 's/-[pP][0-9]$//')
"${PRINTF}" "Using FreeBSD version: %s\n" "${FBSD_VERSION}"

# Function to ensure jail is stopped
stop_jail() {
    "${PRINTF}" "Stopping jail if running...\n"
    "${IOCAGE}" stop steamjail >/dev/null 2>&1 || true
    "${SLEEP}" 2
}

# Function to destroy jail
destroy_jail() {
    "${PRINTF}" "Removing existing jail...\n"
    "${IOCAGE}" destroy -f steamjail >/dev/null 2>&1 || true
    "${SLEEP}" 2
}

# Clean up existing jail
if "${IOCAGE}" list | "${GREP}" -q "steamjail"; then
    stop_jail
    destroy_jail
fi

"${PRINTF}" "Creating new steamjail...\n"
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

"${SLEEP}" 2

"${PRINTF}" "Configuring jail network...\n"
"${IOCAGE}" set vnet=off steamjail || exit 1
"${IOCAGE}" set ip4_addr="${DEFAULT_IF}|${DEFAULT_IP}" steamjail || exit 1

"${PRINTF}" "Starting jail...\n"
"${IOCAGE}" start steamjail || {
    "${PRINTF}" "Failed to start jail. Error code: %d\n" "$?"
    exit 1
}
"${SLEEP}" 2

"${PRINTF}" "Verifying jail network configuration...\n"
"${IOCAGE}" exec steamjail "${IFCONFIG}" || {
    "${PRINTF}" "Failed to verify network configuration. Error code: %d\n" "$?"
    exit 1
}

"${PRINTF}" "Setting up DNS configuration...\n"
# Copy host's resolv.conf to ensure we have the correct DNS settings for WiFi
"${IOCAGE}" exec steamjail /bin/sh -c 'cp /etc/resolv.conf /etc/resolv.conf.bak 2>/dev/null || true && \
    cat /etc/resolv.conf.bak > /etc/resolv.conf 2>/dev/null || \
    { echo "nameserver 8.8.8.8" > /etc/resolv.conf && echo "nameserver 8.8.4.4" >> /etc/resolv.conf; }' || {
    "${PRINTF}" "Failed to configure DNS. Error code: %d\n" "$?"
    exit 1
}

"${PRINTF}" "Testing network connectivity...\n"
"${IOCAGE}" exec steamjail /usr/bin/host pkg.FreeBSD.org || {
    "${PRINTF}" "Network test failed. Error code: %d\n" "$?"
    "${PRINTF}" "DNS Configuration:\n"
    "${IOCAGE}" exec steamjail "${CAT}" /etc/resolv.conf
    "${PRINTF}" "Network Configuration:\n"
    "${IOCAGE}" exec steamjail "${IFCONFIG}"
    exit 1
}
"${PRINTF}" "Network connectivity verified\n"

# Continue with pkg bootstrap and remaining installation...