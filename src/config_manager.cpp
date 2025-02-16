#include "config_manager.hpp"
#include "FreeBSD.hpp"
#include <fstream>
#include <iostream>

bool ConfigManager::createJailConf() {
    // Create /etc directory if it doesn't exist
    system((std::string(FreeBSD::CMD_MKDIR) + " -p /etc").c_str());
    
    // Create initial jail.conf
    std::ofstream jailconf("/etc/jail.conf");
    if (!jailconf) {
        std::cerr << "Failed to create /etc/jail.conf\n";
        return false;
    }

    jailconf << "exec.start = \"/bin/sh /etc/rc\";\n"
             << "exec.stop = \"/bin/sh /etc/rc.shutdown\";\n"
             << "exec.clean;\n"
             << "mount.devfs;\n"
             << "allow.raw_sockets;\n"
             << "allow.sysvipc;\n\n"
             << "steamjail {\n"
             << "    host.hostname = \"steamjail\";\n"
             << "    path = \"/zroot/iocage/jails/steamjail/root\";\n"
             << "    interface = \"lo1\";\n"
             << "    ip4.addr = 127.0.1.1;\n"
             << "    persist;\n"
             << "}\n";

    jailconf.close();
    return true;
}

bool ConfigManager::createPackagesJson() {
    std::ofstream pkgjson("packages.json");
    if (!pkgjson) {
        std::cerr << "Failed to create packages.json\n";
        return false;
    }

    pkgjson << "{\n"
            << "    \"pkgs\": [\n"
            << "        \"ca_root_nss\",\n"
            << "        \"wine-proton\",\n"
            << "        \"winetricks\"\n"
            << "    ]\n"
            << "}\n";

    pkgjson.close();
    return true;
}