sz81   - a ZX81 and ZX80 emulator using SDL

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


************************************************************************
* Contained within this document is information specific to sz81 only. *
* Please see README.z81 for information about [x]z81, zx81get and the  *
* contents of the games-etc and saverom folders.                       *
************************************************************************


Table of Contents
=================

1.Description
    Features
2.Controls
    For the PC
    For the Amiga
    For the Sharp Zaurus
    For the GPH GP2X/Wiz
3.Operation
    General GUI Navigation
    Loading a Program File
    Saving a Program File
    Saving and Loading State Files
    Configuring a Joystick
    Remapping Joystick Controls
    Using the ZX Printer
    The Configuration File
      show_input_id
      ctrl_remap
    Exiting sz81
4.Compilation and Installation
    For desktop Linux
    For the Amiga
    For the Sharp Zaurus
    For the GPH GP2X/Wiz
5.Porting to Other Platforms
6.Thanks
7.Contacting Us


1.Description
=============

sz81 is a Sinclair ZX80/ZX81 emulator very much based upon the work of
Ian Collier's xz80 and Russell Marks's z81 but employing the highly
portable SDL and including additional functionality and features for
desktop computers and portable devices.

Features:
 * Virtual keyboard with several adjustable properties
 * Control bar with icons for regularly used features
 * Load anytime file dialog (no need to use LOAD)
 * Save state system with 9 slots for each program
 * Runtime options including a joystick configurator
 * RAM configurations of 1 to 4, 16, 32, 48 and 56K
 * Quicksilva, BI-PAK ZON X-81 and VSYNC-based sound
 * Screenshot saving using the bitmap image format
 * Joystick control remapping within the emulator
 * Full keyboard, mouse and joystick support throughout
 * 3x runtime switchable scaling on supported platforms
 * Window and fullscreen toggling on supported platforms
 * Portrait orientated resolutions such as 240x320
 * The ability to run centred within any resolution
 * Source code support for *nix, AmigaOS4 and Win32
 * Maximum porting potential since it only requires SDL


2.Controls
==========

For the PC:

    Click screen  - Toggle virtual keyboard and control bar
    Backspace     - Rubout i.e. equivalent to using SHIFT + 0
    Comma         - Equivalent to using SHIFT + .
    Cursors       - Equivalent to using SHIFT + 5, 6, 7 and 8
    -/=           - Decrease/increase the volume (if supported)
    ALT + R       - Cycle between 960x720, 640x480 and 320x240
    F11           - Toggle between fullscreen and a window
    F9            - Activate the control remapper for remapping
                    joystick controls to keyboard controls
    PrtScn        - Save a screenshot to the local scnsht folder
    Pause         - Place emulation on hold but not the GUI

    Clicking the screen (or F1) brings up the virtual keyboard and the
    control bar enabling access to several very useful features. These
    are listed below alongside their keyboard equivalents :-

    Exit          - Exit emulator (F10)
    Reset         - Reset emulator (F12)
    Autohide      - Toggle vkeyb between autohide and don't hide (F6)
    Shift Type    - Toggle between sticky shift and toggle shift (F7)
    Opacity DN/UP - Reduce (HOME) or increase (END) vkeyb opacity
    Invert Screen - Toggle between not inverse and inverse video (F8)
    Save State    - Toggle the save state dialog if available (F4)
    Load State    - Toggle the load state dialog if available (F5)
    Load File     - Toggle the load file dialog (F3)
    Options       - Toggle the runtime options (F2)

For the Amiga:

    As above section "For the PC" plus:

    Tooltypes
    ---------

    sz81 can be configured using tooltypes:

    FULLSCREEN
    XRES=<240+>
    YRES=<240+>

    LOCALDATA=<dir to save user data> (default: PROGDIR:save)
    STATICLOADDIR - LOAD/SAVE always uses <LOCALDATA>/progrm (default is
                    to LOAD/SAVE from the directory last used)

    File Selector
    -------------

    The Amiga build uses standard ASL requesters instead of sz81's
    built-in file selector.

For the Sharp Zaurus:

    Click screen  - Toggle virtual keyboard and control bar
    Backspace     - Rubout i.e. equivalent to using SHIFT + 0
    Comma         - Equivalent to using SHIFT + .
    Cursors       - Equivalent to using SHIFT + 5, 6, 7 and 8
    -/=           - Decrease/increase the volume (if supported)

    See PC controls for an explanation of the control bar.

For the GPH GP2X/Wiz:

    Within the emulator (remappable)   | Within the GUI
    -----------------------------------+----------------------
    LTrigger      - SHIFT              | SHIFT/Page up
    RTrigger      -                    | Page down
    Joy Left      - O                  | Selector left
    Joy Right     - P                  | Selector right
    Joy Up        - Q                  | Selector up
    Joy Down      - A                  | Selector down
    Select        - Load file dialog   | Control remapper
    Start         - Virtual keyboard   |
    Button A      - Newline            | Select (selector hit)
    Button B      - Newline            | Newline
    Button Y      - Rubout (SHIFT + 0) | Rubout (SHIFT + 0)
    Button X      - Space              | Space

    See PC controls for an explanation of the control bar.


3.Operation
===========

    General GUI Navigation
    ----------------------
    Every component has a green selector that can be moved around using
    the cursor keys or the joystick (the virtual keyboard and control
    bar's selector will only appear if a joystick is detected). The item
    currently selected can be activated with the enter key, a joystick
    button (selector hit) or by clicking with the mouse. If you use the
    mouse then of course you are free to activate items directly and can
    ignore the selector. Page up and page down (or joystick equivalents)
    will flip between pages and scroll through lists. Escape operates as
    you might expect: it closes the topmost dialog or component and will
    continue to do so until the emulator window is reached.

    Loading a Program File
    ----------------------
    Typing the ZX80's LOAD ("W", newline) or the ZX81's LOAD "" ("J",
    SHIFT + "P" twice, newline) will show the load file dialog. There is
    a much easier way to show the same dialog and that's via the control
    bar icon or the F3 key (joystick select) but there is a fundamental
    difference in the way that these two methods work that you should be
    made aware of: a machine reset will occur if you load a program file
    by directly activating the dialog but not if you activate it via the
    machine's LOAD command. For the most part this won't matter at all,
    but if you are intending to set-up RAMTOP, load a program above it
    before loading another program then you will lose everything if you
    load a program by invoking the dialog directly.

    If you have an understanding of the ZX81 then you will know that it
    is possible to load a program by executing LOAD "PROG" in which case
    sz81's loading mechanism will search for prog.p or PROG.P within the
    last directory navigated to, or if you've yet to navigate anywhere
    then the start-up folder. If you'd like to load an .81 file instead
    then the .p extension can be overridden by appending an .81 suffix
    e.g LOAD "PROG.81".

    There are some additional ways to navigate and utilise the load file
    dialog that enhance its operability: directories can be opened and
    programs loaded with a single mouse or stylus click, the scrollbar
    is interactive and the list can be scrolled using a mouse wheel. It
    is also possible to select an item by typing its initial letter or
    number and then cycling to subsequent items with repeated presses.

    Saving a Program File
    ---------------------
    On the ZX80 saving is achieved by executing SAVE ("E", newline), but
    since there is no way to pass a filename and indeed no native sz81
    save file dialog either, a choice of two naming methods have been
    made available: the first and default method is to save the program
    into the last folder navigated to with the load file dialog (or the
    start-up directory) with a sequential filename e.g. zx80prog0123.o.
    The second funkier method is to embed a BASIC line somewhere within
    the program e.g. 9999 REM SAVE "PROG". The line number used is not
    important, but the use of the SAVE keyword is so you'll need to type
    9999 SAVE "PROG" and then move the cursor back before SAVE and add
    a REM. When the program is saved by executing SAVE, sz81's program
    saving mechanism will look for this embedded BASIC REM statement and
    extract the filename. The default .o extension can be overridden by
    appending a .80 suffix. Please note that if you are using a version
    of sz81 that's been ported to a non *nix platform, these filenaming
    methods may have been replaced with a save dialog native to your OS.

    Saving and Loading State Files
    ------------------------------
    sz81 supports the saving and later reloading of images that are most
    commonly known as save state files. They are simply files containing
    dumps of the emulated machine's memory and the emulator's variables
    to enable a program to be restored to the same state at some other
    point in the future. So, after loading a game and playing the first
    level or two you find that your lunch break is over and you'd like
    to save your current position within the game to continue with later
    when distraction beckons - simply activate the save state dialog by
    pressing F4 or selecting it from the control bar and choose a slot.
    Slots that are inverse have an existing state already stored there
    (from the same program) so either choose a new slot or overwrite an
    existing one. This can be achieved by clicking a slot with the mouse
    or stylus, moving the selector and pressing enter (selector hit) or
    pressing a number from 1 to 9.

    Loading a save state file requires that the original game is loaded
    first and then the load state dialog can be activated with F5 or via
    the control bar. Then simply choose one of the previous slots that
    you've saved to and away you go.

    Configuring a Joystick
    ----------------------
    If you have a digital or analogue joystick plugged in and you'd like
    to use it then open the runtime options (F2 from within the emulator
    or select the control bar icon) and employ the joystick configurator
    using either the cursor keys and enter, the mouse or stylus or the
    joystick once configured. Select a control on the graphical joystick
    representation (the selector will blink) and follow the instructions
    configuring as many of the controls as you possibly can for optimum
    usability.

    Remapping Joystick Controls
    ---------------------------
    With the virtual keyboard and control bar visible, position the
    selector over the function that you'd like to assign to a joystick
    control and press the control remapper (the selector will blink).
    Then press a joystick control to remap the function to the control.
    Existing controls that have been remapped are active within the
    emulator (you cannot remap GUI controls) whilst new controls are
    universally active. It is possible to include the SHIFT modifier
    within the control as long as it is active before you initiate
    remapping. To cancel remapping press the control remapper again.

    Using the ZX Printer
    --------------------
    Emulated printer output is currently written to sequentially named
    pbm image files stored within the local prtout folder. A new file is
    created when either of the ZX81's LLIST or LPRINT commands are used
    and then closed when the emulator is reset or sz81 exited.

    The Configuration File
    ----------------------
    Located within the local folder is the auto-generated configuration
    file sz81rc (or sz81.ini). It is possible to tweak sz81's operation
    by editing the contents of this file but the majority of people will
    leave it as is. Right at the top is show_input_id which can be set
    to TRUE to show input device id's (SDL_keysym's) which is useful if
    you are configuring an input device and would like to know the id's
    of the buttons. Additionally the entire control set is stored within
    this file and controls can be edited, added or removed. For example
    there is no print screen (SDLK_PRINT) key on a handheld device with
    no keyboard and so by default the user cannot take screenshots, but
    if they have a spare input button and they know its id then they can
    by adding the following ctrl_remap:

        ctrl_remap.components=COMP_ALL
        ctrl_remap.protected=TRUE
        ctrl_remap.device=DEVICE_JOYSTICK        // or DEVICE_KEYBOARD
        ctrl_remap.id=19                         // 19 is an example
        ctrl_remap.remap_device=DEVICE_KEYBOARD
        ctrl_remap.remap_id=SDLK_PRINT
        ctrl_remap.remap_mod_id=

    Simply put, this ctrl_remap active across all program components,
    protected from being modified by the control remapper, will cause
    the joystick button with the id of 19 to be intercepted and a PrtScn
    keyboard key event generated in its place. remap_mod_id can be used
    to generate an additional event and this is how backspace generates
    SHIFT + 0 within the emulator. I recommend looking at sdl_input.c
    and sdl_input.h to understand this system in more detail.

    Exiting sz81
    ------------
    Upon exiting, if you have modified any of the configurable options
    you will be presented with a dialog requesting that you confirm
    writing the changes to the configuration file (sz81rc, sz81.ini).
    If you choose "No" then the changes will be discarded. This is
    useful for when you have configured the emulator just how you like
    it and don't wish to disturb it with unimportant modifications.


4.Compilation and Installation
==============================

Extract the sz81 source package and change into the extracted directory.

For desktop Linux:

    Comment/uncomment the PREFIX BINDIR DOCDIR PACKAGE_DATA_DIR group at
    the top of the Makefile depending on whether you want to do a local
    install, system-wide install or run from the source folder (default)
    and then type :-

    make clean
    make
    (If you're running from the source folder then that's it, but if
    you're installing this locally or system-wide then continue)
    (Installing system-wide requires that you login as root now)
    make install

For the Amiga (requires SDK):

    gmake -f Makefile.amigaos4

    To get all the required files together in RAM:sz81:
    gmake -f Makefile.amigaos4 install

    The directory can then be dragged to final location.

For the Sharp Zaurus (requires SDK):

    make clean
    make -f Makefile.zaurus

    Install sz81_2.1.x_zaurus.ipk using Add/Remove Software.

For the GPH GP2X/Wiz (requires SDK):

    make clean
    (For the GPH toolchain execute the following line only)
    make -f Makefile.gp2x
    (For the Open2x toolchain execute the following line only)
    make -f Makefile.open2x
    (For the OpenWiz toolchain execute the following line only)
    make -f Makefile.openwiz

    Extract sz81_2.1.x_{gp2x|open2x|openwiz}.tar.gz and copy the
    contents onto your SD card.


5.Porting to Other Platforms
============================

This project requires SDL only, runs centred within any resolution, can
be scaled-up up to 3x, can be operated entirely with a keyboard, mouse
or joystick, ships with *nix, AmigaOS4 and Win32 source code support by
default and should port easily to many different platforms and devices.


6.Thanks
========

Thanks go to Mr 2X and Neil L from the gp32x.com forums for assisting
with testing the Wiz build.


7.Contacting Us
===============

Please visit the project's support page at
http://sourceforge.net/projects/sz81/support

2011-02-13



