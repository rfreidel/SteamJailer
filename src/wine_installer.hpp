#pragma once
#include <string>
#include <vector>

class WineInstaller {
public:
    WineInstaller(const std::string& jailPath);
    ~WineInstaller();

    bool installWineProton();
    bool installWinetricks();
    bool setupWineComponents();
    bool installSteam();

private:
    std::string m_jailPath;
    std::string m_winePath;
    
    bool executeWineCommand(const std::string& command);
    bool installWinetricksComponent(const std::string& component);
};