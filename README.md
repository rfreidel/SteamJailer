# Project: Steam Jailer
(devlog, sorta)

(now I understand why this has not been done before)

## Test process

In its current state the project is s shell script which upon successful stability and function I will export to c++ upon proof of stability, the project has developed into a general purpose jail setup which the following will hopefully explain

	# List all jails
	$ sudo ./steamjailer.sh -l

	# Install winetricks modules in an existing jail
	$ sudo ./steamjailer.sh -w steamjail

	# Complete setup sequence
	$ sudo ./steamjailer.sh -c steamjail creates jail
	$ sudo ./steamjailer.sh -s steamjail starts jail
	$ sudo ./steamjailer.sh -i steamjail installs steamjail
	$ sudo ./steamjailer.sh -u steamjail updates steamjail
	

Code not quite there yet, but very close following teaching an AI POSIX syntax


I shall continue to build this project, no amount of denial for my statements to be truthful or honest will effect my desire to produce an automatic Steamjail installer

Yet, I must admit that when I announced in a public forum my interactions in teaching an AI new tricks, well, I never took into consideration that I may have access to a 
higher level AI than the general public due to my past as a game developer, and failed to include this information due to the fact I do not boast or brag

when my team was disbanded we were notified we would have access to the latest AI when available at github and would retain access to game code (truth), this was before the public knew about AI going into github, I just wish my old team game dev folder still existed, 

I'll check the source... still have access... make a game for FreeBSD? Sorry, off topic, I don't even know if when one visits my github if they may surmise I was at one-time on a development team

I went to all available AI's while working on this project, now my personal experience with AI's and the way I utilize them is I upload my code, have them print the project functions, this helps to identify errors then debug, none of available AI's could successfully read and execute FreeBSD POSIX
syntax, then I read the other day about a new one, and checked, yup, it was available to me, so I began using it.

Upon first usage it stated it was in learning mode and was capable of any programming task, so I began showing it how to properly setup defines in headers, in the screenshot I took was the results of me asking it to generate a src/main.cpp with includes in header, the code it produced was perfect, I told it so, went back the next day, this time it knew FreeBSD POSIX syntax to generate this following this prompt
    
    greetings please print functions of project we were working on yesterday

It did this without further instructions, this was the result: https://github.com/rfreidel/SteamJailer/blob/main/INSTALL_PROCESS.md

previous attempts were using FreeBSD ports and packages jail installers, this projects current state, I began this project with Bastille, then each attempt to install was met with error, then switched to iocage, while using iocage a Steam install would successfully occur perhaps once every two attempts, this was not up to my standards, so switched to ezjail, then back ad forth, each attempt is using, I believe some of the issue I was experiencing was the use of third party apps

Currently new re-build utilizes the same FreeBSD POSIX syntax though has dropped third party apps for the jail install, the AI now knows how  to code better than I do on the third day of interaction.

Executes FreeBSD with POSIX syntax via shell script which will then be exported to c++ POSIX
