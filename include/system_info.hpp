#ifndef SYSTEM_INFO_HPP
#define SYSTEM_INFO_HPP

class SystemInfo {
public:
    static void printDate();          // Prints current date using FreeBSD date command
    static void printUser();          // Prints current user using FreeBSD whoami
    static bool isRoot();             // Checks if running as root using FreeBSD id
};

#endif