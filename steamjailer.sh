#!/bin/sh

# Current Date and Time (UTC - YYYY-MM-DD HH:MM:SS formatted): 2025-02-19 05:28:06
# Current User's Login: rfreidel
# Do use FreeBSD-commands Posix-syntax
# Don't use iocage

USAGE="Usage: ./steamjailer.sh [-c create] [-d destroy] [-s start] [-S stop] [-h help] [-i install] [-u update] [-l list] [-w winetricks] JAILNAME"
JAILBASE="/jail"
STEAM_JAIL="steam0"
WINE="/usr/local/wine-proton/bin/wine"

check_root() {
    if [ "$(/usr/bin/id -u)" -ne 0 ]; then
        echo "This script must be run as root." >&2
        exit 1
    fi
}

show_usage() {
    echo "${USAGE}"
    exit 1
}

do_list() {
    echo "Installed jails:"
    if [ -d "${JAILBASE}" ]; then
        for jail in ${JAILBASE}/*; do
            if [ -d "${jail}" ]; then
                jailname=$(basename "${jail}")
                if /usr/sbin/jls -j "${jailname}" >/dev/null 2>&1; then
                    echo "${jailname} (Running)"
                else
                    echo "${jailname} (Stopped)"
                fi
            fi
        done
    else
        echo "No jails found in ${JAILBASE}"
    fi
}

do_create() {
    local jailname="$1"
    echo "Creating jail: ${jailname}"
    echo "You will be prompted to set the jail's root password (not host password)"
    
    /bin/mkdir -p "${JAILBASE}/${jailname}"
    /usr/sbin/bsdinstall jail "${JAILBASE}/${jailname}"
    
    if [ $? -eq 0 ]; then
        echo "Jail base system installed successfully"
        
        /bin/cat > "${JAILBASE}/${jailname}/etc/rc.conf" << EOF
sendmail_enable="NONE"
syslogd_flags="-ss"
clear_tmp_enable="YES"
dbus_enable="YES"
EOF

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

do_destroy() {
    local jailname="$1"
    echo "Destroying jail: ${jailname}"
    
    /usr/sbin/service jail stop "${jailname}"
    /usr/bin/sed -i '' "/^${jailname} {/,/^}/d" /etc/jail.conf
    /sbin/umount -f "${JAILBASE}/${jailname}/dev" 2>/dev/null || true
    
    echo "Removing system flags..."
    /bin/chflags -R 0 "${JAILBASE}/${jailname}" 2>/dev/null || true
    
    echo "Setting all files writable..."
    /bin/chmod -R u+w "${JAILBASE}/${jailname}" 2>/dev/null || true
    
    echo "Attempting cleanup with find..."
    /usr/bin/find "${JAILBASE}/${jailname}" -type f -exec /bin/chmod 644 {} + 2>/dev/null || true
    /usr/bin/find "${JAILBASE}/${jailname}" -type d -exec /bin/chmod 755 {} + 2>/dev/null || true
    
    echo "Removing jail directory..."
    /bin/rm -rf "${JAILBASE}/${jailname}"
    
    if [ -d "${JAILBASE}/${jailname}" ]; then
        echo "Warning: Could not completely remove jail directory."
        echo "Try manual cleanup with:"
        echo "chflags -R 0 ${JAILBASE}/${jailname}"
        echo "chmod -R u+w ${JAILBASE}/${jailname}"
        echo "rm -rf ${JAILBASE}/${jailname}"
    else
        echo "Jail ${jailname} destroyed successfully"
    fi
}

do_start() {
    local jailname="$1"
    echo "Starting jail: ${jailname}"
    /usr/sbin/service jail start "${jailname}"
}

do_stop() {
    local jailname="$1"
    echo "Stopping jail: ${jailname}"
    /usr/sbin/service jail stop "${jailname}"
}

do_winetricks() {
    local jailname="$1"
    echo "Installing Winetricks modules in jail: ${jailname}"
    
    if ! /usr/sbin/jls -j "${jailname}" >/dev/null 2>&1; then
        echo "Error: Jail ${jailname} is not running"
        echo "Please start the jail first with: $0 -s ${jailname}"
        exit 1
    fi

    echo "Updating Winetricks..."
    /usr/sbin/jexec -U steam "${jailname}" "${WINE}" winetricks --self-update

    echo "Installing Winetricks modules..."
    /usr/sbin/jexec -U steam "${jailname}" "${WINE}" winetricks dxvk_nvapi d3dcompiler_47 cmd dotnet7 faudio
}

do_install() {
    local jailname="$1"
    echo "Installing Steam environment in jail: ${jailname}"
    
    /usr/sbin/jexec "${jailname}" /usr/sbin/pkg update
    
    /usr/sbin/jexec "${jailname}" /usr/sbin/pkg install -y \
        wine-proton \
        winetricks \
        vulkan-tools \
        mesa-dri \
        mesa-libs \
        libglvnd \
        dbus \
        ca_root_nss
    
    /bin/cat >> "${JAILBASE}/${jailname}/etc/profile" << EOF
export WINE="${WINE}"
export WINEPREFIX=/home/steam/.wine
export DISPLAY=:0
EOF

    /usr/sbin/jexec "${jailname}" /usr/sbin/pw user add steam -m -G wheel

    echo "Installing Steam..."
    /usr/sbin/jexec -U steam "${jailname}" "${WINE}" steam
}

do_update() {
    local jailname="$1"
    echo "Updating jail: ${jailname}"
    
    /usr/sbin/freebsd-update -b "${JAILBASE}/${jailname}" fetch install
    /usr/sbin/jexec "${jailname}" /usr/sbin/pkg upgrade -y
}

check_root

CMD=""
JAILNAME=""

while getopts "c:d:s:S:i:u:w:lh" opt; do
    case ${opt} in
        c) CMD="create"; JAILNAME="$OPTARG" ;;
        d) CMD="destroy"; JAILNAME="$OPTARG" ;;
        s) CMD="start"; JAILNAME="$OPTARG" ;;
        S) CMD="stop"; JAILNAME="$OPTARG" ;;
        i) CMD="install"; JAILNAME="$OPTARG" ;;
        u) CMD="update"; JAILNAME="$OPTARG" ;;
        w) CMD="winetricks"; JAILNAME="$OPTARG" ;;
        l) CMD="list" ;;
        h|?) show_usage ;;
    esac
done

[ -z "${CMD}" ] && show_usage

case ${CMD} in
    create)     do_create "${JAILNAME}" ;;
    destroy)    do_destroy "${JAILNAME}" ;;
    start)      do_start "${JAILNAME}" ;;
    stop)       do_stop "${JAILNAME}" ;;
    install)    do_install "${JAILNAME}" ;;
    update)     do_update "${JAILNAME}" ;;
    list)       do_list ;;
    winetricks) do_winetricks "${JAILNAME}" ;;
    *)          show_usage ;;
esac