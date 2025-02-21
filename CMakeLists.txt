cmake_minimum_required(VERSION 3.20)
project(jail_automation VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(wxWidgets REQUIRED COMPONENTS core base)
find_package(CURL REQUIRED)
find_package(nlohmann_json 3.11.2 REQUIRED)

# Add source files
set(SOURCES
    src/main.cpp
    src/jail_manager.cpp
    src/network_checker.cpp
    src/wine_installer.cpp
    src/gui/main_window.cpp
    src/gui/steam_launcher.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    wxWidgets::core
    wxWidgets::base
    CURL::libcurl
    nlohmann_json::nlohmann_json
)