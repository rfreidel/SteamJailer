#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>

void runCommand(const std::string &command) {
    std::cout << "Running command: " << command << std::endl;
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Command failed: " << command << std::endl;
        std::exit(result); // Exit on command failure to prevent further execution
    }
}

std::vector<std::string> getAvailableReleases() {
    std::vector<std::string> releases;
    FILE* pipe = popen("iocage fetch -r | grep -Eo '^[0-9]+\\.[0-9]+-RELEASE'", "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    char buffer[128];
    while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
        std::stringstream ss(buffer);
        std::string release;
        ss >> release;
        releases.push_back(release);
    }
    pclose(pipe);
    return releases;
}

std::string getNetworkInterface() {
    FILE* pipe = popen("ifconfig | grep 'flags=' | awk '{print $1}' | tr -d ':'", "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    char buffer[128];
    std::vector<std::string> interfaces;
    while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
        std::stringstream ss(buffer);
        std::string iface;
        ss >> iface;
        interfaces.push_back(iface);
    }
    pclose(pipe);

    for (const auto& iface : interfaces) {
        if (iface.find("lo") == std::string::npos) {
            return iface;
        }
    }
    return "lo0"; // default to loopback if no other interface found
}

std::string getIPAddress(const std::string& interface) {
    FILE* pipe = popen(("ifconfig " + interface + " | grep 'inet6' | awk '{print $2}'").c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    char buffer[128];
    std::string ip;
    while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
        std::stringstream ss(buffer);
        ss >> ip;
        if (ip.find("fe80") != 0) { // ignore link-local addresses
            break;
        }
    }
    pclose(pipe);
    return ip.empty() ? "127.0.1.1" : ip;
}

void installIocage() {
    // Check if iocage is installed
    if (std::system("command -v iocage >/dev/null 2>&1") != 0) {
        // Install iocage
        std::cout << "iocage is not installed. Installing iocage..." << std::endl;
        runCommand("sudo pkg install -y py311-iocage");
    } else {
        std::cout << "iocage is already installed." << std::endl;
    }
}

void activateIocage() {
    // Activate iocage on the primary zpool
    runCommand("sudo iocage activate zroot");
}

void setupJail() {
    // Check if the jail already exists
    if (std::system("sudo iocage get state steamjailer >/dev/null 2>&1") == 0) {
        std::cout << "Jail steamjailer already exists." << std::endl;
    } else {
        // Get available releases
        std::vector<std::string> releases = getAvailableReleases();
        if (releases.empty()) {
            std::cerr << "No available releases found!" << std::endl;
            std::exit(1);
        }

        // Use the latest available release
        std::string latestRelease = releases.back();
        std::cout << "Using release: " << latestRelease << std::endl;

        // Get network interface and IP address
        std::string networkInterface = getNetworkInterface();
        std::string ipAddress = getIPAddress(networkInterface);

        // Create the jail with the latest available release
        runCommand("sudo iocage create -r " + latestRelease + " -n steamjailer ip6_addr=\"" + networkInterface + "|" + ipAddress + "/64\" allow_raw_sockets=\"1\"");
    }

    // Start the jail
    runCommand("sudo iocage start steamjailer");

    // Mount necessary filesystems
    runCommand("sudo iocage fstab -a steamjailer devfs /dev devfs rw 0 0");
    runCommand("sudo iocage fstab -a steamjailer procfs /proc procfs rw 0 0");
}

void installPackages() {
    // Commands to install necessary packages
    runCommand("sudo iocage exec steamjailer pkg install -y wine-proton winetricks");
}

int main() {
    std::cout << "Initializing jailer setup..." << std::endl;

    // Install iocage if not already installed
    installIocage();

    // Activate iocage
    activateIocage();

    // Setup jail
    setupJail();

    // Install packages
    installPackages();

    std::cout << "Jailer setup completed successfully." << std::endl;
    return 0;
}