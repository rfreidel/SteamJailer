#ifndef FREEBSD_HPP
#define FREEBSD_HPP

#include <string>

namespace FreeBSD {
    // FreeBSD standard command paths with POSIX syntax
    constexpr const char* CMD_PKG = "/usr/sbin/pkg";
    constexpr const char* CMD_IOCAGE = "/usr/local/bin/iocage";
    constexpr const char* CMD_IFCONFIG = "/sbin/ifconfig";
    constexpr const char* CMD_WINE = "/usr/local/bin/wine";
    constexpr const char* CMD_WINETRICKS = "/usr/local/bin/winetricks";
    constexpr const char* CMD_FREEBSD_VERSION = "/bin/freebsd-version";
    constexpr const char* CMD_FETCH = "/usr/bin/fetch";
    constexpr const char* CMD_AWK = "/usr/bin/awk";
    constexpr const char* CMD_GREP = "/usr/bin/grep";
    constexpr const char* CMD_JLS = "/usr/sbin/jls";
    constexpr const char* CMD_JAIL = "/usr/sbin/jail";
    constexpr const char* CMD_JEXEC = "/usr/sbin/jexec";
    constexpr const char* CMD_RM = "/bin/rm";
    constexpr const char* CMD_MKDIR = "/bin/mkdir";
    
    // Default values
    constexpr const char* STEAM_INSTALLER = "https://cdn.cloudflare.steamstatic.com/client/installer/SteamSetup.exe";
    constexpr const char* DEFAULT_JAIL_IP = "127.0.1.1";
    constexpr const char* JAIL_NAME = "steamjail";  // Added this line
    constexpr const char* JAIL_USER = "steam";
    constexpr const char* WINE_PREFIX = "/home/steam/.wine";
}

#endif // FREEBSD_HPP