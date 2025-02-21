#include "jail_manager.hpp"
#include "logger.hpp"
#include <cstdlib>
#include <array>
#include <memory>
#include <stdexcept>

JailManager::JailManager() {
    LOG_INFO("Initializing Jail Manager");
}

bool JailManager::createJail(const std::string& jailName) {
    LOG_INFO("Creating jail: " + jailName);
    m_jailName = jailName;
    m_jailPath = "/usr/local/jail/" + jailName;

    if (!setupBaseJail()) {
        LOG_ERROR("Failed to set up base jail");
        return false;
    }

    if (!configureJailConf()) {
        LOG_ERROR("Failed to configure jail.conf");
        return false;
    }

    LOG_NOTICE("Jail " + jailName + " created successfully");
    return true;
}

bool JailManager::executeCommand(const std::string& command) {
    LOG_DEBUG("Executing command: " + command);
    
    std::array<char, 128> buffer;
    std::string result;
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        LOG_ERROR("Command execution failed: " + command);
        return false;
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    
    int status = pclose(pipe);
    if (status != 0) {
        LOG_ERROR("Command failed with status " + std::to_string(status) + ": " + command);
        return false;
    }
    
    LOG_DEBUG("Command executed successfully");
    return true;
}

bool JailManager::configureNetworking() {
    LOG_INFO("Configuring networking for jail: " + m_jailName);
    
    // Load VNET kernel module if not loaded
    if (!executeCommand("kldload if_epair || true")) {
        LOG_ERROR("Failed to load if_epair kernel module");
        return false;
    }
    
    // Create and configure VNET interface
    if (!executeCommand("ifconfig epair0 create")) {
        LOG_ERROR("Failed to create epair interface");
        return false;
    }
    
    LOG_NOTICE("Network configuration completed successfully");
    return true;
}