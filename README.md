# Project: Steam Jailer

An ambitious multipart project uses iocache and custom c++ and shell to provide this function

Installs iocache configures and loads your jail, installs wine-proton, winetricks in a script that is commented, installs modules, and eventualy Steam using Posix compliant syntax I think, into the jail

# Warning
This project is very invasive, it not only installs a jail, jail manager, wine-proton, but also and eventually Steam

# Intention
This projects intended purpose is to automate the installation of Steam within a jail on FreeBSD, including the installation of wine-proton both system-wide and within the jail. The project will also provide options to install necessary patches and enable iocage in the system's rc.conf. Finally, it will eventually offer the ability to link the launcher to the user's $HOME/bin folder with a default program icon for FreeBSD, I am mostly concerned at this time for posix compliance, stability, and function in the main part of project. The project `steamjailer` provides an interface allowing control of Steam with-in the jail using zenity

It is beyond the scope of this project to provide a wine-proton capable of playing modern games, the current wine-proton in ports and packages is capable of playing SkyrimSE, Dark SoulsIII, Fallout4, etc. This project will grow depending on base package support

# TL;DR: Using iocage to Interface with Jail SteamJailer

## What is iocage?
iocage is a FreeBSD jail manager that simplifies the creation, management, and maintenance of jails. It provides a command-line interface for managing jails and supports various features like jail templates, resource limits, and networking configuration.

## What is SteamJailer?
SteamJailer is a specific jail configuration designed to run Steam within a FreeBSD jail. It ensures that Steam operates in a contained environment, isolating it from the rest of the system for security and management purposes.

## Example Steps to Interface with SteamJailer using iocage

### 1. Install iocage
```sh
pkg install iocage

2. Create a New Jail
sh

iocage create -n steamjailer -r 14.2-RELEASE

    -n steamjailer: Specifies the name of the jail.
    -r 14.2-RELEASE: Specifies the FreeBSD release version.

3. Set Jail Configuration

Configure the jail to meet the requirements of SteamJailer:
sh

iocage set boot=on steamjailer
iocage set allow_raw_sockets=1 steamjailer
iocage set allow_mount=1 steamjailer
iocage set allow_mount_devfs=1 steamjailer

4. Start the Jail
sh

iocage start steamjailer

5. Access the Jail
sh

iocage console steamjailer

6. Install Steam Inside the Jail

Once inside the jail, install Steam and any dependencies:
sh

pkg install steam

7. Configure Networking

Ensure the jail has access to the internet and Steam servers. This may involve setting up NAT or other network configurations on the host system.
8. Run Steam

Launch Steam from within the jail and proceed with the normal Steam setup and usage.
Useful Commands

    List Jails: iocage list
    Stop Jail: iocage stop steamjailer
    Restart Jail: iocage restart steamjailer
    Update Jail: iocage update steamjailer

Notes

    Ensure the host system has the necessary resources to run Steam within a jail.
    Regularly update both the FreeBSD system and the jail to maintain security and compatibility.
    Consult the iocage documentation for advanced configurations and troubleshooting.

For more detailed information, refer to the official iocage and FreeBSD documentation.
