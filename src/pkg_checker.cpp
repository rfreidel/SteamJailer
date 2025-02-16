#include "pkg_checker.hpp"
#include "FreeBSD.hpp"
#include <iostream>
#include <string>
#include <array>
#include <memory>

std::string execCommand(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    
    return result;
}

bool PkgChecker::checkPkgConfig() {
    std::cout << "Checking pkg repository configuration...\n";
    
    // Check current pkg configuration
    std::string check_cmd = std::string(FreeBSD::CMD_PKG) + " -vv | " + 
                           FreeBSD::CMD_GREP + " " + "\"Repositories\"";
    
    try {
        std::string pkg_config = execCommand(check_cmd.c_str());
        std::cout << "Current pkg configuration:\n" << pkg_config << "\n";
        
        // Check if using latest
        if (pkg_config.find("latest") != std::string::npos) {
            std::cout << "Confirmed using 'latest' repository\n";
        } else {
            std::cout << "Note: System appears to be using 'quarterly' repository\n";
            std::cout << "To switch to 'latest', you can run:\n";
            std::cout << "sudo pkg update -f\n";
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error checking pkg configuration: " << e.what() << "\n";
        return false;
    }
    
    return true;
}

bool PkgChecker::checkInstalledPackages() {
    std::cout << "Checking installed packages...\n";
    
    try {
        // Check wine-proton
        std::string wine_check = std::string(FreeBSD::CMD_PKG) + " info | " + 
                                FreeBSD::CMD_GREP + " wine-proton";
        std::string wine_status = execCommand(wine_check.c_str());
        
        // Check winetricks
        std::string tricks_check = std::string(FreeBSD::CMD_PKG) + " info | " + 
                                 FreeBSD::CMD_GREP + " winetricks";
        std::string tricks_status = execCommand(tricks_check.c_str());
        
        std::cout << "\nPackage Status:\n";
        std::cout << "wine-proton: " << (wine_status.empty() ? "Not installed" : "Installed") << "\n";
        std::cout << "winetricks: " << (tricks_status.empty() ? "Not installed" : "Installed") << "\n";
        
    } catch (const std::runtime_error& e) {
        std::cerr << "Error checking packages: " << e.what() << "\n";
        return false;
    }
    
    return true;
}