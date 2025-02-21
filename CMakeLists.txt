cmake_minimum_required(VERSION 3.31.5)
project(jail_automation VERSION 1.0
    DESCRIPTION "FreeBSD Jail Automation with Wine-Proton"
    LANGUAGES CXX)

# Use modern C++ features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable modern CMake features
include(FetchContent)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find required packages
find_package(CURL REQUIRED)
find_package(nlohmann_json 3.11.2 REQUIRED)

# Find zenity
find_program(ZENITY_PATH zenity REQUIRED)

# Set FreeBSD-specific options
if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    set(CMAKE_INSTALL_PREFIX "/usr/local")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    list(APPEND CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")
endif()

# Add source files
set(SOURCES
    src/main.cpp
    src/jail_manager.cpp
    src/network_checker.cpp
    src/wine_installer.cpp
    src/gui/zenity_gui.cpp
    src/gui/steam_launcher.cpp
    src/logger.cpp
)

# Create executable
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PRIVATE
        ${SOURCES}
)

# Configure zenity path
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.hpp
)

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        CURL::libcurl
        nlohmann_json::nlohmann_json
)