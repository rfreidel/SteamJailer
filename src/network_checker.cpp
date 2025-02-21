#include "network_checker.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <errno.h>
#include <string.h>

NetworkChecker::NetworkChecker() : m_localIP("127.0.0.1") {
    // Initialize with loopback address, will be updated in getLocalIP()
}

NetworkChecker::~NetworkChecker() {
    // Cleanup if needed
}

bool NetworkChecker::checkInternetConnection() {
    // Test connection to a reliable external host (e.g., cloudflare DNS)
    return testConnection("1.1.1.1");
}

bool NetworkChecker::checkLocalhostConnection() {
    return testConnection("127.0.0.1");
}

bool NetworkChecker::testConnection(const std::string& host) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return false;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80); // Test HTTP port

    struct timeval timeout;
    timeout.tv_sec = 3;  // 3 seconds timeout
    timeout.tv_usec = 0;

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        close(sock);
        return false;
    }

    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
        close(sock);
        return false;
    }

    int result = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    close(sock);
    
    return result == 0;
}

std::string NetworkChecker::getLocalIP() const {
    struct ifaddrs *ifaddr, *ifa;
    std::string ip;

    if (getifaddrs(&ifaddr) == -1) {
        return "127.0.0.1";
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;

        // Skip loopback and non-IPv4 addresses
        if (ifa->ifa_addr->sa_family != AF_INET || 
            (ifa->ifa_flags & IFF_LOOPBACK) || 
            !(ifa->ifa_flags & IFF_UP)) {
            continue;
        }

        struct sockaddr_in *addr = (struct sockaddr_in*)ifa->ifa_addr;
        ip = inet_ntoa(addr->sin_addr);
        if (!ip.empty() && ip != "127.0.0.1") {
            freeifaddrs(ifaddr);
            return ip;
        }
    }

    freeifaddrs(ifaddr);
    return "127.0.0.1";
}