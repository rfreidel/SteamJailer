bool JailManager::createJail(const std::string& version) {
    // Check if jail exists first
    std::string check_cmd = std::string("/usr/local/bin/iocage list | /usr/bin/grep -q ") + 
                           FreeBSD::JAIL_NAME;
    if (system(check_cmd.c_str()) == 0) {
        std::cout << "Jail already exists\n";
        return true;
    }

    try {
        // Get default interface
        std::string iface = executeCommand("/usr/bin/netstat -f inet -rn | /usr/bin/grep -w default | /usr/bin/awk '{print $6}'");
        if (iface.empty()) {
            throw std::runtime_error("Could not determine default interface");
        }

        // Create jail with proper FreeBSD POSIX syntax
        std::string create_cmd = std::string("/usr/local/bin/iocage create") +
            " -r " + version +
            " -n " + FreeBSD::JAIL_NAME +
            " ip4_addr=\"" + iface + "|" + FreeBSD::DEFAULT_JAIL_IP + "/24\"" +
            " allow_raw_sockets=1" +
            " boot=on" +
            " allow_mlock=1" +
            " sysvmsg=new" +
            " sysvsem=new" +
            " sysvshm=new";
        
        runCommand(create_cmd);

        // Mount required filesystems
        runCommand("/usr/local/bin/iocage fstab -a " + std::string(FreeBSD::JAIL_NAME) + 
                  " devfs /dev devfs rw 0 0");

        // Create steam user in jail
        std::string add_user = "/usr/local/bin/iocage exec " + std::string(FreeBSD::JAIL_NAME) + 
                              " /usr/sbin/pw useradd " + FreeBSD::JAIL_USER +
                              " -m -G " + FreeBSD::JAIL_GROUP;
        runCommand(add_user);

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating jail: " << e.what() << std::endl;
        return false;
    }
}

void JailManager::installSteam() {
    if (!isRunning()) {
        throw std::runtime_error("Jail must be running to install Steam");
    }

    try {
        // Install wine-proton and winetricks in jail
        std::string pkg_cmd = "/usr/local/bin/iocage exec " + std::string(FreeBSD::JAIL_NAME) +
                             " /usr/sbin/pkg install -y wine-proton winetricks";
        runCommand(pkg_cmd);

        // Initialize wine for steam user
        std::string wine_init = "/usr/local/bin/iocage exec -U " + 
                               std::string(FreeBSD::JAIL_USER) + " " +
                               FreeBSD::JAIL_NAME + " /usr/bin/env " +
                               "HOME=/home/" + FreeBSD::JAIL_USER + " " +
                               "WINEPREFIX=/home/" + FreeBSD::JAIL_USER + "/.wine " +
                               "/usr/local/wine-proton/bin/wine wineboot --init";
        runCommand(wine_init);

        // Download Steam installer
        std::string fetch_cmd = "/usr/local/bin/iocage exec -U " +
                               std::string(FreeBSD::JAIL_USER) + " " +
                               FreeBSD::JAIL_NAME + " /usr/bin/fetch -o " +
                               "/home/" + FreeBSD::JAIL_USER + "/SteamSetup.exe " +
                               FreeBSD::STEAM_INSTALLER;
        runCommand(fetch_cmd);

        // Install Steam
        std::string install_cmd = "/usr/local/bin/iocage exec -U " +
                                std::string(FreeBSD::JAIL_USER) + " " +
                                FreeBSD::JAIL_NAME + " /usr/bin/env " +
                                "HOME=/home/" + FreeBSD::JAIL_USER + " " +
                                "WINEPREFIX=/home/" + FreeBSD::JAIL_USER + "/.wine " +
                                "/usr/local/wine-proton/bin/wine " +
                                "/home/" + FreeBSD::JAIL_USER + "/SteamSetup.exe /S";
        runCommand(install_cmd);

        // Cleanup
        std::string cleanup = "/usr/local/bin/iocage exec " + 
                            std::string(FreeBSD::JAIL_NAME) +
                            " /bin/rm -f /home/" + FreeBSD::JAIL_USER + "/SteamSetup.exe";
        runCommand(cleanup);

        std::cout << "Steam installation completed successfully!\n";
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Steam installation failed: ") + e.what());
    }
}

bool JailManager::isRunning() {
    std::string check_cmd = "/usr/sbin/jls -N | /usr/bin/grep -q " + 
                           std::string(FreeBSD::JAIL_NAME);
    return system(check_cmd.c_str()) == 0;
}

bool JailManager::startJail() {
    if (isRunning()) {
        return true;
    }

    std::string start_cmd = "/usr/local/bin/iocage start " + 
                           std::string(FreeBSD::JAIL_NAME);
    return system(start_cmd.c_str()) == 0;
}