#ifndef FREEBSD_HPP
#define FREEBSD_HPP

namespace FreeBSD {
    // FreeBSD system commands with POSIX paths
    constexpr const char* CMD_DATE = "/bin/date \"+%Y-%m-%d %H:%M:%S\"";  // Fixed date format
    constexpr const char* CMD_ID = "/usr/bin/id";
    constexpr const char* CMD_ID_USER = "/usr/bin/id -un";  // Added for getting username
    constexpr const char* CMD_PW = "/usr/sbin/pw";
    constexpr const char* CMD_PKG = "/usr/sbin/pkg";
    constexpr const char* CMD_IOCAGE = "/usr/local/bin/iocage";
    constexpr const char* CMD_WINE = "/usr/local/wine-proton/bin/wine";
    constexpr const char* CMD_WINETRICKS = "/usr/local/bin/winetricks";
    constexpr const char* CMD_FREEBSD_VERSION = "/bin/freebsd-version -ru";
    constexpr const char* CMD_FETCH = "/usr/bin/fetch";
    constexpr const char* CMD_GREP = "/usr/bin/grep";
    constexpr const char* CMD_JLS = "/usr/sbin/jls -N";
    constexpr const char* CMD_RM = "/bin/rm -f";
    constexpr const char* CMD_MKDIR = "/bin/mkdir -p";
    constexpr const char* CMD_CHMOD = "/bin/chmod";
    constexpr const char* CMD_CHOWN = "/usr/sbin/chown";
    constexpr const char* CMD_SHELL = "/bin/sh";
    constexpr const char* CMD_ENV = "/usr/bin/env";
    
    // Wine components
    constexpr const char* WINE_COMPONENTS[] = {
        "dxvk",
        "nvapi",
        "d3dcompiler_47",
        "dotnet7",
        "faudio",
        "vcrun2022",
        "webio",
        "vkd3d"
    };
    
    // Jail configuration
    constexpr const char* JAIL_NAME = "steamjail";
    constexpr const char* JAIL_USER = "steam";
    constexpr const char* JAIL_GROUP = "wheel";
    constexpr const char* DEFAULT_JAIL_IP = "127.0.1.1";
    constexpr const char* WINE_PREFIX = "/home/steam/.wine";
    constexpr const char* STEAM_INSTALLER = "https://cdn.cloudflare.steamstatic.com/client/installer/SteamSetup.exe";
}

#endif // FREEBSD_HPP