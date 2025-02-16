#include "jail_manager.hpp"
#include <iostream>
#include <unistd.h>

int main() {
    try {
        if (geteuid() != 0) {
            throw std::runtime_error("This program must be run as root");
        }

        std::cout << "Starting Steam jail setup..." << std::endl;
        
        std::string version = JailManager::getSystemVersion();
        std::cout << "FreeBSD Version: " << version << std::endl;
        
        if (!JailManager::exists()) {
            if (!JailManager::createJail(version)) {
                throw std::runtime_error("Failed to create jail");
            }
        }
        
        if (!JailManager::startJail()) {
            throw std::runtime_error("Failed to start jail");
        }
        
        JailManager::configureWine();
        JailManager::installSteam();

        std::cout << "\nSteam installation completed successfully." << std::endl;
        std::cout << "To start Steam, run: sudo iocage exec " << FreeBSD::JAIL_NAME 
                 << " wine ~/.wine/drive_c/Program\\ Files\\ \\(x86\\)/Steam/Steam.exe" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}