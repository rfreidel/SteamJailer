#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

// FreeBSD POSIX command paths
const char* CMD_DATE = "/bin/date";
const char* CMD_WHOAMI = "/usr/bin/whoami";
const char* CMD_UNAME = "/usr/bin/uname";
const char* CMD_SED = "/usr/bin/sed";\
const char* CMD_ECHO = "/bin/echo";

int main() {
    // Get FreeBSD version without -p1/-P1 suffix
    std::string cmd = std::string(CMD_UNAME) + " -r | " + 
                     CMD_SED + " -e 's/-[pP][0-9]$//'";
    
    FILE* fp = popen(cmd.c_str(), "r");
    if (!fp) {
        std::cerr << "Error executing command\n";
        return 1;
    }

    char version[256];
    if (fgets(version, sizeof(version), fp) == nullptr) {
        std::cerr << "Error reading command output\n";
        pclose(fp);
        return 1;
    }
    pclose(fp);

    // Remove newline
    std::string fbsd_version(version);
    if (!fbsd_version.empty() && fbsd_version[fbsd_version.length()-1] == '\n') {
        fbsd_version.erase(fbsd_version.length()-1);
    }

    // Create packages.json with required configuration
    std::ofstream json("packages.json");
    if (!json) {
        std::cerr << "Error: Cannot create packages.json\n";
        return 1;
    }

    json << "{\n"
         << "    \"repository\": \"https://pkg.freebsd.org/FreeBSD:14:amd64/latest\",\n"
         << "    \"pkgs\": [\n"
         << "        \"wine-proton\",\n"
         << "        \"winetricks\",\n"
         << "        \"ca_root_nss\",\n"
         << "        \"liberation-fonts-ttf\",\n"
         << "        \"wine-gecko\",\n"
         << "        \"wine-mono\"\n"
         << "    ],\n"
         << "    \"winetricks\": [\n"
         << "        \"d3dx9\",\n"
         << "        \"d3dx10\",\n"
         << "        \"d3dx11\",\n"
         << "        \"vcrun2019\",\n"
         << "        \"dotnet48\",\n"
         << "        \"corefonts\",\n"
         << "        \"dxvk\",\n"
         << "        \"nvapi\"\n"
         << "    ],\n"
         << "    \"env\": {\n"
         << "        \"WINE\": \"/usr/local/wine-proton/bin/wine\",\n"
         << "        \"WINEPREFIX\": \"${HOME}/.wine\",\n"
         << "        \"WINEARCH\": \"win64\"\n"
         << "    }\n"
         << "}\n";

    json.close();
    std::cout << "Created packages.json configuration file\n";
    return 0;
}