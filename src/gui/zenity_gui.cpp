#include "gui/zenity_gui.hpp"
#include "config.hpp"
#include "logger.hpp"
#include <array>
#include <cstdio>
#include <stdexcept>
#include <sstream>

ZenityGUI::ZenityGUI() : m_progressPipe(nullptr) {}

std::string ZenityGUI::executeZenity(const std::vector<std::string>& args) {
    std::string cmd = ZENITY_PATH;
    for (const auto& arg : args) {
        cmd += " " + arg;
    }

    std::array<char, 128> buffer;
    std::string result;
    
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        LOG_ERROR("Failed to execute zenity command: " + cmd);
        throw std::runtime_error("Failed to execute zenity command");
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    
    pclose(pipe);
    return result;
}

bool ZenityGUI::showQuestion(const std::string& message) {
    try {
        std::vector<std::string> args = {
            "--question",
            "--text=" + message,
            "--width=300"
        };
        executeZenity(args);
        return true;
    } catch (...) {
        return false;
    }
}

void ZenityGUI::showInfo(const std::string& message) {
    std::vector<std::string> args = {
        "--info",
        "--text=" + message,
        "--width=300"
    };
    executeZenity(args);
}

void ZenityGUI::showError(const std::string& message) {
    std::vector<std::string> args = {
        "--error",
        "--text=" + message,
        "--width=300"
    };
    executeZenity(args);
}

std::string ZenityGUI::getInput(const std::string& prompt) {
    std::vector<std::string> args = {
        "--entry",
        "--text=" + prompt,
        "--width=300"
    };
    return executeZenity(args);
}

std::string ZenityGUI::getPassword(const std::string& prompt) {
    std::vector<std::string> args = {
        "--password",
        "--text=" + prompt,
        "--width=300"
    };
    return executeZenity(args);
}

std::string ZenityGUI::selectFile(const std::string& title) {
    std::vector<std::string> args = {
        "--file-selection",
        "--title=" + title,
        "--width=600"
    };
    return executeZenity(args);
}

bool ZenityGUI::showProgress(const std::string& title, const std::string& text) {
    std::string cmd = std::string(ZENITY_PATH) + 
                     " --progress" +
                     " --title=\"" + title + "\"" +
                     " --text=\"" + text + "\"" +
                     " --auto-close" +
                     " --width=300";
    
    m_progressPipe = popen(cmd.c_str(), "w");
    return m_progressPipe != nullptr;
}

void ZenityGUI::updateProgress(int percentage) {
    if (m_progressPipe) {
        fprintf(m_progressPipe, "%d\n", percentage);
        fflush(m_progressPipe);
    }
}

void ZenityGUI::closeProgress() {
    if (m_progressPipe) {
        pclose(m_progressPipe);
        m_progressPipe = nullptr;
    }
}