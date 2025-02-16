#include "jail_manager.hpp"
#include "FreeBSD.hpp"
#include <iostream>
#include <stdexcept>
#include <array>
#include <cstdio>
#include <string>
#include <unistd.h>

// Existing methods...

std::string JailManager::getSystemVersion() {
    return executeCommand(FreeBSD::CMD_FREEBSD_VERSION);
}

bool JailManager::isRunning() {
    std::string cmd = std::string(FreeBSD::CMD_JLS) + " -j " + 
                      FreeBSD::JAIL_NAME + " >/dev/null 2>&1";
    return system(cmd.c_str()) == 0;
}

bool JailManager::exists() {
    std::string cmd = std::string(FreeBSD::CMD_IOCAGE) + " list | " + 
                      FreeBSD::CMD_GREP + " -q " + FreeBSD::JAIL_NAME;
    return system(cmd.c_str()) == 0;
}

bool JailManager::startJail() {
    if (!exists()) {
        std::cerr << "Jail does not exist" << std::endl;
        return false;
    }

    try {
        if (isRunning()) {
            std::cout << "Jail is already running" << std::endl;
            return true;
        }

        // Clean stop any existing instance
        std::string stop_cmd = std::string(FreeBSD::CMD_IOCAGE) + " stop " + 
                              FreeBSD::JAIL_NAME + " >/dev/null 2>&1 || true";
        system(stop_cmd.c_str());
        
        // Wait for stop
        for (int i = 0; i < 5 && isRunning(); i++) {
            sleep(1);
        }

        std::cout << "Starting jail..." << std::endl;
        runCommand(std::string(FreeBSD::CMD_IOCAGE) + " start " + FreeBSD::JAIL_NAME);

        // Wait for jail to initialize
        for (int i = 0; i < 10; i++) {
            std::cout << "Waiting for jail to initialize... (" << (i+1) << "/10)" << std::endl;
            
            if (isRunning() && 
                verifyJailService("/usr/bin/true") && 
                verifyJailService(FreeBSD::CMD_PKG + std::string(" -N"))) {
                std::cout << "Jail startup confirmed" << std::endl;
                return true;
            }
            sleep(1);
        }

        throw std::runtime_error("Jail failed to start properly");
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

    try {
        if (!isRunning()) {
            std::cout << "Jail is not running" << std::endl;
            return true;
        }

        std::cout << "Stopping jail..." << std::endl;
        runCommand(std::string(FreeBSD::CMD_IOCAGE) + " stop " + FreeBSD::JAIL_NAME);

        // Wait for stop
        for (int i = 0; i < 5 && isRunning(); i++) {
            sleep(1);
        }

        if (isRunning()) {
            std::cerr << "Warning: Using force stop" << std::endl;
            runCommand(std::string(FreeBSD::CMD_IOCAGE) + " stop -f " + FreeBSD::JAIL_NAME);
            sleep(2);
        }

        return !isRunning();
    } catch (const std::exception& e) {
        std::cerr << "Error stopping jail: " << e.what() << std::endl;
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
        
        // Fetch FreeBSD release
        runCommand(std::string(FreeBSD::CMD_IOCAGE) + " fetch release=" + version);
        
        // Create jail with proper network configuration
        std::string create_cmd = std::string(FreeBSD::CMD_IOCAGE) + 
            " create -r " + version +
            " -n " + FreeBSD::JAIL_NAME +
            " ip4_addr=\"" + interface + "|" + FreeBSD::DEFAULT_JAIL_IP + "/24\"" +
            " allow_raw_sockets=1" +
            " boot=on" +
            " allow_mlock=1" +  // Allow memory locking for Steam
            " sysvmsg=new" +    // System V IPC for Wine
            " sysvsem=new" +
            " sysvshm=new";
        
        runCommand(create_cmd);

        // Configure jail mounts
        runCommand(std::string(FreeBSD::CMD_IOCAGE) + " fstab -a " + 
                  FreeBSD::JAIL_NAME + " devfs /dev devfs rw 0 0");
        runCommand(std::string(FreeBSD::CMD_IOCAGE) + " fstab -a " + 
                  FreeBSD::JAIL_NAME + " procfs /proc procfs rw 0 0");
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating jail: " << e.what() << std::endl;
        return false;
    }
}

std::string JailManager::getNetworkInterface() {
    // Get the first non-loopback interface
    std::string cmd = std::string(FreeBSD::CMD_IFCONFIG) + " -l | " + 
                      FreeBSD::CMD_GREP + " -v '^lo'";
    std::string result = executeCommand(cmd);
    
    // Get first word (interface name)
    size_t pos = result.find_first_of(" \t\n\r");
    if (pos != std::string::npos) {
        result = result.substr(0, pos);
    }
    
    if (result.empty()) {
        throw std::runtime_error("No suitable network interface found");
    }
    
    return result;
}

// ... rest of the implementation remains the same