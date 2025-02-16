#include "iocage_installer.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <memory>
#include <array>
#include <regex>

std::string IocageInstaller::executeCommand(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;
    
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("Command execution failed: " + command);
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    
    return result;
}

void IocageInstaller::runCommand(const std::string& command) {
    std::cout << "Executing: " << command << std::endl;
    if (system(command.c_str()) != 0) {
        throw std::runtime_error("Command failed: " + command);
    }
}

bool IocageInstaller::isRoot() {
    return (geteuid() == 0);
}

std::string IocageInstaller::getSystemVersion() {
    return executeCommand(std::string(CMD_FREEBSD_VERSION) + " -r");
}

std::string IocageInstaller::getNetworkInterface() {
    std::string result = executeCommand(std::string(CMD_IFCONFIG) + " -l");
    std::istringstream iss(result);
    std::string iface;
    
    while (iss >> iface) {
        if (iface.find("lo") == std::string::npos) {
            return iface;
        }
    }
    
    return "lo0";
}

std::string IocageInstaller::getIPAddress(const std::string& interface) {
    std::string cmd = std::string(CMD_IFCONFIG) + " " + interface + " inet | " + 
                     CMD_AWK + " '/inet / {print $2}'";
    std::string result = executeCommand(cmd);
    return result.empty() ? "127.0.0.1" : result;
}

bool IocageInstaller::verifyPackage(const std::string& pkgName) {
    return system((std::string(CMD_PKG) + " info -e " + pkgName).c_str()) == 0;
}

void IocageInstaller::install() {
    if (!isRoot()) {
        throw std::runtime_error("This program must be run as root");
    }

    std::string version = getSystemVersion();
    std::cout << "FreeBSD Version: " << version << std::endl;

    // Update and install packages
    runCommand(std::string(CMD_PKG) + " update");
    
    if (!verifyPackage("py311-iocage")) {
        runCommand(std::string(CMD_PKG) + " install -y py311-iocage");
    }
    if (!verifyPackage("wine-proton")) {
        runCommand(std::string(CMD_PKG) + " install -y wine-proton");
    }
    if (!verifyPackage("winetricks")) {
        runCommand(std::string(CMD_PKG) + " install -y winetricks");
    }

    // Activate iocage
    runCommand(std::string(CMD_ZFS) + " list zroot >/dev/null 2>&1");
    runCommand(std::string(CMD_IOCAGE) + " activate zroot");
}

void IocageInstaller::setupJail() {
    std::string version = getSystemVersion();
    std::string interface = getNetworkInterface();
    std::string ip = getIPAddress(interface);

    // Check if jail exists
    if (system((std::string(CMD_IOCAGE) + " list | " + CMD_GREP + " -q steamjail").c_str()) == 0) {
        std::cout << "Jail 'steamjail' already exists." << std::endl;
        return;
    }

    // Create jail with current system version
    runCommand(std::string(CMD_IOCAGE) + " fetch release=" + version);
    runCommand(std::string(CMD_IOCAGE) + " create -r " + version + 
              " -n steamjail ip4_addr=\"" + interface + "|" + ip + 
              "/24\" allow_raw_sockets=1 boot=on");

    // Start and configure jail
    runCommand(std::string(CMD_IOCAGE) + " start steamjail");
    runCommand(std::string(CMD_IOCAGE) + " fstab -a steamjail devfs /dev devfs rw 0 0");
    runCommand(std::string(CMD_IOCAGE) + " fstab -a steamjail procfs /proc procfs rw 0 0");
}

void IocageInstaller::setupWinePrefix() {
    // Create wine prefix directory
    runCommand(std::string(CMD_IOCAGE) + " exec steamjail mkdir -p /home/steam/.wine");
    
    // Initialize wine
    runCommand(std::string(CMD_IOCAGE) + " exec steamjail " + CMD_WINE + " wineboot");
    
    // Configure wine
    runCommand(std::string(CMD_IOCAGE) + " exec steamjail " + CMD_WINETRICKS + 
              " corefonts vcrun2019");
}

void IocageInstaller::configureWine() {
    // Update jail packages
    runCommand(std::string(CMD_IOCAGE) + " exec steamjail " + CMD_PKG + " update");
    runCommand(std::string(CMD_IOCAGE) + " exec steamjail " + CMD_PKG + 
              " install -y wine-proton winetricks");
    
    setupWinePrefix();
}

void IocageInstaller::installSteam() {
    // Download Steam installer
    runCommand(std::string(CMD_IOCAGE) + " exec steamjail " + CMD_FETCH + 
              " -o /tmp/SteamSetup.exe " + STEAM_INSTALLER);
    
    // Install Steam
    runCommand(std::string(CMD_IOCAGE) + " exec steamjail " + CMD_WINE + 
              " /tmp/SteamSetup.exe /S");
}