#!/bin/sh

# version_check.sh - Check system compatibility for jail automation project
# Returns 0 if system is compatible, 1 if not

# Check FreeBSD version
FREEBSD_VERSION=$(freebsd-version -u | cut -d '-' -f1)
MAJOR_VERSION=$(echo "${FREEBSD_VERSION}" | cut -d '.' -f1)
MINOR_VERSION=$(echo "${FREEBSD_VERSION}" | cut -d '.' -f2)

# Function to compare versions
version_compare() {
    if [ "$1" -lt "$2" ]; then
        return 1
    elif [ "$1" -eq "$2" ] && [ "$3" -lt "$4" ]; then
        return 1
    fi
    return 0
}

# Check minimum version requirements
if ! version_compare "$MAJOR_VERSION" "13" "$MINOR_VERSION" "4"; then
    echo "Error: FreeBSD version ${FREEBSD_VERSION} is not supported"
    echo "Minimum required version is 13.4"
    exit 1
fi

# Check required features
echo "Checking system requirements..."

# Check VNET support
if ! sysctl -n kern.features.vimage >/dev/null 2>&1; then
    echo "Error: VNET support not available"
    echo "Add 'vimage_load=\"YES\"' to /boot/loader.conf"
    exit 1
fi

# Check RACCT support
if ! sysctl -n kern.racct.enable >/dev/null 2>&1; then
    echo "Error: RACCT support not enabled"
    echo "Add 'kern.racct.enable=1' to /boot/loader.conf"
    exit 1
fi

# Check ZFS support
if ! zfs list >/dev/null 2>&1; then
    echo "Error: ZFS not available"
    echo "ZFS support is required"
    exit 1
fi

# Check package availability
echo "Checking package availability..."
if ! pkg -N >/dev/null 2>&1; then
    echo "Error: pkg not available"
    exit 1
fi

# All checks passed
echo "System compatibility check passed"
if [ "$MAJOR_VERSION" -eq 13 ]; then
    echo "Notice: Running on FreeBSD 13.x - Some features may be limited"
    echo "Consider upgrading to FreeBSD 14.2 for full feature support"
fi

exit 0