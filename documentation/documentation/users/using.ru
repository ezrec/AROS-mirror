=============================
Руководство пользователя AROS 
=============================

:Authors:   Stefan Rieken, Matt Parsons, Adam Chodorowski 
:Copyright: Copyright Љ 1995-2006, The AROS Development Team
:Version:   $Revision: 24430 $
:Date:      $Date: 2006-05-08 04:01:22 +0500 (а†аН, 08 аМаАаЙ 2006) $
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


Вся ли информация для пользователей собрана в этом руководстве?
---------------------------------------------------------------

This chapter should have told you how to get, install and use AROS. After 
having tried running every program in the directories C, Demos, Utilities, 
Tools, Games, etc., you might wonder if that is all. Yes, currently that is all 
a "User" can do with AROS! But when any new important user code will be ready, 
it will be added to this guide, of course.

If you think that I have not provided enough information here about compiling, 
installing, Subversion, the shell, etc., it might be good to know that I have
reasons for it. First, there is already much information available, and it
would be unnecessary as well as unfair just to copy that information in this
document. 
Second, we are talking about very particular information. Some of the readers 
might be interested in compiling the source code, others might want to know all 
about the Amiga shell. So to keep this guide readable, I only point to places 
where you can find such information, instead of providing it here. You, the 
reader, can then decide if this is of interest to you.


.. _Linux: http://www.linux.org/
.. _UAE:   http://www.freiburg.linux.de/~uae/
