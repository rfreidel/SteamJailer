#include "jail_manager.hpp"
#include "FreeBSD.hpp"
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <iomanip>

int main() {
    try {
        if (geteuid() != 0) {
            throw std::runtime_error("This program must be run as root");
        }

        // Get and display current time
        std::time_t now = std::time(nullptr);
        std::cout << "Installation started at: " 
                  << std::put_time(std::gmtime(&now), "%Y-%m-%d %H:%M:%S UTC") 
                  << std::endl;

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
        std::cout << "To start Steam, run: " << FreeBSD::CMD_IOCAGE << " exec " 
                 << FreeBSD::JAIL_NAME << " " << FreeBSD::CMD_WINE 
                 << " ~/.wine/drive_c/Program\\ Files\\ \\(x86\\)/Steam/Steam.exe" 
                 << std::endl;
        
        // Get and display completion time
        now = std::time(nullptr);
        std::cout << "Installation completed at: " 
                  << std::put_time(std::gmtime(&now), "%Y-%m-%d %H:%M:%S UTC") 
                  << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}