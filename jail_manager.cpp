#include "jail_manager.hpp"
#include "FreeBSD.hpp"
#include <iostream>
#include <stdexcept>
#include <array>
#include <cstdio>
#include <string>
#include <unistd.h>

std::string JailManager::executeCommand(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    
    int status = pclose(pipe);
    if (status != 0) {
        throw std::runtime_error("Command failed: " + command);
    }
    
    return result;
}

void JailManager::runCommand(const std::string& command) {
    if (system(command.c_str()) != 0) {
        throw std::runtime_error("Command failed: " + command);
    }
}

bool JailManager::verifyJailService(const std::string& service_check) {
    std::string cmd = std::string(FreeBSD::CMD_IOCAGE) + " exec " + 
                      FreeBSD::JAIL_NAME + " " + service_check + 
                      " >/dev/null 2>&1";
    return system(cmd.c_str()) == 0;
}

void JailManager::configureWine() {
    if (!isRunning() && !startJail()) {
        throw std::runtime_error("Failed to start jail for Wine configuration");
    }

    std::cout << "Installing Wine and dependencies...\n";
    
    runCommand(std::string(FreeBSD::CMD_IOCAGE) + " exec " + FreeBSD::JAIL_NAME + 
              " " + FreeBSD::CMD_PKG + " update -f");
              
    runCommand(std::string(FreeBSD::CMD_IOCAGE) + " exec " + FreeBSD::JAIL_NAME + 
              " " + FreeBSD::CMD_PKG + " install -y wine-proton winetricks vulkan-tools");

    std::cout << "Setting up Wine environment...\n";
    
    std::string wine_init = std::string(FreeBSD::CMD_IOCAGE) + " exec -U " + 
                           FreeBSD::JAIL_USER + " " + FreeBSD::JAIL_NAME + " " + 
                           FreeBSD::CMD_ENV + " WINE=" + FreeBSD::CMD_WINE + " " + 
                           FreeBSD::CMD_WINE + " wineboot -i";
    runCommand(wine_init);

    std::cout << "Installing Wine components...\n";
    for (const char* component : FreeBSD::WINE_COMPONENTS) {
        std::cout << "Installing " << component << "...\n";
        std::string tricks_cmd = std::string(FreeBSD::CMD_IOCAGE) + " exec -U " + 
                                FreeBSD::JAIL_USER + " " + FreeBSD::JAIL_NAME + " " + 
                                FreeBSD::CMD_ENV + " WINE=" + FreeBSD::CMD_WINE + " " + 
                                FreeBSD::CMD_WINETRICKS + " " + component;
        runCommand(tricks_cmd);
    }

    sleep(2);
}

void JailManager::installSteam() {
    if (!isRunning() && !startJail()) {
        throw std::runtime_error("Failed to start jail for Steam installation");
    }

    std::cout << "Downloading Steam installer...\n";
    std::string fetch_cmd = std::string(FreeBSD::CMD_IOCAGE) + " exec -U " + 
                           FreeBSD::JAIL_USER + " " + FreeBSD::JAIL_NAME + " " + 
                           FreeBSD::CMD_FETCH + " -o /tmp/SteamSetup.exe " + 
                           FreeBSD::STEAM_INSTALLER;
    runCommand(fetch_cmd);

    std::cout << "Installing Steam...\n";
    std::string install_cmd = std::string(FreeBSD::CMD_IOCAGE) + " exec -U " + 
                             FreeBSD::JAIL_USER + " " + FreeBSD::JAIL_NAME + " " + 
                             FreeBSD::CMD_ENV + " WINE=" + FreeBSD::CMD_WINE + " " + 
                             FreeBSD::CMD_WINE + " /tmp/SteamSetup.exe /S";
    runCommand(install_cmd);

    std::string cleanup_cmd = std::string(FreeBSD::CMD_IOCAGE) + " exec " + 
                             FreeBSD::JAIL_NAME + " " + FreeBSD::CMD_RM + 
                             " /tmp/SteamSetup.exe";
    runCommand(cleanup_cmd);

    std::cout << "Steam installation completed.\n";
}

// ... rest of the implementation