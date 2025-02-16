#!/bin/sh

# FreeBSD POSIX paths
CXX="/usr/bin/c++"
MKDIR="/bin/mkdir -p"

# Create build directory
${MKDIR} build

# Compile
${CXX} -std=c++17 src/main.cpp -o build/create_json

echo "Build complete. Run './build/create_json' to generate packages.json"