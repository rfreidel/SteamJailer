# Project: Steam Jailer

SUBNOTE: This pile of "code" may not function, yet the launcher is based on a personal game launcher I build for my own use, to release it to the public is quite an undertaking, so if you are familiar with c++ please look at the code, submit patches, test, then following my next moment of inspiration, I'll update it again

Currently I am going through all the needed steps to make the project function, thus far I have had to fill in manually the contents o ezjail conf, and I had forgotten about having ports set up for the jail, I have completed these steps, re compile/start program

This projects purpose is to automate the installation of Steam within a jail on FreeBSD, including the installation of wine-proton both system-wide and within the jail. The project will also provide options to install necessary patches and enable ezjail in the system's rc.conf. Finally, it will offer the ability to link the launcher to the user's $HOME/bin folder with a default program icon for FreeBSD. My script for installing jails is currently broken, I had to take a step away rom it for a bit, yet I believe the c++ code while it may be solid does not properly function at this state , well mostly due to the jail issue

It is beyond the scope of this project to provide a wine-proton capable of playing modern games, the current wine-proton is capable of playing SkyrimSE, Dark SoulsIII, etc, this project will grow depending on base package support

# Project Structure

Shell Script: To install and configure ezjail, create and start the jail (with the capability to create a new jail if one already exists), and install necessary packages inside the jail. 

C++ Program: To manage the installation and launching of Steam and games within the jail.

Step 1: Shell Script \
Step 2: C++ Program
Public code references from 4 repositories

# Installation

    chmod +x  install_packges.sh 
    
    ./install_packages.sh
    


# Usage Instructions

Run the Setup Script: This will install ezjail, create and start the jail, and install necessary packages inside the jail. If a jail with the same name already exists, it will create a new jail with a unique name. (note: I may change the jail config, something about seems incorrect), t this point in time it does sort of setup the jail correctly, by later this evening I will update the script for installing wine-proton, I need to test things first, Skyrim here I come
    
    chmod +x steam_jailer_setup.sh
    ./steam_jailer_setup.sh

Compile the C++ Program: This will compile the steam_launcher.cpp program.
sh

    c++ steam_launcher.cpp -o steam_jailer

Run the Launcher: This will allow you to install Wine-Proton, Steam, launch games, and link the launcher to $HOME/bin.
sh

    ./steam_jailer

By following these steps, you can set up a Steam game launcher on FreeBSD using a jail, ensuring that all necessary libraries and dependencies are available for the games.
