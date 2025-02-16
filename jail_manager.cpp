#include "jail_manager.hpp"
#include <iostream>
#include <array>
#include <stdexcept>
#include <regex>
#include <cstdio>
#include <unistd.h>
#include <cstring>

std::string JailManager::executeCommand(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("Failed to execute: " + command);
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    
    if (pclose(pipe) != 0) {
        throw std::runtime_error("Command failed: " + command);
    }
    
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    
    return result;
}

void JailManager::runCommand(const std::string& command) {
    std::cout << "Executing: " << command << std::endl;
    if (system(command.c_str()) != 0) {
        throw std::runtime_error("Command failed: " + command);
    }
}

std::string JailManager::getSystemVersion() {
    std::string version = executeCommand(std::string(FreeBSD::CMD_FREEBSD_VERSION) + " -r");
    std::regex version_regex(R"((\d+\.\d+)-.*?)");
    std::smatch matches;
    if (std::regex_search(version, matches, version_regex)) {
        return matches[1].str() + "-RELEASE";
    }
    throw std::runtime_error("Could not determine FreeBSD version");
}

std::string JailManager::getNetworkInterface() {
    std::string result = executeCommand(std::string(FreeBSD::CMD_IFCONFIG) + " -l");
    
    // Convert string to char array for parsing
    char* cstr = new char[result.length() + 1];
    std::strcpy(cstr, result.c_str());
    
    // Parse space-separated interfaces
    char* token = std::strtok(cstr, " \t\n");
    std::string interface = "lo0"; // Default fallback
    
    while (token != nullptr) {
        std::string iface(token);
        if (iface.find("lo") == std::string::npos) {
            interface = iface;
            break;
        }
        token = std::strtok(nullptr, " \t\n");
    }
    
    delete[] cstr;
    return interface;
}

bool JailManager::exists() {
    std::string cmd = std::string(FreeBSD::CMD_IOCAGE) + " list | " + 
                     FreeBSD::CMD_GREP + " -q " + FreeBSD::JAIL_NAME;
    return system(cmd.c_str()) == 0;
}

bool JailManager::isRunning() {
    try {
        std::string cmd = std::string(FreeBSD::CMD_IOCAGE) + " list | " + 
                         FreeBSD::CMD_GREP + " " + FreeBSD::JAIL_NAME + 
                         " | " + FreeBSD::CMD_AWK + " '{ print $3 }'";
        return executeCommand(cmd) == "up";
    } catch (const std::exception&) {
        return false;
    }
}

bool JailManager::createJail(const std::string& version) {
    if (exists()) {
        std::cout << "Jail already exists" << std::endl;
        return true;
    }

    try {
        std::string interface = getNetworkInterface();
        
        // Fetch release
        runCommand(std::string(FreeBSD::CMD_IOCAGE) + " fetch release=" + version);
        
        // Create jail
        std::string create_cmd = std::string(FreeBSD::CMD_IOCAGE) + " create"
            " -r " + version +
            " -n " + FreeBSD::JAIL_NAME +
            " ip4_addr=\"" + interface + "|" + FreeBSD::DEFAULT_JAIL_IP + "/24\"" +
            " allow_raw_sockets=1" +
            " boot=on";
        
        runCommand(create_cmd);

        // Mount filesystems
        runCommand(std::string(FreeBSD::CMD_IOCAGE) + " fstab -a " + FreeBSD::JAIL_NAME + 
                  " devfs /dev devfs rw 0 0");
        runCommand(std::string(FreeBSD::CMD_IOCAGE) + " fstab -a " + FreeBSD::JAIL_NAME + 
                  " procfs /proc procfs rw 0 0");
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating jail: " << e.what() << std::endl;
        return false;
    }
}

bool JailManager::startJail() {
    if (!exists()) {
        std::cerr << "Jail does not exist" << std::endl;
        return false;
    }

    if (isRunning()) {
        std::cout << "Jail is already running" << std::endl;
        return true;
    }

    try {
        runCommand(std::string(FreeBSD::CMD_IOCAGE) + " start " + FreeBSD::JAIL_NAME);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error starting jail: " << e.what() << std::endl;
        return false;
    }
}

bool JailManager::stopJail() {
    if (!exists()) {
        std::cerr << "Jail does not exist" << std::endl;
        return false;
    }

    if (!isRunning()) {
        std::cout << "Jail is already stopped" << std::endl;
        return true;
    }

    try {
        runCommand(std::string(FreeBSD::CMD_IOCAGE) + " stop " + FreeBSD::JAIL_NAME);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error stopping jail: " << e.what() << std::endl;
        return false;
    }
}

void JailManager::configureWine() {
    if (!isRunning()) {
        if (!startJail()) {
            throw std::runtime_error("Failed to start jail for Wine configuration");
        }
    }

    // Install packages
    runCommand(std::string(FreeBSD::CMD_IOCAGE) + " exec " + FreeBSD::JAIL_NAME + 
              " " + FreeBSD::CMD_PKG + " update -f");
    runCommand(std::string(FreeBSD::CMD_IOCAGE) + " exec " + FreeBSD::JAIL_NAME + 
              " " + FreeBSD::CMD_PKG + " install -y wine-proton winetricks");

    // Setup Wine
    runCommand(std::string(FreeBSD::CMD_IOCAGE) + " exec " + FreeBSD::JAIL_NAME + 
              " mkdir -p /home/steam/.wine");
    runCommand(std::string(FreeBSD::CMD_IOCAGE) + " exec " + FreeBSD::JAIL_NAME + 
              " " + FreeBSD::CMD_WINE + " wineboot");
    runCommand(std::string(FreeBSD::CMD_IOCAGE) + " exec " + FreeBSD::JAIL_NAME + 
              " " + FreeBSD::CMD_WINETRICKS + " corefonts vcrun2019");
}

void JailManager::installSteam() {
    if (!isRunning()) {
        if (!startJail()) {
            throw std::runtime_error("Failed to start jail for Steam installation");
        }
    }

    runCommand(std::string(FreeBSD::CMD_IOCAGE) + " exec " + FreeBSD::JAIL_NAME + 
              " " + FreeBSD::CMD_FETCH + " -o /tmp/SteamSetup.exe " + FreeBSD::STEAM_INSTALLER);
    runCommand(std::string(FreeBSD::CMD_IOCAGE) + " exec " + FreeBSD::JAIL_NAME + 
              " " + FreeBSD::CMD_WINE + " /tmp/SteamSetup.exe /S");
    runCommand(std::string(FreeBSD::CMD_IOCAGE) + " exec " + FreeBSD::JAIL_NAME + 
              " rm -f /tmp/SteamSetup.exe");
}