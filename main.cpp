#include "iocage_installer.hpp"
#include <iostream>

int main() {
    try {
        std::cout << "Starting Steam jail setup..." << std::endl;
        
        IocageInstaller::install();
        IocageInstaller::setupJail();
        IocageInstaller::configureWine();
        IocageInstaller::installSteam();

        std::cout << "\nSteam installation completed successfully." << std::endl;
        std::cout << "To start Steam, run: sudo iocage exec steamjail wine ~/.wine/drive_c/Program\\ Files\\ \\(x86\\)/Steam/Steam.exe" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}