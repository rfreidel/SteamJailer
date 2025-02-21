#pragma once
#include <string>

class NetworkChecker {
public:
    NetworkChecker();
    ~NetworkChecker();

    bool checkInternetConnection();
    bool checkLocalhostConnection();
    std::string getLocalIP() const;

private:
    bool testConnection(const std::string& host);
    std::string m_localIP;
};