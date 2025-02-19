#!/bin/sh

# Current Date and Time (UTC - YYYY-MM-DD HH:MM:SS formatted): 2025-02-19 06:41:01
# Current User's Login: rfreidel
# Do use FreeBSD-commands Posix-syntax
# Don't use iocage

LOG_DIR="/var/log/steamjailer"
LOG_FILE="${LOG_DIR}/steamjailer.log"

# Get current UTC time in correct format
get_utc_time() {
    /bin/date -u "+%Y-%m-%d %H:%M:%S UTC"
}

# Create log directory if it doesn't exist
init_logging() {
    if [ ! -d "${LOG_DIR}" ]; then
        /bin/mkdir -p "${LOG_DIR}"
        /bin/chmod 750 "${LOG_DIR}"
    fi
    if [ ! -f "${LOG_FILE}" ]; then
        /usr/bin/touch "${LOG_FILE}"
        /bin/chmod 640 "${LOG_FILE}"
    fi
}

# Function to log messages with correct timestamp
log_message() {
    local level="$1"
    local message="$2"
    local timestamp=$(get_utc_time)
    local user=$(/usr/bin/whoami)
    echo "${timestamp} [${level}] ${user}: ${message}" >> "${LOG_FILE}"
}

# Specific logging functions
log_info() {
    log_message "INFO" "$1"
}

log_error() {
    log_message "ERROR" "$1"
}

log_warning() {
    log_message "WARNING" "$1"
}

log_debug() {
    log_message "DEBUG" "$1"
}

# Function to rotate logs
rotate_logs() {
    if [ -f "${LOG_FILE}" ]; then
        local size=$(/usr/bin/stat -f %z "${LOG_FILE}")
        if [ ${size} -gt 5242880 ]; then  # 5MB
            local timestamp=$(get_utc_time | tr ' ' '_')
            /bin/mv "${LOG_FILE}" "${LOG_FILE}.${timestamp}"
            /usr/bin/touch "${LOG_FILE}"
            /bin/chmod 640 "${LOG_FILE}"
            log_info "Log file rotated due to size > 5MB"
        fi
    fi
}

# Synchronize system time
sync_time() {
    if /usr/bin/which ntpdate >/dev/null 2>&1; then
        /usr/sbin/ntpdate -u pool.ntp.org >/dev/null 2>&1 || true
    fi
}

# Initialize logging with time sync
sync_time
init_logging
rotate_logs