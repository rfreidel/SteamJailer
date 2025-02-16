#ifndef JAIL_MANAGER_HPP
#define JAIL_MANAGER_HPP

class JailManager {
public:
    // Jail Management
    static bool createJail();         // Creates jail with required settings
    static bool startJail();          // Starts the jail
    static bool stopJail();           // Stops the jail
    static bool isRunning();          // Checks if jail is running
    static bool exists();             // Checks if jail exists

    // Package Management
    static bool updatePackages();     // Updates pkg in jail
    static bool installWineProton();  // Installs wine-proton and winetricks

    // Steam Installation
    static bool setupSteamUser();     // Creates steam user in jail
    static bool downloadSteam();      // Downloads Steam installer
    static bool installSteam();       // Installs Steam using wine-proton

    // Utilities
    static bool executeCommand(const char* command);  // Executes FreeBSD commands
    static bool configureNetwork();    // Sets up jail networking
};

#endif