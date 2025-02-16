#ifndef FREEBSD_HPP
#define FREEBSD_HPP

namespace FreeBSD {
    // FreeBSD POSIX commands (updated with correct paths)
    constexpr const char* CMD_DATE = "/bin/date";
    constexpr const char* CMD_ID = "/usr/bin/id";
    constexpr const char* CMD_WHOAMI = "/usr/bin/whoami";
    constexpr const char* CMD_PKG = "/usr/sbin/pkg";
    constexpr const char* CMD_IOCAGE = "/usr/local/bin/iocage";
    constexpr const char* CMD_SERVICE = "/usr/sbin/service";
    constexpr const char* CMD_SYSRC = "/usr/sbin/sysrc";
    constexpr const char* CMD_ZFS = "/sbin/zfs";
    constexpr const char* CMD_WINE = "/usr/local/wine-proton/bin/wine";
    constexpr const char* CMD_WINETRICKS = "/usr/local/bin/winetricks";
    constexpr const char* CMD_FETCH = "/usr/bin/fetch";
    constexpr const char* CMD_GREP = "/usr/bin/grep";
    constexpr const char* CMD_AWK = "/usr/bin/awk";
    constexpr const char* CMD_CHMOD = "/bin/chmod";
    constexpr const char* CMD_CHOWN = "/usr/sbin/chown";

    // Jail configuration
    constexpr const char* JAIL_NAME = "steamjail";
    constexpr const char* JAIL_USER = "steam";
    constexpr const char* JAIL_GROUP = "wheel";
    constexpr const char* DEFAULT_JAIL_IP = "127.0.1.1";
    constexpr const char* STEAM_INSTALLER = "https://cdn.cloudflare.steamstatic.com/client/installer/SteamSetup.exe";
}

#endif // FREEBSD_HPP