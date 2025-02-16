#include "jail_manager.hpp"
#include "FreeBSD.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

int main() {
    try {
        // Check if running as root
        std::string id_output = JailManager::executeCommand(FreeBSD::CMD_ID + std::string(" -u"));
        if (id_output != "0\n" && id_output != "0") {
            throw std::runtime_error("This program must be run as root.\n"
                                   "Please use 'su -' to become root first.");
        }

        // Display system information
        std::cout << "Current Date and Time (UTC - YYYY-MM-DD HH:MM:SS formatted): " 
                  << JailManager::executeCommand(FreeBSD::CMD_DATE) << std::endl;
        
        std::cout << "Current User's Login: " 
                  << JailManager::executeCommand(FreeBSD::CMD_ID_USER) << std::endl;

        // Get FreeBSD version
        std::string version = JailManager::getSystemVersion();
        std::cout << "FreeBSD Version: " << version << std::endl;

        // Rest of the implementation...
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}