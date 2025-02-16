bool JailManager::checkJail() {
    // Check if jail exists
    std::string check_cmd = "/usr/local/bin/iocage list | /usr/bin/grep -q " + 
                           std::string(FreeBSD::JAIL_NAME);
    return system(check_cmd.c_str()) == 0;
}

bool JailManager::createJail(const std::string& version) {
    try {
        // First verify jail doesn't exist
        if (checkJail()) {
            std::cout << "Jail already exists.\n";
            return true;
        }

        std::cout << "Creating new jail...\n";

        // Create base jail
        std::string create_cmd = "/usr/local/bin/iocage create"
                                " -r " + version +
                                " -n " + FreeBSD::JAIL_NAME +
                                " ip4_addr=\"lo0|" + FreeBSD::DEFAULT_JAIL_IP + "/32\"" +
                                " boot=on" +
                                " allow_raw_sockets=1" +
                                " allow_mlock=1";
        
        runCommand(create_cmd);

        // Configure jail for Steam
        std::string update_cmd = "/usr/local/bin/iocage update " + FreeBSD::JAIL_NAME;
        runCommand(update_cmd);

        // Install required packages in jail
        std::string pkg_cmd = "/usr/local/bin/iocage exec " + FreeBSD::JAIL_NAME +
                             " /usr/sbin/pkg install -y wine-proton winetricks";
        runCommand(pkg_cmd);

        // Create steam user
        std::string user_cmd = "/usr/local/bin/iocage exec " + FreeBSD::JAIL_NAME +
                              " /usr/sbin/pw useradd " + FreeBSD::JAIL_USER +
                              " -m -G " + FreeBSD::JAIL_GROUP;
        runCommand(user_cmd);

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating jail: " << e.what() << std::endl;
        return false;
    }
}

void JailManager::runCommand(const std::string& command) {
    std::cout << "Executing: " << command << std::endl;
    if (system(command.c_str()) != 0) {
        throw std::runtime_error("Command failed: " + command);
    }
}

std::string JailManager::getSystemVersion() {
    return executeCommand("/bin/freebsd-version -ru");
}