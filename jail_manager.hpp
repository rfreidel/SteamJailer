#ifndef JAIL_MANAGER_HPP
#define JAIL_MANAGER_HPP

#include <string>

class JailManager {
public:
    static std::string executeCommand(const std::string& command);
    static void runCommand(const std::string& command);
    static std::string getSystemVersion();
    static std::string getNetworkInterface();
    static bool exists();
    static bool isRunning();
    static bool startJail();
    static bool stopJail();
    static bool createJail(const std::string& version);
    static bool createSteamUser(const std::string& password);
    static bool validatePassword(const std::string& password);
    static void configureWine();
    static void installSteam();

private:
    static bool verifyJailService(const std::string& service_check);
};

#endif // JAIL_MANAGER_HPP