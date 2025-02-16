#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

class ConfigManager {
public:
    static bool createJailConf();
    static bool createPackagesJson();
};

#endif