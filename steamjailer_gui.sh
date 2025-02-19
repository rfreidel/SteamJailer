#!/bin/sh

# Current Date and Time (UTC - YYYY-MM-DD HH:MM:SS formatted): 2024-02-19 07:00:00
# Current User's Login: rfreidel
# Do use FreeBSD-commands Posix-syntax
# Don't use iocage

# Base configuration
JAILBASE="/jail"
VERBOSE=1

# Check if dialog is in path, if not use full path
if which dialog >/dev/null 2>&1; then
    DIALOG="dialog"
else
    DIALOG="/usr/local/bin/dialog"
fi

# Create temp file safely
tempfile=$(mktemp /tmp/steamjailer.XXXXXX) || exit 1
trap "rm -f $tempfile" 0 1 2 5 15

# Test dialog installation and display
$DIALOG --clear --backtitle "Steam Jail Manager" \
        --title "Initializing" \
        --infobox "Starting Steam Jail Manager..." 5 40 || {
    # If dialog fails, try to install it
    echo "Installing dialog package..."
    env ASSUME_ALWAYS_YES=YES pkg install dialog
    
    # Recheck dialog after installation
    if ! which dialog >/dev/null 2>&1; then
        echo "Failed to install dialog. Please install it manually:"
        echo "# pkg install dialog"
        exit 1
    fi
}

# Function to display menu
show_menu() {
    $DIALOG --clear --backtitle "Steam Jail Manager" \
            --title "Main Menu" \
            --menu "Choose an operation:" 20 60 11 \
            1 "Create New Jail" \
            2 "Destroy Jail" \
            3 "Start Jail" \
            4 "Stop Jail" \
            5 "Install Steam Environment" \
            6 "Install Winetricks Modules" \
            7 "Update Jail" \
            8 "List Jails" \
            9 "View Logs" \
            10 "Exit" 2> "$tempfile"
    return $?
}

# List jails function
list_jails() {
    local jail_list=""
    if [ -d "${JAILBASE}" ]; then
        for jail in ${JAILBASE}/*; do
            if [ -d "${jail}" ]; then
                jailname=$(basename "${jail}")
                if /usr/sbin/jls -j "${jailname}" >/dev/null 2>&1; then
                    status="Running"
                else
                    status="Stopped"
                fi
                jail_list="${jail_list} ${jailname} ${status}"
            fi
        done
    fi
    echo "$jail_list"
}

# Main loop
while true; do
    show_menu || exit 1
    
    choice=$(cat "$tempfile")
    
    case "$choice" in
        1)
            $DIALOG --inputbox "Enter new jail name:" 8 60 2> "$tempfile"
            if [ $? -eq 0 ]; then
                jailname=$(cat "$tempfile")
                ./steamjailer.sh -c "$jailname" 2>&1 | $DIALOG --programbox "Creating jail..." 20 70
            fi
            ;;
        2)
            jail_list=$(list_jails)
            if [ -n "$jail_list" ]; then
                $DIALOG --menu "Select jail to destroy:" 20 60 10 $jail_list 2> "$tempfile"
                if [ $? -eq 0 ]; then
                    jailname=$(cat "$tempfile")
                    if $DIALOG --yesno "Are you sure you want to destroy $jailname?" 8 60; then
                        ./steamjailer.sh -d "$jailname" 2>&1 | $DIALOG --programbox "Destroying jail..." 20 70
                    fi
                fi
            else
                $DIALOG --msgbox "No jails found." 6 60
            fi
            ;;
        3)
            jail_list=$(list_jails)
            if [ -n "$jail_list" ]; then
                $DIALOG --menu "Select jail to start:" 20 60 10 $jail_list 2> "$tempfile"
                if [ $? -eq 0 ]; then
                    jailname=$(cat "$tempfile")
                    ./steamjailer.sh -s "$jailname" 2>&1 | $DIALOG --programbox "Starting jail..." 20 70
                fi
            else
                $DIALOG --msgbox "No jails found." 6 60
            fi
            ;;
        4)
            jail_list=$(list_jails)
            if [ -n "$jail_list" ]; then
                $DIALOG --menu "Select jail to stop:" 20 60 10 $jail_list 2> "$tempfile"
                if [ $? -eq 0 ]; then
                    jailname=$(cat "$tempfile")
                    ./steamjailer.sh -S "$jailname" 2>&1 | $DIALOG --programbox "Stopping jail..." 20 70
                fi
            else
                $DIALOG --msgbox "No jails found." 6 60
            fi
            ;;
        5)
            jail_list=$(list_jails)
            if [ -n "$jail_list" ]; then
                $DIALOG --menu "Select jail for Steam installation:" 20 60 10 $jail_list 2> "$tempfile"
                if [ $? -eq 0 ]; then
                    jailname=$(cat "$tempfile")
                    ./steamjailer.sh -i "$jailname" 2>&1 | $DIALOG --programbox "Installing Steam..." 20 70
                fi
            else
                $DIALOG --msgbox "No jails found." 6 60
            fi
            ;;
        6)
            jail_list=$(list_jails)
            if [ -n "$jail_list" ]; then
                $DIALOG --menu "Select jail for Winetricks installation:" 20 60 10 $jail_list 2> "$tempfile"
                if [ $? -eq 0 ]; then
                    jailname=$(cat "$tempfile")
                    ./steamjailer.sh -w "$jailname" 2>&1 | $DIALOG --programbox "Installing Winetricks modules..." 20 70
                fi
            else
                $DIALOG --msgbox "No jails found." 6 60
            fi
            ;;
        7)
            jail_list=$(list_jails)
            if [ -n "$jail_list" ]; then
                $DIALOG --menu "Select jail to update:" 20 60 10 $jail_list 2> "$tempfile"
                if [ $? -eq 0 ]; then
                    jailname=$(cat "$tempfile")
                    ./steamjailer.sh -u "$jailname" 2>&1 | $DIALOG --programbox "Updating jail..." 20 70
                fi
            else
                $DIALOG --msgbox "No jails found." 6 60
            fi
            ;;
        8)
            jail_list=$(list_jails)
            if [ -n "$jail_list" ]; then
                echo "$jail_list" | $DIALOG --title "Installed Jails" --programbox 20 60
            else
                $DIALOG --msgbox "No jails found." 6 60
            fi
            ;;
        9)
            if [ -f "/var/log/steamjailer/steamjailer.log" ]; then
                $DIALOG --title "Steam Jail Logs" --textbox "/var/log/steamjailer/steamjailer.log" 24 80
            else
                $DIALOG --msgbox "No logs found." 6 60
            fi
            ;;
        10)
            clear
            exit 0
            ;;
        *)
            $DIALOG --msgbox "Invalid option" 6 60
            ;;
    esac
done