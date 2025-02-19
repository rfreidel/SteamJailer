# Add near the top of the file, after variable declarations

# Function to ensure time is synchronized
sync_jail_time() {
    local jailname="$1"
    if [ -n "$jailname" ]; then
        # Sync host time first
        if /usr/bin/which ntpdate >/dev/null 2>&1; then
            /usr/sbin/ntpdate -u pool.ntp.org >/dev/null 2>&1 || true
        fi
        
        # Get host time
        local host_time=$(/bin/date -u "+%Y-%m-%d %H:%M:%S")
        
        # Set jail time
        /usr/sbin/jexec "$jailname" /bin/date -u -s "$host_time" >/dev/null 2>&1 || true
    fi
}

# Modify the do_start function to include time sync
do_start() {
    local jailname="$1"
    echo "Starting jail: ${jailname}"
    log_info "Starting jail: ${jailname}"
    /usr/sbin/service jail start "${jailname}"
    
    # Sync time after jail starts
    sync_jail_time "${jailname}"
}