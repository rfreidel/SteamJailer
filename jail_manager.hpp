#ifndef JAIL_MANAGER_HPP
#define JAIL_MANAGER_HPP

#include <string>
#include "FreeBSD.hpp"

class JailManager {
public:
    static bool createJail(const std::string& version);
    static bool startJail();
    static bool stopJail();
    static bool isRunning();
    static bool exists();
    static std::string getSystemVersion();
    static std::string getNetworkInterface();
    static void configureWine();
    static void installSteam();

private:
    static std::string executeCommand(const std::string& command);
    static void runCommand(const std::string& command);
};

#endif // JAIL_MANAGER_HPP