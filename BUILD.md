# Build Instructions for Jail Automation Project

## System Requirements

- FreeBSD 14.2-RELEASE-p1 or newer
- CMake 3.31.5 or newer
- C++20 compatible compiler (Clang)

## Required Packages

Install the required packages using pkg:

```sh
# Install build dependencies
pkg install -y \
    cmake \
    pkgconf \
    git \
    curl \
    nlohmann-json \
    wine-proton \
    winetricks

# Optional documentation dependencies
pkg install -y \
    ronn \
    man
```

## Building the Project

1. Clone the repository:
```sh
git clone https://github.com/rfreidel/steamjailer.git
cd jail-automation
```

2. Create and enter build directory:
```sh
mkdir build
cd build
```

3. Configure the project:
```sh
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DENABLE_TESTING=ON \
    -DBUILD_DOCUMENTATION=ON
```

4. Build the project:
```sh
make -j$(sysctl -n hw.ncpu)
```

5. Run tests (optional):
```sh
make test
```

6. Install the project (requires root privileges):
```sh
doas make install
```

## Installation Locations

The project will be installed in the following locations:

- Executable: `/usr/local/bin/jail-automation`
- Configuration: `/usr/local/etc/jailautomation/`
- Man pages: `/usr/local/man/man1/`
- Log file: `/var/log/jailautomation.log`

## Verify Installation

1. Check the installation:
```sh
# Verify executable
which jail-automation

# Check version
jail-automation --version

# View man page
man jail-automation
```

2. Verify logging setup:
```sh
# Check log file permissions
ls -l /var/log/jailautomation.log

# Verify syslog configuration
grep jailautomation /etc/syslog.conf
```

## Required ZFS Configuration

Ensure ZFS is properly configured:

```sh
# Check ZFS status
zfs list
zpool status

# Create jail dataset if not exists
doas zfs create -o mountpoint=/usr/local/jail zroot/jails
```

## Network Configuration

Verify network configuration requirements:

```sh
# Check if VNET modules are loaded
kldstat | grep if_epair

# Load VNET module if needed
doas kldload if_epair

# Check PF status
service pf status

# Enable PF if needed
doas sysrc pf_enable="YES"
doas service pf start
```

## Common Build Issues

### Permission Issues
If you encounter permission issues:
```sh
# Fix log file permissions
doas touch /var/log/jailautomation.log
doas chown root:wheel /var/log/jailautomation.log
doas chmod 640 /var/log/jailautomation.log
```

### CMake Configuration Issues
If CMake can't find wxWidgets:
```sh
# Set wxWidgets paths explicitly
cmake .. \
    -DwxWidgets_CONFIG_EXECUTABLE=/usr/local/bin/wx-config \
    -DwxWidgets_wxrc_EXECUTABLE=/usr/local/bin/wxrc-gtk3-3.2
```

### Build Directory Cleanup
To clean and rebuild:
```sh
# Clean build directory
cd build
make clean
rm -rf *

# Reconfigure and rebuild
cmake ..
make
```

## Running the Application

After installation, run the application with:
```sh
# Run with default settings
doas jail-automation

# Run with verbose logging
doas jail-automation --verbose

# Run with specific config file
doas jail-automation --config /path/to/config.json
```

## Uninstalling

To uninstall the application:
```sh
cd build
doas make uninstall

# Remove log file and configuration
doas rm -f /var/log/jailautomation.log
doas rm -rf /usr/local/etc/jailautomation
```

## Development Setup

For development work:
```sh
# Generate compilation database for IDE integration
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

# Enable debug build with symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

## Additional Notes

- All commands use FreeBSD-specific syntax and POSIX compliance
- The build system requires root privileges for installation
- Log rotation is handled by newsyslog
- The application integrates with FreeBSD's rc system
- ZFS datasets are used for jail storage
- PF is used for network configuration
