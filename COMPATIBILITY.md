# FreeBSD Version Compatibility Guide

## Version Requirements

### FreeBSD 14.2-RELEASE (Recommended)
- Full native support
- All features available
- Latest package versions

### FreeBSD 13.4/13.5-RELEASE (Legacy Support)
- Requires additional configuration
- Some package version adjustments needed
- Minor feature limitations

## Package Version Differences

| Package | FreeBSD 14.2 | FreeBSD 13.4/13.5 | Adjustment Needed |
|---------|--------------|-------------------|-------------------|
| CMake | 3.31.5 | 3.24.x | Use minimum version 3.24 |
| Clang | 16.0.x | 14.0.x | Update C++20 flags |
| zenity | 3.44.x | 3.32.x | Adjust dialog options |
| wine-proton | 8.0.x | 7.0.x | Check compatibility |

## Required Adjustments

### 1. CMake Configuration
```cmake
# Update CMakeLists.txt for 13.4/13.5 compatibility
cmake_minimum_required(VERSION 3.24)  # Lower version requirement

# Add FreeBSD version check
if(CMAKE_SYSTEM_VERSION VERSION_LESS 14.0)
    message(STATUS "Building for FreeBSD 13.x - Adjusting features")
    set(FREEBSD_LEGACY_BUILD TRUE)
endif()
```

### 2. Compiler Settings
```cmake
# Adjust C++ standard settings for older Clang
if(FREEBSD_LEGACY_BUILD)
    set(CMAKE_CXX_STANDARD 17)  # Fall back to C++17 if needed
    add_compile_options(-Wno-deprecated-declarations)
else()
    set(CMAKE_CXX_STANDARD 20)
endif()
```

### 3. Package Installation
```sh
# FreeBSD 13.4/13.5 specific package installation
pkg install -y \
    cmake>=3.24.0 \
    pkgconf \
    git \
    curl \
    nlohmann-json \
    zenity>=3.32.0 \
    wine-proton>=7.0 \
    winetricks
```

## Feature Compatibility Matrix

| Feature | FreeBSD 14.2 | FreeBSD 13.4/13.5 | Notes |
|---------|--------------|-------------------|-------|
| ZFS Features | Full | Limited | Adjust dataset properties |
| VNET/Jails | Full | Basic | Check jail parameters |
| Wine-Proton | Latest | Limited | Test game compatibility |
| GUI Elements | Full | Basic | Adjust zenity dialogs |

## Required System Adjustments

### 1. Boot Loader Settings (13.4/13.5)
```sh
# Add to /boot/loader.conf
kern.racct.enable=1
```

### 2. Kernel Module Requirements
```sh
# Check and load required modules
kldload if_epair
kldload nullfs
```

### 3. System Configuration
```sh
# Update /etc/sysctl.conf
sysctl security.jail.allow_raw_sockets=1
sysctl security.jail.socket_unixiproute_only=1
```

## Installation Process (13.4/13.5)

1. Check system compatibility:
```sh
# Check FreeBSD version
freebsd-version

# Check system requirements
sysctl kern.features | grep VNET
sysctl kern.features | grep RACCT
```

2. Install base requirements:
```sh
# Update package repository
pkg update

# Install base packages
pkg install -y \
    cmake \
    git \
    curl \
    zenity
```

3. Configure build environment:
```sh
# Create build directory
mkdir build
cd build

# Configure with legacy support
cmake .. \
    -DFREEBSD_LEGACY_BUILD=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local
```

4. Build and install:
```sh
# Build with adjusted parameters
make -j$(sysctl -n hw.ncpu)

# Install with proper permissions
doas make install
```

## Known Limitations (13.4/13.5)

1. ZFS Features
- Some advanced ZFS features may not be available
- Use basic dataset properties only

2. Jail Configuration
```sh
# Use basic jail parameters
jail -c name="testjail" \
    path="/usr/local/jail/testjail" \
    host.hostname="testjail" \
    ip4.addr="192.168.1.100" \
    command="/bin/sh"
```

3. Wine-Proton Compatibility
- Test games individually for compatibility
- Some newer games may not work with older Wine-Proton versions

## Troubleshooting

### Common Issues on 13.4/13.5

1. VNET Issues:
```sh
# Check VNET support
sysctl net.inet.ip.forwarding=1
service netif restart
```

2. Permission Issues:
```sh
# Adjust jail permissions
chown -R root:wheel /usr/local/jail
chmod 755 /usr/local/jail
```

3. Package Conflicts:
```sh
# Force specific package versions
pkg install -f wine-proton-7.0
```

## Performance Considerations

1. Resource Limits
```sh
# Adjust resource limits in /etc/rc.conf
jail_parameters="allow.raw_sockets=1 allow.socket_af=1"
```

2. Memory Management
```sh
# Set conservative memory limits
rctl -a jail:testjail:memoryuse:deny=8G
```

## Upgrade Path

Consider upgrading to FreeBSD 14.2 for:
- Better performance
- Full feature support
- Latest security updates
- Improved compatibility