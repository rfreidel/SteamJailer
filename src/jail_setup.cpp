#include "jail_setup.hpp"
#include "FreeBSD.hpp"
#include <fstream>
#include <iostream>

bool JailSetup::createJailConf() {
    std::ofstream conf(FreeBSD::JAIL_CONF);
    if (!conf) {
        std::cerr << "Error: Cannot create " << FreeBSD::JAIL_CONF << "\n";
        return false;
    }

    conf << "exec.start = \"/bin/sh /etc/rc\";\n"
         << "exec.stop = \"/bin/sh /etc/rc.shutdown\";\n"
         << "exec.clean;\n"
         << "mount.devfs;\n"
         << "allow.raw_sockets;\n"
         << "allow.sysvipc;\n\n"
         << "steamjail {\n"
         << "    host.hostname = \"steamjail\";\n"
         << "    path = \"" << FreeBSD::JAIL_PATH << "\";\n"
         << "    interface = \"lo1\";\n"
         << "    ip4.addr = 127.0.1.1;\n"
         << "    persist;\n"
         << "}\n";

    conf.close();
    return true;
}

bool JailSetup::createPackagesJson() {
    std::ofstream json("packages.json");
    if (!json) {
        std::cerr << "Error: Cannot create packages.json\n";
        return false;
    }

    json << "{\n"
         << "    \"pkgs\": [\n"
         << "        \"wine-proton\",\n"
         << "        \"winetricks\",\n"
         << "        \"ca_root_nss\"\n"
         << "    ]\n"
         << "}\n";

    json.close();
    return true;
}

bool JailSetup::setupDNS() {
    std::string cp_cmd = std::string(FreeBSD::CMD_CP) + " " +
                        FreeBSD::RESOLV_CONF + " " +
                        FreeBSD::JAIL_PATH + "/etc/resolv.conf";
    return system(cp_cmd.c_str()) == 0;
}

bool JailSetup::enableIocage() {
    std::string cmd = std::string(FreeBSD::CMD_SERVICE) + " iocage onestart";
    return system(cmd.c_str()) == 0;
}