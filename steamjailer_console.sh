#!/bin/sh

# Current Date and Time (UTC - YYYY-MM-DD HH:MM:SS formatted): 2024-02-19 07:15:00
# Current User's Login: rfreidel
# Do use FreeBSD-commands Posix-syntax
# Don't use iocage

# Base configuration
JAILBASE="/jail"
VERBOSE=1
LOG_FILE="/var/log/steamjailer/steamjailer.log"
LOG_VERBOSE="/var/log/steamjailer/verbose.log"

# Logging functions
log_message() {
    local level="$1"
    local message="$2"
    local timestamp=$(/bin/date -u "+%Y-%m-%d %H:%M:%S UTC")
    local user=$(/usr/bin/whoami)
    local pid=$$
    echo "${timestamp} [${level}] ${user}[${pid}]: ${message}" | tee -a "$LOG_FILE"
    
    if [ "$VERBOSE" = "1" ]; then
        local cmd_info=$(/usr/bin/id -un)@$(/bin/hostname)
        local debug_info="PWD=$PWD EUID=$(/usr/bin/id -u)"
        echo "${timestamp} [${level}] ${cmd_info} ${debug_info}: ${message}" >> "$LOG_VERBOSE"
    fi
}

log_verbose() {
    if [ "$VERBOSE" = "1" ]; then
        log_message "VERBOSE" "$1"
    fi
}

log_info() {
    log_message "INFO" "$1"
}

log_warning() {
    log_message "WARNING" "$1"
}

log_error() {
    log_message "ERROR" "$1"
}

# Check if jail exists
check_jail_exists() {
    local jailname="$1"
    if [ -d "${JAILBASE}/${jailname}" ]; then
        return 0
    else
        return 1
    fi
}

# Check if jail is running
check_jail_running() {
    local jailname="$1"
    if /usr/sbin/jls -j "${jailname}" >/dev/null 2>&1; then
        return 0
    else
        return 1
    fi
}

# Destroy jail function
destroy_jail() {
    local jailname="$1"
    
    if ! check_jail_exists "$jailname"; then
        log_error "Jail does not exist: $jailname"
        return 1
    fi

    if check_jail_running "$jailname"; then
        log_info "Stopping jail before destruction: $jailname"
        /usr/sbin/service jail stop "$jailname"
    fi

    log_info "Removing jail directory: ${JAILBASE}/${jailname}"
    if ! rm -rf "${JAILBASE:?}/${jailname}"; then
        log_error "Failed to remove jail directory: ${JAILBASE}/${jailname}"
        return 1
    fi

    # Remove jail configuration if it exists
    if [ -f "/etc/jail.conf.d/${jailname}.conf" ]; then
        log_info "Removing jail configuration: /etc/jail.conf.d/${jailname}.conf"
        rm -f "/etc/jail.conf.d/${jailname}.conf"
    fi

    return 0
}

# List jails
list_jails() {
    log_verbose "Scanning jail directory: ${JAILBASE}"
    echo "Available jails:"
    echo "----------------"
    local found_jails=0
    
    if [ -d "${JAILBASE}" ]; then
        for jail in ${JAILBASE}/*; do
            if [ -d "${jail}" ]; then
                jailname=$(basename "${jail}")
                if check_jail_running "$jailname"; then
                    status="Running"
                    jail_info=$(/usr/sbin/jls -j "${jailname}" 2>/dev/null)
                    log_verbose "Jail ${jailname} status: ${status}"
                    log_verbose "Jail info: ${jail_info}"
                else
                    status="Stopped"
                    log_verbose "Jail ${jailname} status: ${status}"
                fi
                printf "%-20s %s\n" "$jailname" "$status"
                found_jails=1
            fi
        done
    fi
    
    if [ $found_jails -eq 0 ]; then
        log_warning "No jails found in: ${JAILBASE}"
        echo "No jails found."
    fi
    echo "----------------"
}

# Show help
show_help() {
    echo "Steam Jail Manager - Console Version"
    echo "Usage: $0 [option] [jailname]"
    echo ""
    echo "Options:"
    echo "  -c <jailname>    Create new jail"
    echo "  -d <jailname>    Destroy jail"
    echo "  -s <jailname>    Start jail"
    echo "  -S <jailname>    Stop jail"
    echo "  -i <jailname>    Install Steam environment"
    echo "  -w <jailname>    Install Winetricks modules"
    echo "  -u <jailname>    Update jail"
    echo "  -l               List jails"
    echo "  -v               Toggle verbose mode"
    echo "  -h               Show this help"
    echo ""
}

# Main execution
if [ "$(/usr/bin/id -u)" -ne 0 ]; then
    echo "This script must be run as root." >&2
    exit 1
fi

# Process command line arguments
if [ $# -eq 0 ]; then
    show_help
    exit 1
fi

while getopts "c:d:s:S:i:w:u:lvh" opt; do
    case $opt in
        d)
            log_info "Attempting to destroy jail: $OPTARG"
            if check_jail_exists "$OPTARG"; then
                printf "Are you sure you want to destroy %s? (y/N) " "$OPTARG"
                read -r confirm
                if [ "$confirm" = "y" ] || [ "$confirm" = "Y" ]; then
                    if destroy_jail "$OPTARG"; then
                        log_info "Successfully destroyed jail: $OPTARG"
                    else
                        log_error "Failed to destroy jail: $OPTARG"
                    fi
                else
                    log_info "Jail destruction cancelled: $OPTARG"
                fi
            else
                log_error "Jail does not exist: $OPTARG"
            fi
            ;;
        l)
            list_jails
            ;;
        # Other cases remain the same...
    esac
done

exit 0