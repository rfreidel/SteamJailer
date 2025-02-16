#include "jail_manager.hpp"
#include "FreeBSD.hpp"
#include <iostream>
#include <array>
#include <stdexcept>
#include <regex>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <ctime>

namespace {
    std::string executeCommand(const std::string& command) {
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
        
        return result;
    }

    void runCommand(const std::string& command) {
        std::cout << "Executing: " << command << std::endl;
        if (system(command.c_str()) != 0) {
            throw std::runtime_error("Command failed: " + command);
        }
    }
}

bool JailManager::startJail() {
    if (!exists()) {
        std::cerr << "Jail does not exist" << std::endl;
        return false;
    }

    try {
        // First, try to stop the jail cleanly (ignore errors)
        std::string stop_cmd = std::string(FreeBSD::CMD_IOCAGE) + " stop " + 
                              FreeBSD::JAIL_NAME + " 2>/dev/null || true";
        system(stop_cmd.c_str());
        
        sleep(2);
        
        if (isRunning()) {
            // Try force stop
            stop_cmd = std::string(FreeBSD::CMD_IOCAGE) + " stop -f " + 
                      FreeBSD::JAIL_NAME + " 2>/dev/null || true";
            system(stop_cmd.c_str());
            sleep(2);
        }
        
        std::cout << "Starting jail..." << std::endl;
        runCommand(std::string(FreeBSD::CMD_IOCAGE) + " start " + FreeBSD::JAIL_NAME);
        
        sleep(2);
        if (!isRunning()) {
            throw std::runtime_error("Jail failed to start after command completed");
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error managing jail: " << e.what() << std::endl;
        return false;
    }
}

bool JailManager::isRunning() {
    try {
        std::string cmd = std::string(FreeBSD::CMD_JLS) + " -j " + 
                         FreeBSD::JAIL_NAME + " jid 2>/dev/null";
        return system(cmd.c_str()) == 0;
    } catch (const std::exception&) {
        return false;
    }
}

bool JailManager::exists() {
    std::string cmd = std::string(FreeBSD::CMD_IOCAGE) + " get all " + 
                     FreeBSD::JAIL_NAME + " 2>/dev/null >/dev/null";
    return system(cmd.c_str()) == 0;
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
    char* cstr = new char[result.length() + 1];
    std::strcpy(cstr, result.c_str());
    
    char* token = std::strtok(cstr, " \t\n");
    std::string interface = "lo0";
    
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

bool JailManager::createJail(const std::string& version) {
    if (exists()) {
        std::cout << "Jail already exists" << std::endl;
        return true;
    }

    try {
        std::string interface = getNetworkInterface();
        
        runCommand(std::string(FreeBSD::CMD_IOCAGE) + " fetch release=" + version);
        
        std::string create_cmd = std::string(FreeBSD::CMD_IOCAGE) + " create"
            " -r " + version +
            " -n " + FreeBSD::JAIL_NAME +
            " ip4_addr=\"" + interface + "|" + FreeBSD::DEFAULT_JAIL_IP + "/24\"" +
            " allow_raw_sockets=1" +
            " boot=on";
        
        runCommand(create_cmd);

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

void JailManager::configureWine() {
    if (!isRunning()) {
        if (!startJail()) {
            throw std::runtime_error("Failed to start jail for Wine configuration");
        }
    }

    runCommand(std::string(FreeBSD::CMD_IOCAGE) + " exec " + FreeBSD::JAIL_NAME + 
              " " + FreeBSD::CMD_PKG + " update -f");
    runCommand(std::string(FreeBSD::CMD_IOCAGE) + " exec " + FreeBSD::JAIL_NAME + 
              " " + FreeBSD::CMD_PKG + " install -y wine-proton winetricks");

    runCommand(std::string(FreeBSD::CMD_IOCAGE) + " exec " + FreeBSD::JAIL_NAME + 
              " " + FreeBSD::CMD_MKDIR + " -p " + FreeBSD::WINE_PREFIX);
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
              " " + FreeBSD::CMD_RM + " -f /tmp/SteamSetup.exe");
}