#ifndef FREEBSD_HPP
#define FREEBSD_HPP

namespace FreeBSD {
    // FreeBSD POSIX commands
    constexpr const char* CMD_DATE = "/bin/date";
    constexpr const char* CMD_WHOAMI = "/usr/bin/whoami";
    constexpr const char* CMD_IOCAGE = "/usr/local/bin/iocage";
    constexpr const char* CMD_PKG = "/usr/sbin/pkg";
    constexpr const char* CMD_CP = "/bin/cp";
    constexpr const char* CMD_MKDIR = "/bin/mkdir";
    constexpr const char* CMD_TOUCH = "/usr/bin/touch";
    constexpr const char* CMD_UNAME = "/usr/bin/uname";
    constexpr const char* CMD_SERVICE = "/usr/sbin/service";

    // Paths
    constexpr const char* JAIL_CONF = "/etc/jail.conf";
    constexpr const char* RESOLV_CONF = "/etc/resolv.conf";
    constexpr const char* JAIL_PATH = "/zroot/iocage/jails/steamjail/root";
}
#endif