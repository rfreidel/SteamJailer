#include "gui/zenity_gui.hpp"
#include "jail_manager.hpp"
#include "network_checker.hpp"
#include "wine_installer.hpp"
#include "logger.hpp"
#include <memory>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        // Initialize components
        auto gui = std::make_unique<ZenityGUI>();
        auto netChecker = std::make_unique<NetworkChecker>();
        
        // Check root privileges
        if (getuid() != 0) {
            gui->showError("This application must be run as root");
            return 1;
        }

        // Check internet connection
        if (!netChecker->checkInternetConnection()) {
            gui->showError("No internet connection detected");
            return 1;
        }

        // Initialize jail manager
        auto jailManager = std::make_unique<JailManager>();
        
        // Get jail name from user
        std::string jailName = gui->getInput("Enter jail name:");
        if (jailName.empty()) {
            return 0;
        }

        // Show progress for jail creation
        gui->showProgress("Creating Jail", "Setting up jail environment...");
        
        // Create and configure jail
        if (!jailManager->createJail(jailName)) {
            gui->showError("Failed to create jail");
            return 1;
        }
        
        gui->updateProgress(50);

        // Initialize wine installer
        auto wineInstaller = std::make_unique<WineInstaller>(jailManager->getJailPath());
        
        // Install Wine and Steam
        if (!wineInstaller->installWineProton()) {
            gui->showError("Failed to install Wine-Proton");
            return 1;
        }
        
        gui->updateProgress(75);
        
        if (!wineInstaller->installWinetricksComponents()) {
            gui->showError("Failed to install Winetricks components");
            return 1;
        }
        
        gui->updateProgress(90);
        
        if (!wineInstaller->installSteam()) {
            gui->showError("Failed to install Steam");
            return 1;
        }
        
        gui->updateProgress(100);
        gui->closeProgress();

        // Show completion message
        gui->showInfo("Setup completed successfully!\n\nJail Name: " + jailName + 
                     "\nPath: " + jailManager->getJailPath());
        
        return 0;
    } catch (const std::exception& e) {
        ZenityGUI().showError("An error occurred: " + std::string(e.what()));
        return 1;
    }
}