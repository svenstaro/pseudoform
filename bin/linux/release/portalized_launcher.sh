#!/bin/bash

# This is a script for launching Pseudoform along with the provided
# shared libraries. It also takes care of some eventualities in case
# it is launched from odd places. In case of a crash, some data about 
# the crash is gathered in a file along with some info about the the
# computer's configuration. Nothing is sent anywhere over the Internet,
# this has to be done manually and with the user's consent. 
# -----------------------------------------------------------------------------
# Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
# This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
# For conditions of distribution and use, see copyright notice in COPYING

GAMENAME=portalized
LOGFILE="$HOME"/."$GAMENAME"/"$GAMENAME".log

# Create game directory in home if non-existant
if [ ! -d "$HOME"/."$GAMENAME" ]; then
   mkdir -p "$HOME"/."$GAMENAME" || exit 1
fi   

# Make sure we are in the game directory
GAMEPATH=`readlink -f "$0"`
GAMEDIR=`dirname "$GAMEPATH"`
cd "$GAMEDIR"

# Check if the game was installed completely
if [ ! -d ../../../data ]; then
	echo "Can't find data/ directory in `pwd`"
	echo "Your installations seems to be incomplete!"
	exit 1
fi

# Begin new log
echo -e "STARTING PORTALIZED AT `date`\n" > "$LOGFILE"

# Figure out user's machine architecture and launch correct executable
MACHINE=`uname -m`
if [ "$MACHINE" = "x86_64" ]; then
	LIBS=./libs64
	BIN=./"$GAMENAME".bin64
	sed s'/plugins=.*$/plugins="libs64\/OGRE\/"/' -i ../../../data/scripts/root.xml
elif [ "$MACHINE" = "i686" ]; then
	LIBS=./libs32
	BIN=./"$GAMENAME".bin32
	sed s'/plugins=.*$/plugins="libs32\/OGRE\/"/' -i ../../../data/scripts/root.xml
else
	echo -e "\nERROR: Your machine type is unrecognized!"
	echo "Please report this issue to the developers along with the logfile at $LOGFILE"
	echo -e "Crash at `date`\nReason: Unrecognized architecture\nThe detected architecture is: `uname -a`" >> $LOGFILE
	exit 1
fi

# Finally, run the game
LD_LIBRARY_PATH="$LIBS":"$LD_LIBRARY_PATH" "$BIN" "$@" >> "$LOGFILE" 2>&1

# Check for crash
e=$?
if [ $e -ne 0 ]; then
	echo -e "\nERROR: Something went horribly wrong!"
	echo "Please report this issue to the developers along with the logfile at $LOGFILE"
	echo -e "\nLibraries $BIN links with:" >> "$LOGFILE"
	ldd $BIN >> "$LOGFILE" 2>&1
	if [ -z "`which glxinfo`" ]; then
		echo -e "\nCan't find glxinfo!" >> "$LOGFILE"
	else
		echo -e "\nglxinfo:" >> "$LOGFILE"
		glxinfo >> "$LOGFILE" 2>&1
	fi
elif [ $e -eq 0 ]; then
	echo -e "\nSUCCESS: Portalized shut down without error at `date`!" >> "$LOGFILE"
fi

# Workaround for OIS bug which doesn't restore text repeat on unclean exit
xset r
