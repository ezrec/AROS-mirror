=============================
Руководство пользователя AROS 
=============================

:Authors:   Stefan Rieken, Matt Parsons, Adam Chodorowski 
:Copyright: Copyright Љ 1995-2006, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Unfinished; only converted to reST. Needs heavy updating.

.. Warning::

   This document is not finished! It is highly likely that many parts are
   out-of-date, contain incorrect information or are simply missing 
   altogether. If you want to help rectify this, please contact us.

.. Contents::


Введение
========

This is the Amiga Research Operating System User's Guide. It is meant to get 
people used to AROS. It is for *everybody* interested in AROS, as it tries to 
provide information on AROS in different levels of advancedness. I try to cover
everything in depth, but in such a way that you don't need to learn what you 
don't *want* to learn.


Кому следует читать это руководство
-----------------------------------

This guide will help you getting used with AROS. It is written for everybody 
who is interested in AROS. Keep in mind that 
you are actually using software that is BETA and in research. It is currently 
mostly fun to play with and cool to program for and program in. So I expect 
that your interest in AROS is explained by one of these reasons. If you came 
until here because you thought AROS was a Multimedia Internet-Ready Etc. OS, 
well, you might be right, but it is *not finished*, so you need to be patient, 
boy. If you thought AROS was a Grapefruit-Machine or a Free Money Project, you
are entirely in the wrong place.


Как следует читать это руководство
----------------------------------

This guide is ordered from "simple" to "advanced". You can start 
reading at any chapter that contains information that is new to you. But maybe 
even more important, you should stop reading at any chapter that contains 
information going beyond your interest. In this way you can learn yourself the 
advanced topics starting from scratch, or you can stop earlier if you think you 
only want to use AROS, and not program it. People with an Amiga background can 
skip the introduction, and start at "Developing for the AROS platform" if they 
never programmed an Amiga before, or go directly to "Developing inside AROS" 
if they already did. So there is a starting point and a stop point for 
everyone.

It is important to realize that this guide is meant for AROS, not Amiga. So 
even if you owned an Amiga for years, you might need to read "Using AROS" too. 
This is not an embarrassment: you will notice that using AROS is very slightly 
different from using AmigaOS. This is because our Workbench is not finished.
At the moment the whole system works through a AmigaDos shell (or Cli to 
older users), although we do have a Workbench, you can not navigate disks with 
it yet. Old Amiga programmers should read "differences with Amiga programming" 
from chapter 4 to get an overview of the differences. 

Работа в AROS
=============

Операционная система в операционной системе ?
---------------------------------------------

AROS is originally developed on Linux_ running on an Intel-based computer. It 
runs on many more machines and Operating Systems, though. This may sound 
strange: an OS running on top of an other OS, that's emulation, right?

A nice term for what AROS does is "API emulation". API is a three-letter 
acronym for Application Programmer's Interface. In plain English: an API 
provides (C Language) functions that a programmer can use. The AmigaOS API 
consists of a load of library calls that an Amiga programmer can use to make 
an Amiga program. AROS emulates the AmigaOS API: it tries to provide the same 
library calls as AmigaOS. An Amiga emulator, such as UAE_; emulates the Amiga 
*computer*: the processor, the connected hardware, everything. This has its 
advantages, like being able to play binary Amiga games on different hardware, 
and its disadvantages, like not being able to use the emulator as a "real" OS, 
on a "real" processor. AROS runs on the "real" processor. But it isn't a 
"real" OS, *unless* you run in such a way that it doesn't require Linux. This 
is called "native" AROS.

AROS can run natively on the Intel and Amiga computers, but not quite as good 
as it runs on Linux. AROS library functions are made to run under Linux first, 
internally using Linux kernel and library calls. This way a programmer has got 
the opportunity to bother about the implementation of the whole system first, 
and to bother about the technical details in a later stadium. People are 
currently working on making the "native" AROS more impressive. The results are
very impressive and it is perfectly possible to use AROS as a real (and only)
Operating system on an IBM PC compatible machine.

Of course, AROS is not *only* an API emulator. It also tries to provide all the 
AmigaOS3.1 system software, and you will also find a few demos and games being 
shipped with AROS, just to show that they work - we might just be at
77% of the whole system, but we already have Quake running!


Работа с "нативной" AROS на i386 или m68k
-----------------------------------------

Native AROS is currently under heavy development. If you want to see cool 
tricks, try AROS on Linux. But if you're (also) interested in what a great job 
the programmers have done, you can try "native" too.

The instructions for installing native AROS are varying depending on which 
platform you use. Because "native" is still in great development, the *results*
from installing native AROS can also vary depending on the age of the code that 
you use.

On an Amiga, you can place the native code somewhere on you harddisk, 
double-click the "boot" icon, do a reset and enjoy a complete Amiga system. 
This is because it is not *really* native. The boot program just temporarily 
replaces a few AmigaOS libraries with AROS libraries. For testing purposes this 
is of course good, but in the end you still run good ol' AmigaOS and not plain 
native AROS. This will change as we build a more complete 68k AROS system.

On i386, you should write a boot image to a floppy and the contrib 
(Contribution disk, containing games, demos and utilities) images to another 
two floppy disks. If you downloaded binary code, there should be instructions 
in the archive for you. Once you have written your floppies, you can reboot the
system. After the floppy is booted, you will find yourself in AROS (looking 
very much like AmigaOS). You can fool around with the "AmigaDOS Shell"; or play 
some games/demos found on the contrib disks (included in the archive) until you 
get bored. AROS can detect disk changes so you can insert one of the contrib 
disks in the floppy drive and explore them, running games and demos on the way. 
For now this is all, but keep in touch, because hard work is currently being 
done here. Soon we should have a Workbench which will allow graphical 
navigation of your Hard and Floppy disks.


Работа с AROS на Linux и FreeBSD
--------------------------------

Once you got the binaries for your system, either by compiling or by 
downloading pre-compiled binaries, you should go down into the 
"bin/$TARGET/AROS" directory, where $TARGET is your system target (something 
like "linux-i386"). Run the file "aros" ("./aros"). The Workbench replacement
"Wanderer" will be started.

Because "Wanderer" is very limited you'll prefer to work with the Shell. You can
start it from the menu "Wanderer/Shell". Now you should type in commands, and the most 
important command is "dir": it will show you a directory's contents. The 
directory named "C" contains all the commands, so it might be useful to display 
its contents with "dir c:". The shell behaves like an AmigaDOS shell, and the commands in 
"C" behave like their AmigaDOS equivalents. (Note to UNIX folks: to address the parent 
directory, use "/" and not "..": this will look ugly because AROS thinks that 
Linux's ".." is a normal directory. You shouldn't use "./" as a prefix to 
address a command within the current directory either, but leave this away 
instead.) Once you are used to it, try to execute a few programs (especially 
the "Demos" and "Games") to get an impression of AROS capabilities.

Using AROS-hosted on PPC
------------------------

To be filled by someone...

Using the AfA on m68k
---------------------

To be filled by someone...

AROS Basics
===========

AROS Zune GUI Basics
--------------------

GUI abbreviation stands for Graphical User Interface, and is applied to
all the means used by OS to interact with user other than plain
command-line interface (CLI). For those who never have used any OS from
Amiga branch, it will be useful to give some GUI basics to help them in
use of our system. Some of it, however, will be AROS-specific. 

An Amiga systems use definitely and common principles, as you can
already note. First, any menu options of any application`s window isn`t
attached to that window - it moved to upper strip, where it can be
easily acessed. To do this, select window you`re need, and move mouse
pointer to upper side of a screen. Then, if you press right mouse button
there, you can see the pull-down menu, representing our application`s
options. Yes, it looks like MacOS somehow. Also you can enable the menu
to appear on any place of the screen, where you press left mouse button.
To do so ... For example, if no application window is selected, you can
see the Wanderer`s menu then. 

Now, let`s consider our desktop - as you probably already know, it`s
called Wanderer. What is this ? Well, Wanderer is an application, just
like all others. In fact, it is an AROS file manager, allowing you to
choose and operate files (the functionality isn`t complete yet), launch
programs, get some system information, launch CLI (shell window) and
other functions. Usually it opens on wide screen and acts like your
desktop (icons on this desktiop represents the volumes and disks you can
work with). It can be set aside by unselecting Backdrop option, which
can be found in Wanderer`s menu (remember paragraph above?). After that
a Wanderer becomes just another window you can move, resize etc. So, you
can see it isn`t like a Windows or another system`s desktop, fixed to
it`s place. Of course, you can even not use the Wanderer at all and use
your preferred file manager (e.g. Directory Opus).

But how do the applications behave then, where will they`re open?
There`s a `screen` term - screen is the place where your window is meant
to be open. If it`s said that application going to open on Wanderer
screen, it will look like it`s usually happens in other OS - your app
will appear as window on desktop. Also window usually can be open on
it`s own screen - it looks like it captures the whole screen. But you
can switch the screens with a gadget in top right corner fo the screen
(this is also applicable to the windows). So you can switch between
Wanderer, Directory Opus and any other apps opening on it`s own screen.
This behaviour also comes from Amiga`s history.
                                                  
Well, the time has come to say something about windows itself. Aros
window usually has controls to manupalte with it. This control buttons
are called gadgets (which can be translated as interactive kind of
graphical element). First one in the top left corner of a window allows
to close it. Next, in the right part allows to minimise/maximise window.
And the last used to put window to front or to back just like we switch
sreens. Windows can have no gadgets at all (look at the Kitty demo -
it`s doesn`t even have a borders and yet has well-curved shape) or have
a different set of them. 

The window`s contents consists from some usual elements could be seen in
any GUI - buttons, lists, strings of text, any other kind of gadgets. If
application is intended to change any preferences of a system or an
application it`s usual shortly called *Pref* and has a set of buttons to
operate. Usually this buttons are: TEST (applies all the changes made by
Pref but doesn`t save and do not the changes but close the window), SAVE
(saves the changes and close the window), USE (applies the changes and
close the window, but do not saves them), CANCEL (discard all the
changes and close the window).

Also, from Amiga`s history the file placement unit is often called a
drawer instead of a folder/directory in other systems, but it`s meaning
remains the same. Translate it as a directory if you`re unsure.

There`s a special keys in AROS, just like on original Amiga, used to make
quick commands with it. Left and Right WinKey (on PC keyboard) replaces the 
original Amiga Keys and is used in different combinations to launch commands.

Another unknown name you can encounter in AROS is Zune. What`s that ?
Zune is GUI toolkit developed in replacement and best traditions of MUI
(Magic User Interface), widely used on Amiga`s. But is there an
application called Zune?  You can find Zune Pref and it allows you to
set settings for Zune-based applications altogether or in particular.
For example, to set Zune prefs for Wanderer you can select GUI prefs
from it`s menu, or to set Zune prefs for other apps you can use it as
the CLI command Zune <app filename>.

To be finished...


AROS CLI (Command Line Interface)
---------------------------------

CLI commands abstract and comparision, TAB completion...

AROS has it`s CLI, the Command Line Interface, greatly expanding the 
capabilities of OS. Those who had used the AmigaOS can note that it looks
Pretty close to the AmigaDOS. There`s some CLI basics described in 
`introduction </users/introduction>`__ to CLI commands. 

To be finished...

AROS System programs
--------------------

We have mentioned the applications, it`s good to give a description of
their functions. So, there`s a groups of the AROS system applications
collected in the separate directories:

    + C - the place for all the system commands used in CLI
    + Classes - the place for datatypes, gadget`s images and Zune classes
    + Devs - where the device-related files (drivers, keymaps) and 
      datatypes are placed  
    + Extras - where all the contributed programs reside
    + Fonts - here you can find all of the system fonts. Any additional fonts 
      must be appended (assigned) to this dir.
    + Libs - where the system libraries are located.
    + Locale - holds catalog files of various AROS apps translations
    + Prefs - has a number of preferences-editing programs
    + S - contains some system launch-time scripts
    + System - the place for some system controls
    + Tools -  the place for some commonly used system apps
    + Utilities - the place for some not-so-commonly used but useful apps

Instead of applications, there`s more permanent running programs called
*tasks*. 

Another kind of AROS applications is the *Commodities*. This is applications 
which can help you make your system more comfortable. For example, AROS windows
doesn`t set to the top of others when you click on it, and you can find it 
uncommodable. You can use the AROS commodity ClickToFront to fix it. It can be 
found beneath other commodities in SYS:Tools/Commodities directory. When you 
double click on it, window will become to the top of others if double clicked.
Another example is Opaque commodity - it allows you to move windows with their
contents. There`s also an Exchange commodity which allows you to manipulate 
launched commodities and get information about them. Usually commodities do not 
open any windows.  

To operate with files of different types Amiga-like systems is using the 
*datatypes*. Datatype is the kind of system library allows the programs 
to read or/and write to such files without taking care of the implementing
such a format in that program.   

And if we dig a little deeper there`s some system terms that can be explained.
AROS uses *handlers* to communicate with the filesystems and *HIDD`s* to 
communicate with the hardware.

To be finished...

Customising the installed AROS
==============================

Setting up the Locale
---------------------

AROS is becoming a really international system this days, being
translated to many languages. Translating isn`t very difficult, and
number of the AROS translators is still increasing. If unicode support
will be implemented it can be translated in every language people use.
If you feel you can give AROS to your country, both OS and
documentation, do not hesitate to contact us and offer your help.

So about the language. First, depending on fonts used you must set fonts
by launching SYS:Prefs/Fonts and designating Fonts to different system
text: Icons (used for icons labels), Screen (used on common screen) and
System (used in CLI window). If your language uses different set than
ISO (for example, cyrillyc CP-1251) there`s *must* be the fonts in
correct codepage. Aros currently can use two kinds of fonts - the Amiga
bitmap fonts (which can be used directly) and TrueType (via FreeType 2
manager, which still has some issues with non-ISO codepages). Bitmap
fonts are in any particular codepage, and TTF can be unicode.

How can you change the AROS locale ? To do this you need to launch a
Locale pref in SYS:Prefs. You can see a list of supported locales there
and select your preferred ones. On the second page of this Pref you can
select the country used (it gives correct currency and date/time
format). And the last tab allows you to change timezone to that used in
your location.

After you`ve made changes to fonts reboot the system, and you must be
able to see all the translated content.

So now we can read, but can we write also in our language? To do this,
you must change the keyboard layout.

Keyboard pref...Also this pref allows you to save presets - just like any
application it`s got a menu, and you can save your preferences to file
with the given name and keep different settings of locales. We can use
it later to switch our keyboard layouts. Choose your layout from list and 
make a left click to open the context menu. 


To be finished...        

Setting up the Network
----------------------

To communicate with other computers on network, AROS uses a TCP Stack, AROSTCP, 
which is a port of AmiTCP. This software is located in /Extras/Networking/Stacks/AROSTCP
directory. Setting up is not easy but some kind of GUI tool is in development. 
Also please note that actually there`s a very little amount of networking
program on AROS yet (but some interesting tools is in development to be 
soon released).

First you need is to setup your machine side of network. This part can differ 
depending on your hardware. On a real machine you need to install the supported
network interface card (NIC) and plug the cable to it. On a virtual machine
you must set up it`s NIC implementation and check if it`s supported by AROS
(at least, QEMU and VMWare ones is supported). 

Net on the real PC
""""""""""""""""""

To be.

Net on QEMU/Linux
"""""""""""""""""

On linux system some steps needs to be done to make it working.

The tun (tunnel) module must be loaded::

    #> modprobe tun

Then, the kernel must become a router::

    #> echo 1 > /proc/sys/net/ipv4/ip_forward

Then, a rule must be added to the firewall::

    #> iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE

Finally, still being root, start Qemu with::

    #> qemu -cdrom aros.iso -m 48

After this is enabled we can go to the next point.

Second part is setting AROSTCP in AROS to work. 

The Linux tun module, by default, creates a gateway for the fake network at 
172.20.0.0/16 with a gateway at 172.20.0.1.
Say our Qemu hosted machine is at 172.20.0.10.
Say your usual LAN is 192.168.0.0/24 with a DNS at 192.168.0.1 
(or anywhere on the Internet, for that matter).

You have to edit 3 files in the AROSTCP/db: hosts, interfaces, netdb-myhost
In hosts remove any entries. Hosts will be in netdb-myhost for now.
In interfaces Uncomment the prm-rtl8129.device line, because Qemu is emulating 
this NIC, edit it::

    eth0 DEV=DEVS:networks/prm-rtl8029.device UNIT=0 NOTRACKING IP=172.20.0.10 UP

In netdb-myhost, add the various local known hosts, 
your local domain name, the gateway::

    HOST 172.20.0.10 arosbox.lan arosbox
    HOST 172.20.0.1 gateway
    DOMAIN lan
    NAMESERVER 192.168.0.1

The db directory can itself reside anywhere, you set its path in the 
ENVARC:AROSTCP/Config file, I advice you to copy the db files in the (created) 
ENVARC:AROSTCP/db directory, that way the Config file could be::

    ENV:AROSTCP/db

Now make AROSTCP start at boot with the word "True" in ENVARC:AROSTCP/Autorun
Edit the Sys:extras/Networking/Stacks/AROSTCP/S/Package-Startup::

    ; $VER: AROSTCP-PackageStartup 1.0 (01/08/06)
    ; AROSTCP-PackageStartup (c) The AROS Dev Team.
    ;
    Path "C" "S" ADD QUIET

    If not exists T:Syslog
        makedir T:Syslog
    Endif

    If not exists EMU:
        if $AROSTCP/AutoRun eq "True"
        C:execute S/startnet
        EndIf
    EndIf

The Sys:extras/Networking/Stacks/AROSTCP/S/Startnet file should be 
something like::

    ; $VER: AROSTCP-startnet 1.0 (01/08/06)
    ; AROSTCP-startnet (c) The AROS Dev Team.
    ;
    Run <NIL: >NIL: AROSTCP
    WaitForPort AROSTCP
    If NOT Warn
        run >NIL: route add default gateway
    Else
    ; echo "Wait for Stack Failed"
    EndIf

Next boot, test it with:

ifconfig -a
ping www.aros.org

Net on QEMU/Windows
"""""""""""""""""""

Setting QEMU to run on Windows is relatively harder to that of Linux. First,
make sure you have turn your Firewall to learning mode (or prepare it to
receive new rules) or completely disable it. Firewall can block transfers to VM

On Windows version of QEMU you will need to specify 
-net nic -net user switches in it`s command line. There`s also an option to use 
the tap interfaces. There`s some guides describing how to setup QEMU in Windows.

Setting the AROS side is similar to that of Linux use, but you will need to use
the following IP addresses to setup: 10.0.2.15 for ArROS machine IP, 
10.0.2.2 for gateway.

Net on VMWare
""""""""""""" 

VMWare`s side network is relatively easy to set up. 

Later...  

Installing the software
-----------------------    

Actually there`s no installer system in AROS. Installing the application
usually mean that you must extract it to some directory on a harddrive or
ramdisk. Then some probrams require you to make some directory assignments which
is done in CLI with the Assign command and some start script additions. 
For example, Lunapaint needs the Lunapaint: to be assigned to the directory
it was extracted to to work properly. You can do this with the command 
Assign Lunapaint: Disk:Path/Lunapaint . But if you don`t want to type this 
command after reboot to launch it again, you must put it to Startup-Sequence
script. To do this, type this command in CLI prompt::

    :> edit SYS:S/Startup-Sequence
    
Then insert the Lunapaint (or other program) assign after all assign lines 
in the beginning of the file. Save the changes and you`ll have that fixed.
Such a procedure can be used for any program needs it.

Is that all the User's Information in this guide?
=================================================

This chapter should have told you how to get, install and use AROS.
After having tried running every program in the directories C, Demos,
Utilities, Tools, Games, etc., you might wonder if that is all. Yes,
currently that is all a "User" can do with AROS! But when any new
important user code will be ready, it will be added to this guide, of
course.

If you think that I have not provided enough information here about
compiling, installing, Subversion, the shell, etc., it might be good to
know that I have reasons for it. First, there is already much
information available, and it would be unnecessary as well as unfair
just to copy that information in this document.  Second, we are talking
about very particular information. Some of the readers might be
interested in compiling the source code, others might want to know all
about the Amiga shell. So to keep this guide readable, I only point to
places where you can find such information, instead of providing it
here. You, the reader, can then decide if this is of interest to you.


.. _Linux: http://www.linux.org/
.. _UAE:   http://www.freiburg.linux.de/~uae/
.. _install: installation

