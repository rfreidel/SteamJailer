bool isRoot() {
    return (geteuid() == 0);
}

std::string getUsername() {
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        return std::string(pw->pw_name);
    }
    return std::string();
}

void runCommand(const std::string& command) {
    if (!isRoot()) {
        throw std::runtime_error("This program must be run as root. Please use sudo.");
    }

    std::cout << "Running command: " << command << std::endl;
    int result = std::system(command.c_str());
    if (result != 0) {
        throw std::runtime_error("Command failed: " + command);
    }
}

std::vector<std::string> getAvailableReleases() {
    std::vector<std::string> releases;
    
    // First ensure we have the release information
    runCommand("/usr/local/bin/iocage fetch --release");
    
    FILE* pipe = popen("/usr/local/bin/iocage fetch -r", "r");
    if (!pipe) {
        throw std::runtime_error("Failed to execute iocage fetch command");
    }
    
    char buffer[128];
    std::regex release_regex("^[0-9]+\\.[0-9]+-RELEASE$");
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string line(buffer);
        line = line.substr(0, line.find('\n'));
        
        if (std::regex_match(line, release_regex)) {
            releases.push_back(line);
        }
    }
    pclose(pipe);
    
    if (releases.empty()) {
        throw std::runtime_error("No FreeBSD releases found. Please check your internet connection.");
    }
    
    return releases;
}

std::string getNetworkInterface() {
    FILE* pipe = popen("/sbin/ifconfig -l", "r");
    if (!pipe) {
        throw std::runtime_error("Failed to execute ifconfig command");
    }
    
    char buffer[128];
    std::vector<std::string> interfaces;
    
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string line(buffer);
        std::istringstream iss(line);
        std::string iface;
        while (iss >> iface) {
            if (iface.find("lo") == std::string::npos) {
                interfaces.push_back(iface);
            }
        }
    }
    pclose(pipe);

    if (interfaces.empty()) {
        return "lo0";
    }
    return interfaces[0];
}

std::string getIPAddress(const std::string& interface) {
    std::string ipAddress;

    // Try IPv4 first
    std::string cmd = "/sbin/ifconfig " + interface + " inet | /usr/bin/awk '/inet / {print $2}'";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            ipAddress = buffer;
            ipAddress.erase(std::remove(ipAddress.begin(), ipAddress.end(), '\n'), ipAddress.end());
            std::regex ipv4_regex(R"(\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b)");
            if (std::regex_match(ipAddress, ipv4_regex)) {
                pclose(pipe);
                return ipAddress;
            }
        }
        pclose(pipe);
    }

    // Try IPv6 if IPv4 fails
    cmd = "/sbin/ifconfig " + interface + " inet6 | /usr/bin/awk '/inet6/ && !/fe80/ {print $2}'";
    pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            ipAddress = buffer;
            ipAddress.erase(std::remove(ipAddress.begin(), ipAddress.end(), '\n'), ipAddress.end());
        }
        pclose(pipe);
    }

    return ipAddress.empty() ? "127.0.0.1" : ipAddress;
}

void installIocage() {
    if (std::system("/usr/bin/which iocage >/dev/null 2>&1") != 0) {
        std::cout << "Installing iocage..." << std::endl;
        runCommand("/usr/sbin/pkg update && /usr/sbin/pkg install -y py39-iocage");
    } else {
        std::cout << "iocage is already installed." << std::endl;
    }
}

void activateIocage() {
    runCommand("/usr/local/bin/iocage activate zroot");
}

void setupJail() {
    // Check if jail exists
    if (std::system("/usr/local/bin/iocage list | /usr/bin/grep -q steamjailer") == 0) {
        std::cout << "Jail steamjailer already exists." << std::endl;
        return;
    }

    auto releases = getAvailableReleases();
    std::string latestRelease = releases.back();
    std::string networkInterface = getNetworkInterface();
    std::string ipAddress = getIPAddress(networkInterface);

    std::string ipScheme = "ip4_addr";
    std::regex ipv6_regex(R"(([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4})");
    if (std::regex_match(ipAddress, ipv6_regex)) {
        ipScheme = "ip6_addr";
    }

    std::string createCmd = "/usr/local/bin/iocage create -r " + latestRelease + 
                           " -n steamjailer " + ipScheme + "=\"" + 
                           networkInterface + "|" + ipAddress + "/24\" allow_raw_sockets=\"1\"";
    runCommand(createCmd);

    // Start jail and mount filesystems
    runCommand("/usr/local/bin/iocage start steamjailer");
    runCommand("/usr/local/bin/iocage fstab -a steamjailer devfs /dev devfs rw 0 0");
    runCommand("/usr/local/bin/iocage fstab -a steamjailer procfs /proc procfs rw 0 0");
}

void installPackages() {
    runCommand("/usr/local/bin/iocage exec steamjailer pkg update");
    runCommand("/usr/local/bin/iocage exec steamjailer pkg install -y wine-proton winetricks");
}
