Scalos - The Amiga Desktop Replacement
======================================

Scalos is a desktop environment for AmigaOS3/4, MorphOS and AROS. It
is an Open Source effort which is hosted at Sourceforge:
https://sourceforge.net/projects/scalos/

You can find a lot of information on its homepage:
http://scalos.noname.fr/

Please, use the ticket tracker at https://sourceforge.net/p/scalos/tickets/
to send any bug reports and feature requests.

Try to provide as much information about your configuration as possible
- *THIS IS IMPORTANT*.

General:
CPU, Graphics system, screen depth, enforcer/cyberguard hit log.

Specific:
Scalos prefs such as bob system type, what you have checked in
the "misc." options of the scalos prefs program etc. Anything you think
could be related to your problem.

Also, provide some means for us to contact you if the
bug is hard to re-create at our end also.



INSTALLATION
============

(For AROS see below) An installer script is included in the archive, and
should install all required files to the appropriate places. If you are
installing Scalos for the first time, a default configuration is installed, too.

After installation, reboot and test away!

Due to unclear licensing conditions, we can not include the original Amiga
installer in this archive. Especially MorphOS users may not have Installer
available on their machines. They can get it from here:

http://aminet.net/util/misc/Installer-43_3.lha

Here you can find some of the non-standard MUI MCCs required for Scalos Prefs:

http://main.aminet.net/dev/mui/MCC_Urltext.lha
http://main.aminet.net/dev/mui/MCC_Lamp.lha
http://mui.zerohero.se/mcc/MCC_Popxxx.lha
http://jabberwocky.amigaworld.de/downloads.php?id=30


If anything else is missing, do not hesitate to contact us
at Scalos@VFEmail.net !


If you display "%pr" processor information in the screen title bar, you
will need to remove the prepended "68" to the string. For PPC support,
we had to change the string returned by "%pr", so for an 68040 processor,
now "68040" is returned instead of "040".

If you use non-English Scalos catalog, then please make sure you are using
the latest language catalog from the website. Currently, only the French
and German catalogs are continously kept up-to-date.


Installation on MorphOS 2.x machines
------------------------------------
Dragged icons will not display correctly unless both "Allow double
buffered screens" and "Allow triple buffered screen" in the
"Display Engine" preferences are both disabled!


Installation on OS3.0 or OS3.1 machines
---------------------------------------
We strongly recommend that you install the improved ilbm.datatype from
Aminet, or a more recent version:

http://aminet.net/util/dtype/ilbmdt44.lha


Installation on AROS machines
-----------------------------
Scalos exists currently only for i386 versions of AROS with v1 ABI.
Unpack the binary release and execute the included InstallAROS script.



HISTORY
=======

See the separate combined "History" file. It contains both main Scalos, 
prefs programs, and datatype sub-system history.

To find out what has been updated since this release please consult the
version history section of http://news.scalos.noname.fr



LICENSE
=======

The components of Scalos are released under various Open Source licenses.
See file "LEGAL" for details.



The Scalos Team
[Scalos@VFEmail.net]
