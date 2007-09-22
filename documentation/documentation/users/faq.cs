====================
�asto kladen� ot�zky
====================

:Authors:   Aaron Digulla, Adam Chodorowski, Sergey Mineychev, AROS-Exec.org
:Copyright: Copyright � 1995-2007, The AROS Development Team
:Version:   $Revision: 26368 $
:Date:      $Date: 2007-07-29 22:31:59 +0200 (ne, 29 VII 2007) $
:Status:    Done.

.. Contents::

Common questions
================

Can I ask a question?
---------------------

Of course you can. Please go to `AROS-Exec forum <http://aros-exec.org/modules/newbb/viewtopic.php?topic_id=1636&start=0>`__ and 
read the threads and ask everything you want. This FAQ will be updated with the 
user's questions, but the forum still remains more recent.

What is AROS all about? 
-----------------------

Please read the introduction_.

.. _introduction: ../../introduction/index


What is the legal status of AROS?
---------------------------------

European law says that it is legal to apply reverse engineering techniques to
gain interoperability. It also says that it is illegal to distribute the
knowledge gained by such techniques. It basically means that you are allowed to
disassemble or resource any software to write something which is compatible (for
example, it would be legal to disassemble Word to write a program which converts
Word documents into ASCII text).

There are of course limitations: you are not allowed to disassemble the software
if the information you would gain by this process can be obtained by other
means. You must also not tell others what you learned. A book like "Windows
inside" is therefore illegal or at least of dubious legality.

Since we avoid disassembling techniques and instead use common available
knowledge (which includes programming manuals) which don't fall below any NDA,
the above doesn't apply directly to AROS. What counts here is the intention of
the law: it is legal to write software which is compatible with some other
software. Therefore we believe that AROS is protected by the law.

Patents and header files are a different issue, though. We can use patented
algorithms in Europe since European law doesn't allow patents on algorithms.
However, code that uses such algorithms that are patented in the USA could not
be imported to the USA. Examples of patented algorithms in AmigaOS include
screen dragging and the specific way menus work. Therefore we avoid implementing
these features in exactly the same way. Header files on the other hand must be
compatible but as different as possible from the orignal.

To avoid any trouble we applied for an official OK from Amiga Inc. They are
quite positive about the effort but they are very uneasy about the legal
implications. We suggest that you take the fact that Amiga Inc did not send us
any cease and desist letters as a positive sign. Unfortunately, no legally sound
agreement has been made yet, besides good intentions on both sides.


Why are you only aiming for compatibility with 3.1?
---------------------------------------------------

There have been discussions about writing an advanced OS with the features of
the AmigaOS. This has been dropped for a good reason. First, everyone agrees
that the current AmigaOS should be enhanced, but no one knows how to do that or
even agrees on what has to be enhanced or what is important. For example, some
want memory protection, but don't want to pay the price (major rewrite of the
available software and speed decrease).

In the end, the discussions ended in either flame wars or reiteration of the
same old arguments over and over again. So we decided to start with something we
know how to handle. Then, when we have the experience to see what is possible or
not, we decide on improvements.

We also want to be binary compatible with the original AmigaOS on Amiga. The
reason for this is just that a new OS without any programs which run on it has
no chance to survive. Therefore we try to make the shift from the original OS to
our new one as painless as possible (but not to the extent that we can't improve
AROS afterwards). As usual, everything has its price and we try to decide
carefully what that price might be and if we and everyone else will be willing
to pay it.


Can't you implement feature XYZ?
--------------------------------

No, because: 

a) If it was really important, it would be in the original OS. :-) 
b) Why don't you do it yourself and send a patch to us?

The reason for this attitude is that there are plenty of people around who think
that their feature is the most important and that AROS has no future if that
feature is not built in right away. Our position is that AmigaOS, which AROS
aims to implement, can do everything a modern OS should do. We see that there
are areas where AmigaOS could be enhanced, but if we do that, who would write
the rest of the OS? In the end, we would have lots of nice improvements to the
original AmigaOS which would break most of the available software but be worth
nothing, because the rest of the OS would be missing.

Therefore, we decided to block every attempt to implement major new features in
the OS until it is more or less completed. We are getting quite close to that
goal now, and there have been a couple of innovations implemented in AROS that
aren't available in AmigaOS.


How compatible is AROS with AmigaOS?
------------------------------------

Very compatible. We expect that AROS will run existing software on the Amiga
without problems. On other hardware, the existing software must be recompiled.
We will offer a preprocessor which you can use on your code which will change
any code that might break with AROS and/or warn you about such code.

Porting programs from AmigaOS to AROS is currently mostly a matter of a simple
recompilation, with the occasional tweak here and there. There are of course
programs for which this is not true, but it holds for most modern ones.


What hardware architectures is AROS available for?
--------------------------------------------------

Currently AROS is available in a quite usable state as native and hosted
(under Linux, and FreeBSD) for the i386 architecture (ie. IBM PC AT
compatible clones). There are ports under way at varying degrees of
completeness to SUN SPARC (hosted under Solaris) and Palm compatible
handhelds (native).


Will there be a port of AROS to PPC? 
------------------------------------

There is currently an effort under way to port AROS to PPC, initially
hosted under Linux.


Why are you using Linux and X11?
--------------------------------

We use Linux and X11 to speed up development. For example, if you implement
a new function to open a window you can simply write that single function and
don't have to write hundreds of other functions in layers.library,
graphics.library, a slew of device drivers and the rest that that function might
need to use. 

The goal for AROS is of course to be independent of Linux and X11 (but it would
still be able to run on them if people really wanted to), and that is slowly
becoming a reality with the native versions of AROS. We still need to use Linux
for development though, since some development tools haven't been ported to AROS
yet.


How do you intend to make AROS portable?
----------------------------------------

One of the major new features in AROS compared to AmigaOS is the HIDD (Hardware
Independent Device Drivers) system, which will allow us to port AROS to
different hardware quite easily. Basically, the core OS libraries do not hit the
hardware directly but instead go through the HIDDs, which are coded using an
object oriented system that makes it easy to replace HIDDs and reuse code.


Why do you think AROS will make it? 
-----------------------------------

We hear all the day from a lot of people that AROS won't make it. Most of them
either don't know what we are doing or they think the Amiga is already dead.
After we explained what we do to the former, most agree that it is possible. The
latter make more problems. Well, is Amiga dead right now? Those who are still
using their Amigas will probably tell you that it isn't. Did your A500 or A4000
blow up when Commodore went bankrupt? Did it blow up when Amiga Technologies
did?

The fact is that there is quite little new software developed for the Amiga
(although Aminet still chugs along quite nicely) and that hardware is also
developed at a lower speed (but the most amazing gadgets seem appear right now).
The Amiga community (which is still alive) seems to be sitting and waiting. And
if someone releases a product which is a bit like the Amiga back in 1984, then
that machine will boom again. And who knows, maybe you will get a CD along with
the machine labeled "AROS". :-)


What do I do if AROS won't compile?
-----------------------------------

Please post a message with details (for example, the error messages you
get) on the Help forum at `AROS-Exec`__ or become a developer and
subscribe to the AROS Developer list and post it there, and someone will
try to help you.

__ http://aros-exec.org/


Will AROS have memory protection, SVM, RT, ...?
-----------------------------------------------

Several hundred Amiga experts (that's what they thought of themselves at least)
tried for three years to find a way to implement memory protection (MP) for
AmigaOS. They failed. You should take it as a fact that the normal AmigaOS will
never have MP like Unix or Windows NT.

But all is not lost. There are plans to integrate a variant of MP into AROS
which will allows protection of at least new programs which know about it. Some
efforts in this area look really promising. Also, is it really a problem if your
machine crashes? Let me explain, before you nail me to a tree. :-) The problem
is not that the machine crashes, but rather: 

1. You have no good idea why it crashed. Basically, you end up having to poke 
   with a 100ft pole into a swamp with a thick fog. 
2. You lose your work. Rebooting the machine is really no issue.

What we could try to construct is a system which will at least alert if
something dubious is happening and which can tell you in great detail what was
happening when the machine crashed and which will allow you to save your work
and *then* crash. There will also be a means to check what has been saved so you
can be sure that you don't continue with corrupted data.

The same thing goes for SVM (swappable virtual memory), RT (resource tracking)
and SMP (symmetric multiprocessing). We are currently planning how to implement
them, making sure that adding these features will be painless. However, they do
not have the highest priority right now. Very basic RT has been added, though.


Can I become a beta tester?
---------------------------

Sure, no problem. In fact, we want as many beta testers as possible, so
everyone is welcome! We don't keep a list of beta testers though, so all
you have to do is to download AROS, test whatever you want and send us a
report.


What is the relation between AROS and UAE?
------------------------------------------

UAE is an Amiga emulator, and as such has somewhat different goals than AROS.
UAE wants to be binary compatible even for games and hardware hitting code,
while AROS wants to have native applications. Therefore AROS is much faster than
UAE, but you can run more software under UAE.

We are in loose contact with the author of UAE and there is a good chance that
code for UAE will appear in AROS and vice versa. For example, the UAE developers
are interested in the source for the OS because UAE could run some applications
much faster if some or all OS functions could be replaced with native code. On
the other hand, AROS could benefit from having an integrated Amiga emulation.

Since most programs won't be available on AROS from the start, Fabio Alemagna
has ported UAE to AROS so you can run old programs at least in an emulation box.


What is the relation between AROS and Haage & Partner?
------------------------------------------------------

Haage & Partner used parts of AROS in AmigaOS 3.5 and 3.9, for example the
colorwheel and gradientslider gadgets and the SetENV command. This means that in
a way, AROS has become part of the official AmigaOS. This does not imply that
there is any formal relation between AROS and Haage & Partner. AROS is an open
source project, and anyone can use our code in their own projects provided they
follow the license.


What is the relation between AROS and MorphOS?
----------------------------------------------

The relationship between AROS and MorphOS is basically the same as between AROS
and Haage & Partner. MorphOS uses parts of AROS to speed up their development
effort; under the terms of our license. As with Haage & Partner, this is good
for both the teams, since the MorphOS team gets a boost to their development
from AROS and AROS gets good improvements to our source code from the MorphOS
team. There is no formal relation between AROS and MorphOS; this is simply how
open source development works.


What programming languages are available?
-----------------------------------------

Most development for AROS is done using ANSI C by crosscompiling the
sources under a different OS, eg. Linux or FreeBSD. Fabio Alemagna has
completed an initial port of GCC to i386 native. However, it is not
currently on the ISO or integrated into the build system.

The languages that are available natively are Python_, Regina_ and False_:

+ Python is a scripting language which has become quite popular, because of 
  its nice design and features (object-oriented programming, module system,
  many useful modules included, clean syntax, ...). A separate project has 
  been started for the AROS port and can be found at 
  http://pyaros.sourceforge.net/.

+ Regina is a portable ANSI compliant REXX interpreter. The goal for the AROS
  port is to be compatible with the ARexx interpreter for the classic
  AmigaOS.

+ False can be classified as an exotic language, so it will most likely not be 
  used for serious development, although it can be lots of fun. :-) 

.. _Python: http://www.python.org/
.. _Regina: http://regina-rexx.sourceforge.net/
.. _False:  http://wouter.fov120.com/false/


Why is there no m68k emulator in AROS?
--------------------------------------

To make old Amiga programs run on AROS, we have ported UAE_ to AROS. AROS's
version of UAE will probably be a bit faster than other versions UAE since AROS
needs less resources than other operating systems (which means UAE will get more
CPU time), and we'll try to patch the Kickstart ROM in UAE to call AROS
functions which will give another small improvement. Of course, this only
applies to the native flavors of AROS and not the hosted flavors.

But why don't we simply implement a virtual m68k CPU to run software directly on
AROS? Well, the problem here is that m68k software expects the data to be in big
endian format while AROS also runs on little endian CPUs. The problem here is
that the little endian routines in the AROS core would have to work with the big
endian data in the emulation. Automatic conversion seems to be impossible (just
an example: there is a field in a structure in the AmigaOS which sometimes
contains one ULONG and sometimes two WORDs) because we cannot tell how a couple
of bytes in RAM are encoded.

.. _UAE: http://www.freiburg.linux.de/~uae/


Will there be an AROS Kickstart ROM? 
------------------------------------

There might be, if someone creates a native Amiga port of AROS and does all the
other work needed to create a Kickstart ROM. Currently, no one has applied for
the job. 


Software questions
==================

How do I access AROS's disk images from UAE?
--------------------------------------------

The floppy disk image can be mounted as a hardfile and then used as a 1.4 MB
harddisk within UAE. After you have put the files you want on the hardfile disk
image (or whatever you wanted to do), you can write it to a floppy.

The geometry of the hardfile is as follows::

    Sectors    = 32
    Surfaces   = 1
    Reserved   = 2
    Block Size = 90


How do I access AROS's disk images from hosted flavors of AROS?
---------------------------------------------------------------

Copy the disk image to the DiskImages directory in AROS (SYS:DiskImages, eg.
bin/linux-i386/AROS/DiskImages) and rename it to "Unit0". After starting AROS,
you can mount the disk image with::

    > mount AFD0: 


What is Zune?
-------------

In case you read on this site about Zune, it's simply an open-source
reimplementation of MUI, which is a powerful (as in user- and
developer-friendly) object-oriented shareware GUI toolkit and de-facto
standard on AmigaOS. Zune is the preferred GUI toolkit to develop
native AROS applications. As for the name itself, it means nothing,
but sounds good.

How can I restore my Prefs to defaults?
---------------------------------------

In AROS, open CLI shell, go to Envarc: and delete relevant files for the 
pref you want to restore.

What is the Graphical and other memory in Wanderer?
---------------------------------------------------

This memory division is mostly a relic from Amiga past, when graphical memory 
was application memory before you added some other, called FAST RAM, a memory where 
applications ended, while the graphics, sounds and some system structures were 
still in graphic memory.

In AROS-hosted, there isn't such kind of memory as Other (FAST), but only GFX, when on 
Native AROS, GFX can have a max of 16MB, although it wouldn't reflect the state 
of the graphic adapter memory...  It has no relation to the amount of memory 
on your graphics card.

*The longwinded answer*
Graphics memory in i386-native signifies the lower 16MB of memory 
in the system. That lower 16MB is the area where ISA cards can do DMA. Allocating 
memory with MEMF_DMA or MEMF_CHIP will end up there, the rest in the other (fast)
memory.

Use C:Avail HUMAN command for memory info.

What do the Wanderer Snapshot <all/window> action actually do? 
--------------------------------------------------------------

This conmmand remembers icon's placement of all (or one) windows.

How do I change the screensaver/background?
-------------------------------------------

At the moment the only way to change screensaver is to write your one.
Blanker commodity could be tuned with Exchange, but it is able to do only 
"starfield" with given amount of stars.
Background of Wanderer is set by Pref tool Prefs/Wanderer.
Background of Zune Windows is set by Zune prefs Prefs/Zune. You can also set 
your chosen application preferences by using the Zune <application> command.

I've launch AROS-hosted but it's failed
---------------------------------------

This could be probably fixed by creating a WBStartup directory in the AROS 
directory. If you are root and AROS crashes at launch, do "xhost +" before 
"sudo && ./aros -m 20". You must also give it some memory with -m option as 
shown. Also don't forget about BackingStore option in section Device of your 
xorg.conf.

What are the command line options for AROS-hosted executable?
-------------------------------------------------------------

You can get a list of them by rinning ./aros -h command.

How can I make windows refresh from black on AROS-hosted?
---------------------------------------------------------

You must supply the following string (as is!) to your /etc/X11/xorg.conf
(or XFree.conf)::
    
    Option  "BackingStore"

What are the AROS-native kernel options used in GRUB line?
----------------------------------------------------------

Here's some::

    nofdc - Disables the floppy driver completely.
    noclick - Disabled the floppy disk change detection (and clicking)
    ATA=32bit - Enables 32-bit I/O in the hdd driver (safe)
    forcedma - Forces DMA to be active in the hdd driver (should be safe, but       
    might not be)
    gfx=<hidd name> - Use the named hidd as the gfx driver
    lib=<name> - Load and init the named library/hidd

Please note that they are case-sensitive.

How can I transfer files to virtual machine with AROS?
------------------------------------------------------

First and simpliest way is to put files to the ISO image and and connect it to VM. 
There's a lot of programs able to create/edit ISO's like UltraISO, WinImage, 
or mkisofs. Second, you can set up the network in AROS and FTP server on your 
host machine. Then you can use FTP client for AROS to transfer files
(look for MarranoFTP). This is tricky enough to stop at this point. User 
documentation contains a chapter about networking, go for it. Also, now
there`s a promising utility (AFS Util), allowing to read (no write support yet) 
files from AROS AFFS/OFS disks and floppies.  

Compilation Errors
------------------

Q: I`ve compiled AROS with gcc4 but found that compiled AROS-hosted segfaults 
with -m > 20 and if I compile AROS-native it does not start (black screen)
A: Add -fno-strict-aliasing to scripts/aros-gcc.in and try to recompile.

Is it possible to make a DOS script that automatically runs when a package is installed? 
----------------------------------------------------------------------------------------

This script should do some assigns and add string to the PATH variable.

1) Create a subdir S and add a file with name 'Package-Startup' with the DOS 
commands to it.

2) Create a variable in the envarc:sys/packages file which contains the path to the S 
directory of your package.

Example::
    Directory layout:

    sys:Extras/myappdir
    sys:Extras/myappdir/S
    sys:Extras/myappdir/S/Package-Startup
    
The variable in envarc:sys/packages could have the name 'myapp' (name doesn't 
matter), the content would then be 'sys:extras/myappdir'

The Package-Startup script would then be called by the startup-sequence.

This is were it is called::

    If EXISTS ENV:SYS/Packages
        List ENV:SYS/Packages NOHEAD FILES TO T:P LFORMAT="If EXISTS $SYS/Packages/%s*NCD $SYS/Packages/%s*NIf EXISTS S/Package-Startup*NExecute S/Package-Startup*NEndif*NEndif*N"
        Execute T:P
        Delete T:P QUIET
        CD SYS:
    EndIf
    
How do I clear the shell window? How do I set it permanently?
-------------------------------------------------------------

Type this command in the shell::

    Echo "*E[0;0H*E[J* "
    
You can edit your S:Shell-Startup and insert this line somewhere, so 
you'll have a new "Cls" command::

    Alias Cls "Echo *"*E[0;0H*E[J*" "

Btw here is my new S:Shell-Startup modified to start the shell in black and 
with a modified prompt::

    Alias Edit SYS:Tools/Editor
    Alias Cls "Echo *"*E[0;0H*E[J*" "
    Echo "*e[>1m*e[32;41m*e[0;0H*e[J"
    Prompt "*n*e[>1m*e[33;41m*e[1m%N/%R - *e[30;41m%S>*e[0m*e[32;41m "
    date

More about printer escape sequences::

    Esc[0m
    Standard Set

    Esc[1m and Esc[22m
    Bold

    Esc[3m and Esc[23m
    Italics

    Esc[4m and Esc[24m
    Underline

    Esc[30m to Esc[39m
    Set Front Color

    Esc[40m to Esc[49m
    Set Background Color

Values meanings::

    30 grey char -- 40 grey cell -- >0 grey background ---- 0 all attributes off
    31 black char - 41 black cell - >1 black background --- 1 boldface
    32 white char - 42 white cell - >2 white background --- 2 faint
    33 blue char -- 43 blue cell -- >3 blue background ---- 3 italic
    34 grey char -- 44 grey cell -- >4 grey background ---- 4 underscore
    35 black char - 45 black cell - >5 black background --- 7 reverse video
    36 white char - 46 white cell - >6 white background --- 8 invisible
    37 blue char -- 47 blue cell -- >7 blue background

The codes can be combined by separating them with a semicolon.

How do I launch AROS-hosted in fullscreen?
------------------------------------------

Call "export AROS_X11_FULLSCREEN=1" in a shell. Start AROS and change the 
screen resolution in the screenmode preferences. Quit AROS and start it again.

How to make 2-state AROS Icons?
-------------------------------

AROS icons is actually renamed PNG files. But if you want icons in two states 
(free/clicked) use this command::

    join img_1.png img_2.png TO img.info
    
How to mount an ISO image on AROS? And can I update my nightly build this way?
------------------------------------------------------------------------------

Get the ISO into AROS (by wget or else way)
Copy the ISO into sys:DiskImages (drawer must be created if it isn`t exist). 
Rename ISO to Unit0 in that dir.
You must add this to your Devs:Mountlist ::

    ISO:
    FileSystem = cdrom.handler
    Device = fdsk.device
    Unit = 0

Then mount ISO:
you can copy anything from ISO: Or for example, make a script to update your 
nightly build like this::

    *Copy ISO:boot/aros-pc-i386.gz sys:boot/
    *copy ISO:C sys:C all quiet
    *copy ISO:Classes sys:Classes all quiet
    *copy *copy ISO:Demos sys:Demos all quiet

and so on for each directory except Prefs, Extras:Networking/Stacks, and 
devs:mountlist itself. Prefs have to be kept if you want it. Also you can set 
AROSTcp to keep it`s settings in separate directory.

If you want to write all over, just do::

    copy ISO:C sys:C all quiet newer  
    
How to unmount a volume?
------------------------

Launch these two commands in CLI::
    
    assign DOSVOLUME: dismount
    assign DOSVOLUME: remove

where DOSVOLUME is DH0:, DF0:, etc

How to mount a FAT Floppy with the FAT.handler?
-----------------------------------------------

Create a mountfile (text file) with the 3 magic lines::

    device = trackdisk.device
    filesystem = fat.handler
    unit = 0

Call it somehow, PC0 for example. Set this file default tool to c:mount in 
properties (or put mountfile to devs:dosdrivers or sys:storage/dosdrivers)
Double click on it.
Insert a FAT formatted floppy.
See the icon appearing on Wanderer`s desktop.

How to mount a real HD FAT partition with the FAT.handler?
----------------------------------------------------------

First you`d need to read the drive`s geometry and write down some values. 
You can use HDToolbox or linux fdisk for that. The BlocksPerTrack value is taken 
from the sectors/track value. Note that it has absolutely nothing to do with 
the physical disk geometry - FAT only uses it as a multiplier.
If you get the Cylinders eg from HDToolbox or using the Linux fdisk like this::

    sudo fdisk -u -l /dev/hda, 
    
Then you'll need to set BlocksPerTrack=63. 
To ensure you have numbers in cylinders look for Units=Cylinders in output. If 
you got fdisk output in sectors (Units=sectors), set BlocksPerTrack=1.

LowCyl and HighCyl is partition`s cylinders seen like::

    mark@ubuntu:~$ sudo fdisk -l -u /dev/hda
    ...
    /dev/hda1 * 63 20980889 10490413+ c W95 FAT32 (LBA)

So, LowCyl is 63, and HighCyl is 20980889, blockspertrack=1

Create a mountfile (text file) with these lines::

    
    device = ata.device
    filesystem = fat.handler,
    Unit = 0

    BlocksPerTrack = 1
    LowCyl = 63
    HighCyl = 20980889
    Blocksize=512

Call it somehow, FAT0 for example
Set this file`s default tool to c:mount in properties
(or put mountfile to devs:dosdrivers or sys:storage/dosdrivers)
Double click on it
See the icon appearing on Wanderer`s desktop

Note: Formulae for counting the blocks
block = ((highcyl - lowcyl) x surfaces + head) x blockspertrack + sec

Hardware questions
==================

Where can I find an AROS Hardware Compatibility List?                   
-----------------------------------------------------

You can find one on the `AROS Wiki <http://en.wikibooks.org/wiki/Aros/Platforms/x86_support>`__ 
page. There can be another lists made by the AROS users.

Why Aros can't boot from my drive set as the SLAVE on IDE channel?
------------------------------------------------------------------

Well, AROS should boot if the drive is SLAVE but ONLY if there's a drive also 
on MASTER. That's appeared to be a correct connection respecting to the IDE 
specification, and AROS follows it.

My system hangs with red cursor on screen or a blank screen
-----------------------------------------------------------

One reason for this can be use of the serial mouse (these is not supported yet).
You must use PS/2 mouse with AROS at the moment. Another can be if you've chosen
a video mode unsupported by your hardware in boot menu. Reboot and try a different one.
