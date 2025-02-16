#ifndef FREEBSD_HPP
#define FREEBSD_HPP

#include <string>

namespace FreeBSD {
    // FreeBSD standard command paths
    constexpr const char* CMD_PKG = "/usr/sbin/pkg";
    constexpr const char* CMD_IOCAGE = "/usr/local/bin/iocage";
    constexpr const char* CMD_IFCONFIG = "/sbin/ifconfig";
    constexpr const char* CMD_WINE = "/usr/local/bin/wine";
    constexpr const char* CMD_WINETRICKS = "/usr/local/bin/winetricks";
    constexpr const char* CMD_FREEBSD_VERSION = "/bin/freebsd-version";
    constexpr const char* CMD_FETCH = "/usr/bin/fetch";
    constexpr const char* CMD_AWK = "/usr/bin/awk";
    constexpr const char* CMD_GREP = "/usr/bin/grep";
    
    // Default values
    constexpr const char* STEAM_INSTALLER = "https://cdn.cloudflare.steamstatic.com/client/installer/SteamSetup.exe";
    constexpr const char* DEFAULT_JAIL_IP = "127.0.1.1";
    constexpr const char* JAIL_NAME = "steamjail";
}

#endif // FREEBSD_HPP