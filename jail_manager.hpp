#ifndef JAIL_MANAGER_HPP
#define JAIL_MANAGER_HPP

#include <string>

class JailManager {
public:
    // Core utility methods
    static std::string executeCommand(const std::string& command);
    static void runCommand(const std::string& command);
    
    // Jail management
    static bool exists();
    static bool isRunning();
    static bool startJail();
    static bool stopJail();
    static bool createJail(const std::string& version);
    
    // System info and validation
    static std::string getSystemVersion();
    static std::string getNetworkInterface();
    static bool validatePassword(const std::string& password);
    
    // User management
    static bool createSteamUser(const std::string& password);
    
    // Wine and Steam setup
    static void configureWine();
    static void installSteam();

private:
    static bool verifyJailService(const std::string& service_check);
    static bool checkJailHealth();
};

#endif // JAIL_MANAGER_HPP