# Project: Steam Jailer

An ambitious multipart part project uses iocache and custom c++ and shell to provide this service

Installs iocache configures and loads your jail, installs wine-proton, winetricks in a script that is commented into the jail, installs modules, and eventualy Steam

# Warning
This project is very invasive, it not only installs wine-proton, but also a jail manager, and eventually Steam

# Intention
This projects purpose is to automate the installation of Steam within a jail on FreeBSD, including the installation of wine-proton both system-wide and within the jail. The project will also provide options to install necessary patches and enable ezjail in the system's rc.conf. Finally, it will offer the ability to link the launcher to the user's $HOME/bin folder with a default program icon for FreeBSD. The project `steamjailer` provides an interface allowing control of Steam with-in the jail

It is beyond the scope of this project to provide a wine-proton capable of playing modern games, the current wine-proton in ports and packages is capable of playing SkyrimSE, Dark SoulsIII, Fallout4, etc. This project will grow depending on base package support

The `steam_installer.sh` is almost fully functioning, I have yet to test `jailer`'s functions with it, this little shell script was quite the undertaking, it install's ezjail as per the FreeBSD Foundation,

    zroot/jails            96K  1.44T    96K  /zroot/jails
    
sets up the zfs stuff required, identifies as shell based software, will identify what your install is on, restarts services devfs, following the successful installion of ezjail and the setup of the jail the project then takes control of your computer, don't be concerned, project will return control momentarily. The first jail setup seems to take a while, subsequent installs are quick on this nvme drive I keep my system updated and the script is setup to installworld into the jail, 
if you require different usage, alter to suit 

The project then installs wine-proton winetricks and modules via a commented shell script where you can add or alter what the script installs in the jail

This is the jail created on my computer

    ronf@bsdbox:~/git/jailer/v1 $ sudo ezjail-admin list
    STA JID  IP              Hostname                       Root Directory
    --- ---- --------------- ------------------------------ ------------------------
    DS  N/A  127.0.1.1       steamjailer_1739358384         /usr/jails/steamjailer_1739358384
    DSN N/A  -                                              



    # SteamJailer
     Build steam_jailer: c++ steam_launcher.cpp -o steam_jailer then ./steam_jailer
    
This is how ezjail is set in my /etc/rc.conf you need to enter this and then re-start services

# Tip

If this project has difficulty if your zpool already exists, then enter

    sudo zpool import -o readonly=off zroot

This command will import the existing pool    

# Project Structure

Shell Script: To install and configure ezjail, create and start the jail (with the capability to create a new jail if one already exists), and install necessary packages inside the jail. 

C++ Program: To manage the installation and launching of Steam and games within the jail.

Step 1: Shell Script

Step 2: C++ Program

Public code references from 4 repositories


# Troubleshooting

If you encounter issues, ensure that:

    The ZFS pool is online and healthy.
    The ezjail configuration is set up correctly.
    The /etc/devfs.rules file is configured to allow jail access to /dev/zfs.

Refer to the log messages in the script for additional information and debugging steps.
Run the Jailer: This will allow you to install items the other scipt may have missed, launch Steam, launch games, and link the launcher to $HOME/bin

sh

    ./steam_jailer

By following these steps, you can set up a Steam game launcher on FreeBSD using a jail, ensuring that all necessary libraries and dependencies are available for the games, if anything is missing simply add it with wintricks in the script.
