#include "jail_manager.hpp"
#include "FreeBSD.hpp"
#include <string>
#include <fstream>
#include <iostream>

bool JailManager::setupPackageSystem() {
    // Create pkg configuration directory
    std::string mkdir_cmd = std::string(FreeBSD::CMD_MKDIR) + " -p " +
                           FreeBSD::JAIL_ROOT + FreeBSD::PKG_CONF_DIR;
    if (system(mkdir_cmd.c_str()) != 0) {
        std::cerr << "Failed to create pkg config directory\n";
        return false;
    }

    // Write pkg configuration
    std::string conf_path = std::string(FreeBSD::JAIL_ROOT) + 
                           FreeBSD::PKG_CONF_DIR + "/FreeBSD.conf";
    std::ofstream conf(conf_path);
    if (!conf) {
        std::cerr << "Failed to create pkg configuration file\n";
        return false;
    }

    // Configure quarterly package repository
    conf << "FreeBSD: {\n"
         << "    url: \"pkg+http://pkg.FreeBSD.org/${ABI}/quarterly\",\n"
         << "    mirror_type: \"http\",\n"
         << "    signature_type: \"none\",\n"
         << "    enabled: yes\n"
         << "}\n";
    conf.close();

    // Copy resolv.conf for DNS resolution
    std::string cp_cmd = std::string(FreeBSD::CMD_CP) + " /etc/resolv.conf " +
                        FreeBSD::JAIL_ROOT + "/etc/resolv.conf";
    if (system(cp_cmd.c_str()) != 0) {
        std::cerr << "Failed to copy resolv.conf\n";
        return false;
    }

    // Bootstrap pkg in jail
    std::string bootstrap_cmd = std::string(FreeBSD::CMD_IOCAGE) + 
                               " exec " + FreeBSD::JAIL_NAME + " " +
                               FreeBSD::CMD_ENV + " ASSUME_ALWAYS_YES=yes " +
                               FreeBSD::CMD_PKG + " bootstrap -f";
    
    if (system(bootstrap_cmd.c_str()) != 0) {
        std::cerr << "Failed to bootstrap pkg\n";
        return false;
    }

    // Update package repository
    std::string update_cmd = std::string(FreeBSD::CMD_IOCAGE) + 
                            " exec " + FreeBSD::JAIL_NAME + " " +
                            FreeBSD::CMD_ENV + " ASSUME_ALWAYS_YES=yes " +
                            FreeBSD::CMD_PKG + " update -f";
    
    return system(update_cmd.c_str()) == 0;
}

bool JailManager::installPackages() {
    // Install packages one by one to better handle errors
    const char* packages[] = {"ca_root_nss", "wine-proton", "winetricks"};
    
    for (const char* pkg : packages) {
        std::string install_cmd = std::string(FreeBSD::CMD_IOCAGE) + 
                                " exec " + FreeBSD::JAIL_NAME + " " +
                                FreeBSD::CMD_ENV + " ASSUME_ALWAYS_YES=yes " +
                                FreeBSD::CMD_PKG + " install -y " + pkg;
        
        std::cout << "Installing " << pkg << "...\n";
        if (system(install_cmd.c_str()) != 0) {
            std::cerr << "Failed to install " << pkg << "\n";
            return false;
        }
    }
    
    return true;
}