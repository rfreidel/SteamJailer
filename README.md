# Project: Steam Jailer

This projects purpose is to automate the installation of Steam within a jail on FreeBSD, including the installation of wine-proton both system-wide and within the jail. The project will also provide options to install necessary patches and enable ezjail in the system's rc.conf. Finally, it will offer the ability to link the launcher to the user's $HOME/bin folder with a default program icon for FreeBSD.

It is beyond the scope of this project to provide a wine-proton capable of playing modern games, the current wine-proton is capable of playing SkyrimSE, Dark SoulsIII, etc, this project will grow depending on base package support

# Project Structure

Shell Script: To install and configure ezjail, create and start the jail (with the capability to create a new jail if one already exists), and install necessary packages inside the jail. 

C++ Program: To manage the installation and launching of Steam and games within the jail.

Step 1: Shell Script \
Step 2: C++ Program
Public code references from 4 repositories

# Usage Instructions

Run the Setup Script: This will install ezjail, create and start the jail, and install necessary packages inside the jail. If a jail with the same name already exists, it will create a new jail with a unique name.
    
    chmod +x steam_jailer_setup.sh
    ./steam_jailer_setup.sh

Compile the C++ Program: This will compile the steam_launcher.cpp program.
sh

    g++ steam_launcher.cpp -o steam_jailer

Run the Launcher: This will allow you to install Wine-Proton, Steam, launch games, and link the launcher to $HOME/bin.
sh

    ./steam_jailer

By following these steps, you can set up a Steam game launcher on FreeBSD using a jail, ensuring that all necessary libraries and dependencies are available for the games.
