int main() {
    try {
        // Check root privileges
        if (system("/usr/bin/id -u | /usr/bin/grep -q '^0$'") != 0) {
            throw std::runtime_error("This program must be run as root");
        }

        // Get current time
        std::cout << "Current Date and Time (UTC): " 
                  << JailManager::executeCommand("/bin/date -u \"+%Y-%m-%d %H:%M:%S\"");

        // Get current user
        std::cout << "Current User: " 
                  << JailManager::executeCommand("/usr/bin/whoami");

        // Get FreeBSD version
        std::string version = JailManager::getSystemVersion();
        std::cout << "FreeBSD Version: " << version;

        // Create jail if it doesn't exist
        if (!JailManager::checkJail()) {
            if (!JailManager::createJail(version)) {
                throw std::runtime_error("Failed to create jail");
            }
        }

        // Start jail if not running
        if (!JailManager::isRunning()) {
            std::cout << "Starting jail...\n";
            if (!JailManager::startJail()) {
                throw std::runtime_error("Failed to start jail");
            }
        }

        std::cout << "\nJail setup completed successfully!\n";
        std::cout << "To install Steam, run:\n";
        std::cout << "/usr/local/bin/iocage exec -U " << FreeBSD::JAIL_USER 
                  << " " << FreeBSD::JAIL_NAME 
                  << " /usr/bin/env WINEPREFIX=/home/" << FreeBSD::JAIL_USER 
                  << "/.wine /usr/local/wine-proton/bin/wine wineboot --init\n\n";
        std::cout << "Then download and install Steam:\n";
        std::cout << "/usr/local/bin/iocage exec -U " << FreeBSD::JAIL_USER 
                  << " " << FreeBSD::JAIL_NAME 
                  << " /usr/bin/fetch -o /home/" << FreeBSD::JAIL_USER 
                  << "/SteamSetup.exe " << FreeBSD::STEAM_INSTALLER << "\n\n";
        std::cout << "/usr/local/bin/iocage exec -U " << FreeBSD::JAIL_USER 
                  << " " << FreeBSD::JAIL_NAME 
                  << " /usr/bin/env WINEPREFIX=/home/" << FreeBSD::JAIL_USER 
                  << "/.wine /usr/local/wine-proton/bin/wine /home/" 
                  << FreeBSD::JAIL_USER << "/SteamSetup.exe /S\n";

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}