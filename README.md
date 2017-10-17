## Synopsis

xigor aims to be a little tool like xbiff was in the old days. It just 
sits there as a user defined icon in the systray area of your favorite 
desktop environment and as soon as a user definable trigger file is changed/touched,
the icon changes to another user definable icon. as soon as the icon is clicked 
with the left mousebutton, it will be reset to the original icon and waits for the 
next change/touch of the triggerfile. A click with any other mousebutton will remove 
the icon.and terminate the application.

It can be used as a mail notification or any other sort of flagging style info
There can be as many instances as you like.

Directly after the startup of the program, it dumps its PID to stdout and detaches 
itself, so you can script around with it very easily.

## Usage

$ xigor -h

usage: xigor [options]
        -h              print this usage and exit
        -d              delay between checks for changes of the trigger file
        -D              do not detach and print some debug output to stdout
        -m              path to the monitorfile (default: ~/.xigor-trigger)
        -s              path to the normal systray icon file
        -a              path to the flagged systray icon file
        -e              path to the error systray icon file

spawns a flagging icon that displays the icon specified by -s in your desktop environments systray area. This icon will be changed into the one specified by -a, as soon as the file specified by -m is changed/touched. The icon specified by -e is used if the monitor file is not existing or cannot be examined by stat(). Detaches immediately after start and outputs the PID of the detached process on sdtdout before doing so.

## Installation

just using make will compile everything. Make sure that you have GTK+3 installed.

## License

Copyright (C) 2017  Thomas Thiel

This program comes with ABSOLUTELY NO WARRANTY; This is free software, and you are welcome to redistribute it within the terms of the GPLv3.
