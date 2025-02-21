#pragma once
#include <string>
#include <vector>
#include <memory>

class ZenityGUI {
public:
    ZenityGUI();
    ~ZenityGUI() = default;

    // Dialog functions
    bool showQuestion(const std::string& message);
    void showInfo(const std::string& message);
    void showError(const std::string& message);
    std::string getInput(const std::string& prompt);
    std::string getPassword(const std::string& prompt);
    std::string selectFile(const std::string& title);
    
    // Progress functions
    bool showProgress(const std::string& title, const std::string& text);
    void updateProgress(int percentage);
    void closeProgress();

private:
    std::string executeZenity(const std::vector<std::string>& args);
    FILE* m_progressPipe;
};