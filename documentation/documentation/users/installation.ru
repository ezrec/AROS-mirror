=============================
Руководство по установке AROS
=============================

:Authors:   Stefan Rieken, Matt Parsons, Adam Chodorowski, Neil Cafferkey
:Copyright: Copyright Љ 1995-2004, The AROS Development Team
:Version:   $Revision: 24430 $
:Date:      $Date: 2006-05-08 04:01:22 +0500 (а†аН, 08 аМаАаЙ 2006) $
:Status:    Done. 
:Abstract:
    Данное руководство посвятит вас в необходимые шаги для установки AROS.
    
    .. Warning:: 
        
        AROS в данный момент имеет статус альфа-версии. Это означает, что в 
        данной стадии она в хороша основном, для экспериментов и разработки. 
        Если вы пришли сюда в надежде, что это законченная и полностью рабочая 
        система, вас ждёт горькое разочарование. AROS далека от этого, но 
        медленно и верно двигается к этой цели.
        

.. Contents::


Скачивание
==========

В настоящее время AROS находится в активной разработке. В результате, Вам предстоит выбор между стабильностью и возможностями. В настоящее время существует 2 типа бинарных пакетов (и исходников), доступных для скачивания: снимки и ночные сборки.

Snapshots are done manually quite infrequently, mostly when there have been
a larger amount of useful changes done to AROS since the last snapshot and
someone feels motivated to create a new snapshot. In short, there is currently
no regular release schedule. Even though they are made infrequently and that we
try to pick times when AROS is particularly stable, there is no guarantee they
will be bug-free or work on your particular machine. That said, we try to
test snapshots on a wider variety of machines, so in practice they should work
relatively well.

Nightly builds are done, as the name implies, automatically every night directly
from the Subversion tree and contain the latest code. However, they have not
been tested in any way and can be horribly broken, extremely buggy and may even
destroy your system if you're very unlucky. Most of the time though, they work
fine.

Please see the `download page`_ for more information on which snapshots and
nightly builds are available and how to download them.


Установка
=========

AROS/i386-linux and AROS/i386-freebsd
-------------------------------------

Требования
""""""""""

Для запуска AROS/i386-linux или AROS/i386-freebsd вам потребуются:

+ Рабочая FreeBSD 5.x или Linux - инсталяция (не имеет особого значения,
  с каким дистрибутивом вы работаете, по крайней мере, если он достаточно 
  известен). 
+ Настроенный и рабочий сервер X (например, X.Org или XFree86).

Собственно, всё.


Распаковка
""""""""""

Since AROS/i386-linux and AROS/i386-freebsd are hosted flavors of AROS,
installation is simple. Simply get the appropriate archives for your platform
from the `download page`_ and extract them where you want them::

    > tar -vxjf AROS-<version>-i386-<platform>-system.tar.bz2

If you downloaded the contrib archive, you may want to extract it too::

    > tar -vxjf AROS-<version>-i386-all-contrib.tar.bz2


Запуск
""""""

After having extracted all files you can launch AROS like this::

    > cd AROS
    > ./aros


.. Note:: 
    
    Unless you are running XFree86 3.x or earlier, you may notice that the
    AROS window does not refresh properly (for example when a different window
    passes over it). This is due to the fact that AROS uses the "backingstore"
    functionality of X, which is turned off by default in XFree86 4.0 and later.
    To turn it on, add the following line to the device section of your
    graphics card in the X configuration file (commonly named
    ``/etc/X11/xorg.conf``, ``/etc/X11/XF86Config-4`` or
    ``/etc/X11/XF86Config``)::

        Option "backingstore"

    A complete device section might then look like this::

        Section "Device"
            Identifier      "Matrox G450"
            Driver          "mga"
            BusID           "PCI:1:0:0"
            Option          "backingstore"
        EndSection


AROS/i386-pc
------------

.. Note:: 
    
    We currently do not support installation of AROS/i386-pc on to a harddrive
    [#]_, so therefore this chapter will only tell you how to create the
    installation media and boot from it. 


Носитель для установки
""""""""""""""""""""""

The recommended installation media for AROS/i386-pc is CDROM, since we can fit
the whole system onto a single disc (and also all the contributed software).
This also makes the installation easier, since you don't have to go through
hoops transferring the software on several floppies.

Since nobody currently sells AROS on CDROM (or any other media for that matter),
you will need access to a CD burner to create the installation disk yourself.


CDROM
^^^^^

Запись
'''''''

Simply download the ISO image from the `download page`_ and burn it to a CD
using your favorite CD burning program. 


Загрузка
''''''''

The easiest way to boot from the AROS installation CD is if you have a computer
that supports booting from CDROM. It might require some fiddling in the BIOS
setup to enable booting from CDROM, as it is quite often disabled by default.
Simply insert the CD into the first CDROM drive and reboot the computer. The
boot is fully automatic, and if everything works you should see a nice
screen after a little while. 

If your computer does not support booting directly from CDROM you can create
a boot floppy_ and use it together with the CDROM. Simply insert both the
boot floppy and the CD into their respective drives and reboot. AROS will start
booting from the floppy, but after the most important things have been loaded
(including the CDROM filesystem handler) it will continue booting from the
CDROM.


Дискета
^^^^^^^

Запись
''''''

To create the boot floppy, you will need to download the disk image from
the `download page`_, extract the archive, and write the boot image to a floppy
disk. If you are using a UNIX-like operating system (such as Linux or FreeBSD), 
you can do this with the following command::

    > cd AROS-<version>-i386-pc-boot-floppy
    > dd if=aros.bin of=/dev/fd0

If you are using Windows, you will need to get rawrite_ to write the image to
a floppy. Please see the documentation of rawrite_ for information on how to use
it.


Загрузка
'''''''''

Simply insert the boot floppy into the drive and reboot the computer. The boot
is fully automatic, and if everything works you should see a nice screen after
a while.


Заметки
=======

.. [#] It *is* actually possible to install AROS/i386-pc onto a harddrive, but
       the procedure is far from being automated and user-friendly and the
       necessary tools are still being heavily developed and might be quite
       buggy. Therefore we officially do not support harddisk installation for
       the moment.


.. _`download page`: ../../download

.. _rawrite: http://uranus.it.swin.edu.au/~jn/linux/rawwrite.htm

