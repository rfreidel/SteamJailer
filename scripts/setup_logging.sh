#!/bin/sh

# setup_logging.sh - Configure logging for jail automation
# Must be run as root

set -e

# Create log directory if it doesn't exist
mkdir -p /var/log

# Create log file with proper permissions
touch /var/log/jailautomation.log
chown root:wheel /var/log/jailautomation.log
chmod 640 /var/log/jailautomation.log

# Configure newsyslog
cp config/jailautomation.newsyslog.conf /etc/newsyslog.conf.d/jailautomation.conf

# Configure syslog
cat > /etc/syslog.d/jailautomation.conf <<EOF
local0.*                                            /var/log/jailautomation.log
EOF

# Restart syslog service
service syslogd restart

echo "Logging system configured successfully"