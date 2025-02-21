# Build Instructions for steamjailer

## Prerequisites

Install required packages using pkg:
```sh
pkg install -y \
    cmake \
    curl \
    nlohmann-json \
    zenity
```

## Build Steps

1. Clone and enter the repository:
```sh
cd ~/Downloads
git clone https://github.com/user/steamjailer.git
cd steamjailer
```

2. Create and enter build directory:
```sh
mkdir -p build
cd build
```

3. Configure with CMake:
```sh
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DENABLE_TESTING=ON \
    -DBUILD_DOCUMENTATION=ON
```

4. Build the project:
```sh
make
```

5. Install (requires root privileges):
```sh
doas make install
```

## Verification

To verify the installation:
```sh
which steamjailer
steamjailer --version
```

## Common Issues

### Missing Dependencies
If CMake reports missing dependencies, install them:
```sh
pkg install -y cmake curl nlohmann-json zenity
```

### Build Directory Cleanup
If you need to start fresh:
```sh
cd ~/Downloads/steamjailer
rm -rf build
```

### Permission Issues
If you encounter permission issues during installation:
```sh
doas chown -R ${USER}:${USER} /usr/local/bin/steamjailer
```

## System Requirements

- FreeBSD 14.0 or newer
- CMake 3.31.5 or newer
- C++20 compatible compiler (Clang)
- curl 8.12.0 or newer
- nlohmann-json 3.11.2 or newer
- zenity (for GUI components)

## FreeBSD-Specific Notes

1. The build system uses FreeBSD's native package manager (pkg)
2. Uses doas instead of sudo for privilege escalation
3. Default installation path is /usr/local/bin
4. All paths follow FreeBSD FHS (Filesystem Hierarchy Standard)