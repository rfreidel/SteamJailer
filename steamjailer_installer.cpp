#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <regex>
#include <algorithm>

// Function to execute a shell command and exit if it fails
void runCommand(const std::string &command) {
    std::cout << "Running command: " << command << std::endl;
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Command failed: " << command << std::endl;
        std::exit(result); // Exit on command failure to prevent further execution
    }
}

// Function to get available FreeBSD releases using iocage fetch
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

// Function to get the primary network interface (excluding loopback)
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

// Function to get the IP address (IPv4 or IPv6) for a given network interface
std::string getIPAddress(const std::string& interface) {
    std::string ipAddress;

    // Try to get IPv4 address
    FILE* pipe4 = popen(("ifconfig " + interface + " | grep 'inet ' | awk '{print $2}'").c_str(), "r");
    if (pipe4) {
        char buffer4[128];
        if (fgets(buffer4, sizeof(buffer4), pipe4) != nullptr) {
            ipAddress = buffer4;
            ipAddress.erase(std::remove(ipAddress.begin(), ipAddress.end(), '\n'), ipAddress.end());
            std::regex ipv4_regex(R"(\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b)");
            if(std::regex_match(ipAddress, ipv4_regex)) {
                pclose(pipe4);
                return ipAddress;
            } else {
                 ipAddress = "";
            }
        }
        pclose(pipe4);
    }

    // If IPv4 fails, try to get IPv6 address
    FILE* pipe6 = popen(("ifconfig " + interface + " | grep 'inet6 ' | awk '{print $2}'").c_str(), "r");
    if (pipe6) {
        char buffer6[128];
        while (fgets(buffer6, sizeof(buffer6), pipe6) != nullptr) {
            std::string temp_ip = buffer6;
            temp_ip.erase(std::remove(temp_ip.begin(), temp_ip.end(), '\n'), temp_ip.end());
            if (temp_ip.find("fe80") != 0) { // ignore link-local addresses
                ipAddress = temp_ip;
                break;
            }
        }
        pclose(pipe6);
    }

    return ipAddress.empty() ? "127.0.1.1" : ipAddress;
}


// Function to install iocage if it's not already installed
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

// Function to activate iocage on the primary zpool
void activateIocage() {
    // Activate iocage on the primary zpool
    runCommand("sudo iocage activate zroot");
}

// Function to set up the steamjailer jail
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

        // Determine the IP addressing scheme (IPv4 or IPv6)
        std::string ipScheme = "ip4_addr";
        std::regex ipv6_regex(R"(([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4})");
        if (std::regex_match(ipAddress, ipv6_regex)) {
            ipScheme = "ip6_addr";
        }

        // Create the jail with the latest available release
        runCommand("sudo iocage create -r " + latestRelease + " -n steamjailer " + ipScheme + "=\"" + networkInterface + "|" + ipAddress + "/64\" allow_raw_sockets=\"1\"");
    }

    // Start the jail
    runCommand("sudo iocage start steamjailer");

    // Mount necessary filesystems
    runCommand("sudo iocage fstab -a steamjailer devfs /dev devfs rw 0 0");
    runCommand("sudo iocage fstab -a steamjailer procfs /proc procfs rw 0 0");
}

// Function to install necessary packages inside the jail
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