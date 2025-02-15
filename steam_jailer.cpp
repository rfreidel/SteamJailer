#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>  // For exceptions
#include <cstdio>     // For FILE* and related functions
#include <memory>    // For smart pointers

// Function to run a command and check for errors
void runCommand(const std::string &command) {
    std::cout << "Running command: " << command << std::endl;
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Command failed: " << command << ", exit code: " << result << std::endl;
        throw std::runtime_error("Command failed: " + command + ", exit code: " + std::to_string(result)); // Throw exception
    }
}

// Function to execute a command and return its output as a vector of strings
std::vector<std::string> executeAndGetOutput(const std::string& command) {
    std::vector<std::string> outputLines;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose); // Use unique_ptr for RAII
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    char buffer[128];
    while (fgets(buffer, sizeof buffer, pipe.get()) != nullptr) {
        outputLines.emplace_back(buffer); // Add line to vector
        outputLines.back().erase(outputLines.back().find_last_not_of(" \n\r\t") + 1); // Trim whitespace
    }
    return outputLines;
}

// Function to get available releases
std::vector<std::string> getAvailableReleases() {
    std::vector<std::string> releases;
    std::vector<std::string> output = executeAndGetOutput("iocage fetch -r");
    for (const auto& line : output) {
        std::smatch match;
        if (std::regex_search(line, match, std::regex("^[0-9]+\\.[0-9]+-RELEASE"))) {
            releases.push_back(match.str());
        }
    }
    return releases;
}

// Function to get the network interface (excluding loopback)
std::string getNetworkInterface() {
    std::vector<std::string> interfaces;
    std::vector<std::string> output = executeAndGetOutput("ifconfig | grep 'flags=' | awk '{print $1}' | tr -d ':'");

    for (const auto& iface : output) {
        if (iface.find("lo") == std::string::npos) {
            return iface;
        }
    }
    return "lo0"; // default to loopback if no other interface found
}

// Function to get the IP address of a given interface
std::string getIPAddress(const std::string& interface) {
    std::vector<std::string> output = executeAndGetOutput("ifconfig " + interface + " | grep 'inet6' | awk '{print $2}'");
    for (const auto& ip : output) {
        if (ip.find("fe80") != 0) { // ignore link-local addresses
            return ip;
        }
    }
    return "127.0.1.1";
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
    runCommand("sudo iocage activate zroot");
}

// Function to set up the jail
void setupJail() {
    // Check if the jail already exists
    try {
        runCommand("sudo iocage get state steamjailer >/dev/null 2>&1");
        std::cout << "Jail steamjailer already exists." << std::endl;
    } catch (const std::runtime_error& e) {
        // Jail does not exist, proceed with creation
        std::cout << "Jail steamjailer does not exist. Creating..." << std::endl;

        // Get available releases
        std::vector<std::string> releases = getAvailableReleases();
        if (releases.empty()) {
            throw std::runtime_error("No available releases found!");
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

// Function to install necessary packages inside the jail
void installPackages() {
    // Commands to install necessary packages
    runCommand("sudo iocage exec steamjailer pkg install -y wine-proton winetricks");
}

int main() {
    try {
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

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}