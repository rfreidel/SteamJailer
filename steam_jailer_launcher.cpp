#include <iostream>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <cstdio>
#include <memory>
#include <vector>
#include <sstream>
#include <regex> // Required for regex

// Function to run a command and check for errors
void run_command(const std::string &command) {
    std::cout << "Running command: " << command << std::endl;
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Command failed: " << command << ", exit code: " << result << std::endl;
        std::string error_message = "zenity --error --text=\"Command failed: " + command + ", exit code: " + std::to_string(result) + "\"";
        std::system(error_message.c_str());
        throw std::runtime_error("Command failed: " + command + ", exit code: " + std::to_string(result));
    }
}

// Function to display a message using zenity
void show_message(const std::string &message) {
    std::string command = "zenity --info --text=\"" + message + "\"";
    run_command(command);
}

// Function to show a file selection dialog using zenity
std::string show_file_selection_dialog() {
    std::string command = "zenity --file-selection --title=\"Select a Windows Game Executable\"";
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof buffer, pipe.get()) != nullptr) {
        result += buffer;
    }
    result.erase(result.find_last_not_of(" \n\r\t") + 1);
    return result;
}

// Function to set Wine environment variables
void set_wine_environment() {
    // Using setenv is unsafe, it can overwrite important environment variables.
    // It's better to pass the environment variables directly to the iocage exec command.
    // However, if you still want to use setenv, consider checking if the variable is already set.
    if (setenv("WINE", "/usr/local/wine-proton/bin/wine", 1) != 0) {
        perror("setenv WINE failed");
    }
    if (setenv("WINEPREFIX", "/home/steam/.steam-games", 1) != 0) {
        perror("setenv WINEPREFIX failed");
    }
    if (setenv("WINEARCH", "win64", 1) != 0) {
        perror("setenv WINEARCH failed");
    }
}

// Function to install Wine-Proton in the jail
void install_wine_proton() {
    show_message("Installing Wine-Proton in Jail...");
    run_command("sudo iocage exec steamjailer pkg install -y wine-proton winetricks");
    run_command("sudo iocage exec steamjailer /usr/local/wine-proton/bin/pkg32.sh install wine mesa-dri");
    run_command("sudo iocage exec steamjailer WINE=/usr/local/wine-proton/bin/wine winetricks --force corefonts dxvk nvapi vkd3d");

    show_message("Installation completed. Wine-Proton is installed in the jail.");
}

// Function to install Steam in the jail
void install_steam() {
    show_message("Installing Steam in Jail...");
    run_command("sudo fetch -o /tmp/SteamSetup.exe https://cdn.fastly.steamstatic.com/client/installer/SteamSetup.exe");
    run_command("sudo iocage exec steamjailer WINE=/usr/local/wine-proton/bin/wine /tmp/SteamSetup.exe");
    show_message("Steam installation completed.");
}

// Function to install DXVK and related components
void install_dxvk() {
    show_message("Installing DXVK and related components in Jail...");
    run_command("sudo iocage exec steamjailer WINE=/usr/local/wine-proton/bin/wine winetricks --force corefonts dxvk nvapi vkd3d");
    show_message("DXVK & corefonts installation completed successfully.");
}

// Function to launch a game
void launch_game(const std::string &game_path) {
    std::string command = "sudo iocage exec steamjailer WINE=/usr/local/wine-proton/bin/wine \"" + game_path + "\"";
    show_message("Launching game: " + game_path);
    run_command(command);
}

// Function to launch Steam
void launch_steam() {
    show_message("Launching Steam in Jail...");
    // Escape the spaces in "Program Files (x86)" for the shell
    std::string command = "sudo iocage exec steamjailer WINE=/usr/local/wine-proton/bin/wine ~/.steam-games/drive_c/Program\\ Files\\ \\(x86\\)/Steam/Steam.exe -cef-disable-sandbox -cef-disable-gpu-compositing -cef-in-process-gpu";
    run_command(command);
}

// Function to create a symbolic link to the launcher in $HOME/bin
void link_to_home_bin() {
    std::string home = getenv("HOME");
    if (home.empty()) {
        show_message("HOME environment variable not set.");
        return;
    }

    std::string command = "ln -s " + home + "/steam_launcher " + home + "/bin/steam_launcher";
    run_command(command);
    show_message("Steam Launcher linked to $HOME/bin.");
}

int main() {
    try {
        // set_wine_environment(); // Consider passing env vars directly to iocage exec

        std::string choice_command = "zenity --list --title=\"Steam Game Launcher\" --column=\"Task\" Install_Wine_Proton Install_Steam Install_dxvk Launch_Game Launch_Steam Link_to_Home_Bin";
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(choice_command.c_str(), "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }

        char buffer[128];
        std::string result;
        while (fgets(buffer, sizeof buffer, pipe.get()) != nullptr) {
            result += buffer;
        }
        result.erase(result.find_last_not_of(" \n\r\t") + 1);

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

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
