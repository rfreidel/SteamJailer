int main() {
    try {
        // Check if running as root
        if (system("/usr/bin/id -u | /usr/bin/grep -q '^0$'") != 0) {
            throw std::runtime_error("This program must be run as root");
        }

        // Enable iocage
        system("/usr/sbin/sysrc -q iocage_enable=\"YES\"");
        system("/usr/sbin/service iocage onestart");

        // Create and start jail
        std::string version = JailManager::getSystemVersion();
        if (!JailManager::exists() && !JailManager::createJail(version)) {
            throw std::runtime_error("Failed to create jail");
        }

        if (!JailManager::isRunning() && !JailManager::startJail()) {
            throw std::runtime_error("Failed to start jail");
        }

        // Install Steam
        JailManager::installSteam();

        std::cout << "\nTo launch Steam, use:\n"
                  << "/usr/local/bin/iocage exec -U " << FreeBSD::JAIL_USER 
                  << " " << FreeBSD::JAIL_NAME 
                  << " /usr/bin/env HOME=/home/" << FreeBSD::JAIL_USER 
                  << " WINEPREFIX=/home/" << FreeBSD::JAIL_USER << "/.wine "
                  << "/usr/local/wine-proton/bin/wine"
                  << " ~/.wine/drive_c/Program\\ Files\\ \\(x86\\)/Steam/Steam.exe\n";

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}