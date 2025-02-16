#ifndef IOCAGE_INSTALLER_HPP
#define IOCAGE_INSTALLER_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include "FreeBSD.hpp"

class IocageInstaller {
public:
    static std::string getSystemVersion();
    static void install();
    static void setupJail();
    static void configureWine();
    static void installSteam();
    
private:
    static std::string executeCommand(const std::string& command);
    static void runCommand(const std::string& command);
    static bool isRoot();
    static std::string getNetworkInterface();
    static std::string getIPAddress(const std::string& interface);
    static bool verifyPackage(const std::string& pkgName);
    static void setupWinePrefix();
};

#endif // IOCAGE_INSTALLER_HPP