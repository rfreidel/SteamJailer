#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

void run_command(const std::string &command) {
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Command failed: " << command << std::endl;
        std::string error_message = "zenity --error --text=\"Command failed: " + command + "\"";
        std::system(error_message.c_str());
        exit(result);
    }
}

void show_message(const std::string &message) {
    std::string command = "zenity --info --text=\"" + message + "\"";
    run_command(command);
}

std::string show_file_selection_dialog() {
    std::string command = "zenity --file-selection --title=\"Select a Windows Game Exe\"";
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
    setenv("WINE-PROTON", "/usr/local/wine-proton/bin/wine", 1);
    setenv("WINEARCH", "win64", 1);
}

void install_wine_proton() {
    show_message("Installing Wine-Proton in Jail...");
    run_command("sudo jexec pkg install -y wine-proton winetricks");
    show_message("Installation completed. Wine-Proton is installed in the jail.");
}

void install_steam() {
    show_message("Installing Steam in Jail...");
    run_command("sudo jexec winetricks steam");
    show_message("Steam installation completed successfully.");
}

void launch_game(const std::string &game_path) {
    std::string command = "sudo jexec wine steam  \"" + game_path + "\"";
    show_message("Launching game: " + game_path);
    run_command(command);
}

void launch_steam() {
    show_message("Launching Steam in Jail...");
    run_command("sudo jexec wine ~/.wine/drive_c/Program\\ Files\\ \\(x86\\)/Steam/Steam.exe");
}

void link_to_home_bin() {
    std::string home = getenv("HOME");
    std::string command = "ln -s " + home + "/steam_launcher " + home + "/bin/steam_launcher";
    run_command(command);
    show_message("Steam Launcher linked to $HOME/bin.");
}

int main() {
    set_wine_environment();

    std::string choice = "zenity --list --title=\"Steam Jailer\" --column=\"Task\" Install_Wine_Proton Install_Steam Launch_Game Launch_Steam Link_to_Home_Bin";
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
