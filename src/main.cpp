#include <wx/wx.h>
#include "gui/main_window.hpp"
#include "jail_manager.hpp"
#include "network_checker.hpp"
#include "wine_installer.hpp"
#include <memory>
#include <iostream>

class JailAutomationApp : public wxApp {
public:
    virtual bool OnInit() {
        if (!wxApp::OnInit())
            return false;

        // Check for root privileges
        if (getuid() != 0) {
            wxMessageBox("This application must be run as root", "Error",
                        wxOK | wxICON_ERROR);
            return false;
        }

        auto netChecker = std::make_unique<NetworkChecker>();
        if (!netChecker->checkInternetConnection()) {
            wxMessageBox("No internet connection detected", "Error",
                        wxOK | wxICON_ERROR);
            return false;
        }

        auto jailManager = std::make_unique<JailManager>();
        auto wineInstaller = std::make_unique<WineInstaller>(jailManager->getJailPath());

        MainWindow* frame = new MainWindow("FreeBSD Jail Automation",
                                         std::move(jailManager),
                                         std::move(wineInstaller));
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(JailAutomationApp);