#include <iostream>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <cstdio>

void run_command(const std::string &command) {
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Command failed: " << command << std::endl;
        std::string error_message = "zenity --error --text=\"Command failed: " + command + "\"";
        std::system(error_message.c_str());
        std::exit(result);
    }
}

void show_message(const std::string &message) {
    std::string command = "zenity --info --text=\"" + message + "\"";
    run_command(command);
}

std::string show_file_selection_dialog() {
    std::string command = "zenity --file-selection --title=\"Select a Windows Game Executable\"";
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    result.erase(result.find_last_not_of(" \n\r\t") + 1); // Trim trailing whitespace
    return result;
}

void set_wine_environment() {
    setenv("WINE", "/usr/local/wine-proton/bin/wine", 1);
    setenv("WINEPROTON", "/usr/local/wine-proton/bin/wine", 1);
    setenv("WINEPREFIX", "~/.steam-games", 1);
    setenv("WINEARCH", "win64", 1);
}

void install_wine_proton() {
    show_message("Installing Wine-Proton in Jail...");
    run_command("sudo iocage exec steamjailer pkg install -y wine-proton winetricks");
    run_command("sudo iocage exec steamjailer /usr/local/wine-proton/bin/pkg32.sh install wine mesa-dri");
    run_command("sudo iocage exec steamjailer WINE=/usr/local/wine-proton/bin/wine winetricks --force corefonts dxvk nvapi vkd3d");

    show_message("Installation completed. Wine-Proton is installed in the jail.");
}

void install_steam() {
    show_message("Installing Steam in Jail...");
    run_command("sudo fetch -o /tmp/SteamSetup.exe https://cdn.akamai.steamstatic.com/client/installer/SteamSetup.exe");
    run_command("sudo iocage exec steamjailer WINE=/usr/local/wine-proton/bin/wine /tmp/SteamSetup.exe");
    show_message("Steam installation completed.");
}

void install_dxvk() {
    show_message("Installing DXVK and related components in Jail...");
    run_command("sudo iocage exec steamjailer WINE=/usr/local/wine-proton/bin/wine winetricks --force corefonts dxvk nvapi vkd3d");
    show_message("DXVK & corefonts installation completed successfully.");
}

void launch_game(const std::string &game_path) {
    std::string command = "sudo iocage exec steamjailer WINE=/usr/local/wine-proton/bin/wine \"" + game_path + "\"";
    show_message("Launching game: " + game_path);
    run_command(command);
}

void launch_steam() {
    show_message("Launching Steam in Jail...");
    run_command("sudo iocage exec steamjailer WINE=/usr/local/wine-proton/bin/wine ~/.steam-games/drive_c/Program\\ Files\\ \\(x86\\)/Steam/Steam.exe -cef-disable-sandbox -cef-disable-gpu-compositing -cef-in-process-gpu");
}

void link_to_home_bin() {
    std::string home = getenv("HOME");
    std::string command = "ln -s " + home + "/steam_launcher " + home + "/bin/steam_launcher";
    run_command(command);
    show_message("Steam Launcher linked to $HOME/bin.");
}

int main() {
    set_wine_environment();

    std::string choice = "zenity --list --title=\"Steam Game Launcher\" --column=\"Task\" Install_Wine_Proton Install_Steam Install_dxvk Launch_Game Launch_Steam Link_to_Home_Bin";
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(choice.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    result.erase(result.find_last_not_of(" \n\r\t") + 1); // Trim trailing whitespace

    if (result == "Install_Wine_Proton") {
        install_wine_proton();
    } else if (result == "Install_Steam") {
        install_steam();
    } else if (result == "Install_dxvk") {
        install_dxvk();
    } else if (result == "Launch_Game") {
        std::string game_path = show_file_selection_dialog();
        if (!game_path.empty()) {
            launch_game(game_path);
        } else {
            show_message("No game selected. Exiting.");
        }
    } else if (result == "Launch_Steam") {
        launch_steam();
    } else if (result == "Link_to_Home_Bin") {
        link_to_home_bin();
    } else {
        show_message("No valid option selected. Exiting.");
    }

    return 0;
}
