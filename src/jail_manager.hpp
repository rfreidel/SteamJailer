#pragma once
#include <string>
#include <vector>
#include <memory>

class JailManager {
public:
    JailManager();
    ~JailManager();

    bool createJail(const std::string& jailName);
    bool configureNetworking();
    bool mountZFS(const std::string& dataset);
    bool startJail();
    bool stopJail();
    
    std::string getJailPath() const;
    std::string getJailIP() const;

private:
    std::string m_jailName;
    std::string m_jailPath;
    std::string m_jailIP;
    
    bool executeCommand(const std::string& command);
    bool setupBaseJail();
    bool configureJailConf();
};