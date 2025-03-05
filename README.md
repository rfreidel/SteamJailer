# Project: Steam Jailer
(devlog, sorta)
## This project is under development, at this point the direction is clear, just fixing bugs
(now I understand why this has not been done before)

# Project purpose

To provide an easy way to install a jail, wine-proton and Steam into the jail, and control jailed steam, I tested on 14.2-RELEASE-p2 and partially tested with 15-CURRENT

In its current state the project is s c++/shell script project which upon successful stability and function I will turn into an installable package/port

(update 02/20/2025)

I asked Claude 3.5 to print project build instructions https://github.com/rfreidel/SteamJailer/blob/main/BUILD.md

Have re-built project using FreeBSD POSIX syntax, project is designed to be desktop agnostic


Code supports FreeBSD-14.2-RELEASE and higher, attempted to make one compati provides system logging

I went to all available AI's while working on this project, now my personal experience with AI's and the way I utilize them is I upload my code, have them print the project functions, this helps to identify errors then debug, none of available AI's could successfully read and execute FreeBSD POSIX syntax for me, then I read the other day about a new one, and checked, yup, it was available to me, so I began using it.

Upon first usage it stated it was in learning mode and was capable of any programming task, so I began showing it how to properly setup defines in headers, in the screenshot I took was the results of me asking it to generate a src/main.cpp with includes in header.

previous attempts were using FreeBSD ports and packages jail installers, this projects current state project uses vnet jail, I began this project with Bastille, then each attempt to install was met with error, then switched to iocage, while using iocage a Steam install would successfully occur perhaps once every two attempts, this was not up to my standards, so switched to ezjail, then back and forth, each attempt is using, I believe some of the issue I was experiencing was the use of third party apps

Currently new re-build utilizes the same FreeBSD POSIX syntax though has dropped third party apps for the jail install, the AI now knows how  to code better than I do on the third day of interaction.

Executes FreeBSD with POSIX syntax via shell script which will then be exported to c++ POSIX
