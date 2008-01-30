=============================
����������� �� ��������� AROS
=============================

:Authors:   Stefan Rieken, Matt Parsons, Adam Chodorowski, Neil Cafferkey, Sergey Mineychev
:Copyright: Copyright � 1995-2008, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    �����������. 
:Abstract:
    ������ ����������� �������� ��� � ����������� ���� ��� ��������� ��������� ������ AROS.
    
    .. Warning:: 
        
        AROS � ������ ������ ����� ������ �����-������. ��� ��������, ��� � 
        ������ ������ ��� � ������ ��������, ��� ������������� � ����������. 
        ���� �� ������ ���� � �������, ��� ��� ����������� � ��������� ������� 
        �������, ��� ��� ������� �������������. AROS ������ �� �����, ���� 
        �������� � ����� ��������� � ���� ����.
        

.. Contents::


���������
=========

� ��������� ����� AROS ��������� � �������� ����������. � ����������, ��� ��������� 
����� ����� ������������� � �������������. � ��������� ����� ���������� 2 ���� �������� 
������� (� ����������), ��������� ��� ����������: ������ � ������ ������.

������ (����� ���������� � �������� ������������ ������) ����� ����������, ���������
������ ������� ������ ����������, �������� �� �������� ��. ���� �������� �������������
������ �� ������������ ������ ������.

������ ������, �������������� ��������, ���������� ������������� ������ ���� ����� 
�� ���������� � ����������� Subversion � �������� ��� ��������� ���������. 
������, ��� ������ ������� �� �����������, � ��� ������ ��������� ����� ���� ���������� 
��� ������ ��������. ������, ��� �������, ��� �������� ���������� ���������.

��� ����, ����� ������� ��������� ������ � ������ ������, �������� ���� 
`�������� ������� <../../download>`__.


���������
=========

AROS/i386-linux � AROS/i386-freebsd
-------------------------------------

����������
""""""""""

��� ����, ����� ��������� AROS/i386-linux ��� AROS/i386-freebsd ����������� ���������:


+ ������� ����������� FreeBSD 5.x ��� Linux (�� ����� ������� ��������, ����� �����������
  �� �����������, ���� �� ������������ ������)

+ ����������� � ��������������� X-������ (��������, X.Org ��� XFree86) ��� ������ 
  x11.hidd ��� ������������� ���������� libsdl (��� sdl.hidd).

����������, ��.

����������
""""""""""

��������� AROS/i386-linux � AROS/i386-freebsd - hosted-���� AROS, ����������
�� ������ ������ ��, �� ��������� ������ ������. ������ �������� ��������������� 
������ ��� ����� ����������� �� `�������� ������� <../../download>`__ 
� ���������� � ������� ��� ��� �����::

    > tar -vxjf AROS-<version>-i386-<platform>-system.tar.bz2

���� �� ������� �������������� ����� ��������, �� ������ ����������� � ���::

    > tar -vxjf AROS-<version>-i386-all-contrib.tar.bz2


������
"""""""

����� ���������� ���� ������, ��������� AROS::

    > cd AROS
    > ./aros -m <���������� ��������������� ������ � ��, �����>


.. ����������:: 
    
    ���� ������������ XFree86 ������ 3.x ��� ����� ������, �������� ��������� 
    ����������� ���������� ���� AROS (��������, ����� ����, ��� ��� �����������
    ������ ������ ����������). ��� ��������� ����, ��� AROS ���������� �������
    "backingstore" X-�������, �� ��������� ����������� � XFree86 4.0 � ����� 
    �������. ����� �������� ��� �����, �������� ��������� ������� � ������� 
    *Device* ���������� � ����� ������������ X-������� (������ �� ���������� 
    ``/etc/X11/xorg.conf``, ``/etc/X11/XF86Config-4`` ��� ``/etc/X11/XF86Config``)::
    
        Option "backingstore"

    � ����������, ������ ��������� ����� ��������� ��������� �������::

        Section "Device"
            Identifier      "Matrox G450"
            Driver          "mga"
            BusID           "PCI:1:0:0"
            Option          "backingstore"
        EndSection


AROS/i386-pc � AROS/X86-64x Native
----------------------------------

.. ����������:: 
    
    �������� �� ��, ��� AROS ����� ���� ����������� �� ������ ����, ������, ���
    ������� ��������� �� ������ ������ �������� �������������. ������� �� ������
    ������������ ������� ����������, ���� �� ���� �������, �� ��������� ����
    �� ����� ������������.  �� �������� � �������� ������������, ��������� ������
    ����� ���������� � �������. ����� �������, �������, ��� **�� �����** 
    ������������ ��������� �� ����������, ������ ����� ������� �������� ������
    ������. �� �� ����� ����� ��������������� �� ����� ������, ���� ��� 
    ��������� � �������� ���������, �� � ������� ��������� ��������� � �����
    ����� ���������� �� �������!
    
���������� ����������
"""""""""""""""""""""

AROS ���������� "��������" PC-AT (�� ���� i486 ��� ����) � ����� *PCI*,  �����
PS/2 ��� USB, ����������� AT,PS/2 ��� USB, ������ ������ IDE � CDROM �� PATA,
����������� (S)VGA � ��������������� ���������. ��� ������� ����������� ��� 
������� 24 MB ����������� ������. ������������� ������������� VESA-�����������
���������� VGA. � ������ �� ������ ����� �������� � 2D-���������� ��� ���� �� 
ATI � nVidia.

����� ����� ���� ������������ ����������� �� ��������� ����������� ����� (VM).
���������� ��������, ��� � AROS ��������: QEMU, VmVare (Server/Workstation/Fusion), 
Q, Bochs, MS VPC. ���������� ����������� AROS ��� ��������� 
`VmwAROS <http://vmwaros.org>`_, ������� �������� AROS � ��� �������, 
����������������� � ����������� ������ VmWare.

����� ���������� ����������� AROS � ������ � ������ ����� ������������ 
������� ����� � ����������� PCI (��������, pcnet32 ��� intel e100). ����
�� ����������� � ����������� ��� ��������� ����, ������� � FAQ ���������� �
��������� ������ ����������.

����� �������� �� ������ � AROS, ������ ������� �� ������� ������ ��������
����� �� ���� ����� Creative ����� 10k.

���� X86-64x ����� ������ ���������� � ��������� �� ��, ��� ��������������
64-������ ���������� Intel/AMD � ��������������� �������. ��������� ��������
�� ������ ������ ����������. ���� ������� �� ���� �������� �����, � ���
������� ���������� ������������.

������������ ��������
"""""""""""""""""""""

��������������� ��������� ��� ��������� AROS �������� CDROM, �� ���� ���� 
������� ��������� � �������, � ��������� ��� ��. ��� ����� ������ �������� 
������� ���������, ��������� ������ ��� ������������� ���������� �������
�� ��������� ��������� ������.

� ��������� ����� ����� � AROS �� ���������, �������������, ��� ����������� 
������ � �������� CD �������, ����� �������� ���� ��������������.

CDROM
^^^^^

������
''''''

������ �������� ISO-����� LiveCD � AROS �� `�������� ������� <../../download>`__ 
� �������� ��� �� CD-���� � ������� ����� ��������� ��������� ������. ���������� 
��������� ��������� �������� ��� ������ ������, ������������� Windows �� ����� 
������������ `InfraRecorder <http://infrarecorder.sourceforge.net>`__ - �������,
��������� � ������� ��������� ��������� ��� ������ ������. ����� ���� � ������
��������� ��� ������ ������, �������� CDBurnerXP, DeepBurn, AstroBurn ��� Windows,
k3B, Brasero ��� Linux. ��� Amiga � AROS ���� ��������� FryingPan.

��������
''''''''

����� ����� ��������� AROS � CD, ���� ��� ��������� ������������ �������� � CD.
����� ������������� �������� �������� � CD � BIOS, ��������� �� ������ �����������
��� ��������� �� ���������. ������ �������� ���� � AROS � ������ ������ CD 
� ������������� ���������. �������� ��������� ���������������� �, ���� �� 
� �������, ������ �� ������� ����� ��.

���� �������� � CD �� �������������� �� ����� PC, ����� ������� ����������� 
������� � ������������ � ��� �������� � CD. ���������� ������ �������� ��������
��������������� ���������� � ���������������. �������� �������� � �������,
� ����� ����, ��� ��������� ������ ����� (��������, ������� �������� ������� CDROM) 
����� ���������, �������� ����������� � CD.

�������
^^^^^^^

� ��������� ����� ������� ������ ������� ������ ��� �������� �� PC, 
BIOS �������� �� ������������ �������� � CD, ������ �� ������ ��-��������
��������.

������
''''''

����� ������� ����������� �������, ��� ����������� ������� ����� ������� ��
`�������� ������� <../../download>`__, ����������� ����� � �������� ����� �� 
�������. � ������ ������������� �� ��������� UNIX (��������, Linux ��� FreeBSD), 
��� ����� ������� ��������� �������::

    > cd AROS-<version>-i386-pc-boot-floppy
    > dd if=aros.bin of=/dev/fd0

���� �� ��������� � Windows, ��� ������ ������ �� ������� ��� ����������� 
��������� rawrite_. ����������, �������� � ������������ �� �������������. 
����� ���������� ������ � ����������� ����������� - rawwritewin.

��������
''''''''
�������� ������� � �������� � ������������� ���������. �������� ���������
����������������, � ���� �� � �������, ������ �� ������� ������� ���� AROS.

��������� �� ������ ����
"""""""""""""""""""""""""

����� �� ��� ������������� ���, ��� ��������� AROS ��������� � ������ 
*������������* � ����� ���� **�������** ��� ����� ������, ������� 
���������� ��������� � �� ���������� ��� ������� ������ ��������� �����.
������������� ������������ �������������, ��������� ��� ����������� ���������
� ��������� ������������ AROS ����������� �� ����� ����������.  

����������
^^^^^^^^^^

������, ����������� ��� ������ ���� (�������� ��� �����������) � �������������.
��������� ���� ����� ����������� ��������� ���������, ����������� � ����������
� ��������� � BIOS. ��� ������������ �����, ������ �����, ����������� ���� 
������� ����� ����� �����, � ������� ��� ��� ���� �� ��������� ��������� ��������
����������� ������ (������ CD-������ �� �� ������ ���� ������ � ���� ������
�� ����, ��� �� ���������� �������).

���������� �������� ������� �� ����� ������������.

��������� AROS ��� ������ ��
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

�������� ������� ������ ����� �����, ���� ���������� ���������� ������ AROS
�� ���� ������ ����, ����� ��� � ������ ��������� �������. ����� �� ������ 
������������ ��� AROS �������������� ������ ����.

� ��������� �����, ��������� ������ ������������� ��� ������ �����������
InstallAROS, �������������� � ���������� *Tools* �� LiveCD. ��������� �
������� ������� �� � ������. ����� ��� ���������� � ������� ��� ���������������
����, ������� ������ ``Proceed`` ��� �������� � ���� � ���������� ���������. 

����� �� ������ ������� ��� �������� ���������� ��� ��������� (ata.device) � ���
����� (0). ���� �� ������ ���������� ��������� �� �������������� ����, ����� 
������� ��������. ������ ����������� ����� ����� � ������� ������� */Tools/HDToolbox*.
���� �� ������ ��������� ������� �� ����� ��� ���� � ���� ������������ ��������� 
�� ��� ��������� �� �������� �����, �������� ������� ``Only use free space``,
���, ���� ���� ������� **��������**, �� �������� ``Wipe disk``. �� ����� ������ 
������ ������ ������������ �������, � �������� �������� ��������������� ������� 
WORK ��� ��������� ��������. ����� ������� ������ "Proceed", ���������� AROS
������� ������� � ������ ������ �� ������������. ����� �������� ������� �����
��������� InstallAROS.

*���� �� ����������, ����� ����������*

Now you must see the option "Use existing AROS partitions" is selected. Proceed 
with this. You will see some extra options (defaults shown) in a window::

    [ ] Choose language Options
    [x] Install AROS Core System
    [x] Install Extra Software
    [ ] Install Development Software
    [x] Install Bootloader
    
The first, "Choose language Options" allow you to select locale of your new
installed system (by launching /Extras/Locale program). "Install AROS Core System"
allows installing of all AROS base programs, needed for OS to work. 
"Install Extra Software" allows installing an additional programs (located on
in /Extras drawer and, if selected, on WORK partition). "Install Development Software"
allows installing of some development software like some programming languages.
"Install Bootloader: enables installing of GRUB bootloader to MBR of HD (there
can be some situations when you don`t need to install it). Make your choice and
click "Proceed" button. 

On the next installer screen you can choose which partitions you want to format,
and to copy files on, whether WORK partition is used to copy files on it::

    Destination Partition     [x] Format Partition
    DH0
    
    [ ] Use 'WORK' Partition
    [ ] Copy Extras and Development Files to Work
    
    Work Partition            [ ] Format Partition
    DH1
    

After you made your choice and proceed installer will show the GRUB installation
device and path to GRUB files for you to check it. Proceeding, you will see 
the las screen before installation, that warns you about pre-alpha status of
AROS install process. The last clicking "Proceed" and you will see installer
doing it`s work. You can be asked to select your keyboard type and locale 
settings, then the files are being copied. Please wait a while. 

After the install finishes you can remove AROS Live CD out of CD drive an
reboot into your new installed system.


��������� AROS ��������� � Windows(R)/DOS(R)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

��������� AROS � Windows �������� ������. � ����� � �����, ���������� ���� 
��������� ���������� ����������� (��� ���� ������� �����). ���������� 
�������� ���������� ������� ��������� Windows �� ����� � � ���� ������ 
������������� ������� Windows � ������������ ���� GRUB.
 
There can be problems with some older and newer Windows versions (like 95/98 and Vista). 
For Vista you must use steps, similar to the ones for Linux with GRUB installer.
This involves use of some programs like BCD.

TODO more...

��������� AROS ��������� � Linux/BSD
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Installing AROS along with Linux or BSD systems is almost the same as the one 
for Windows. You`d need to create a free space for AROS with available tools.
Then use InstallAROS to do the partitioning and formatting the AROS partition
and copy system to it (you can use additional WORK partition if you want to), 
but it`s better to not install the bootloader (uncheck the corresponding checkbox)::

    [ ] Install Bootloader

After the installer has finished copying the files, it will ask you to reboot.
After the reboot you`d need to boot your Linux/BSD again, to set up the 
bootloader.
AROS uses patched GRUB bootloader, able to load kernel from AFFS. But you don`t
have to use it, if you put AROS kernel to the place, where your system kernel is 
(usually /boot) and use a conventional GRUB from your distribution. Just copy 
/boot/aros-i386.gz from AROS LiveCD to /boot. Then put some new lines to the end 
of /boot/grub/menu.lst file to enable AROS menu entry::

    title AROS VBE  640x480  16bpp
    root (hd0,0)
    kernel /boot/aros-pc-i386.gz vesa=640x480x16 ATA=32bit,forcedma nofdc
    quiet
    boot

You can change the kernel`s parameters to set the screen resolution. The 'nofdc'
option disables the floppy trackdisk device, which is not too useful these days
but can cause troubles in some cases.

If you happen to use lilo or any other booloader, this trick won`t be that easy.
(is hard to make lilo booting AROS kernel). You will need to somehow 
chainload AROS GRUB and set it to start up a kernel.

After the reboot, you must be able to see AROS entry in GRUB menu and boot it.


��������� AROS ��������� � ������� ��
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There`s a lot of another systems out there on the platforms AROS 
supports. IF your system uses GRUB bootloader the process must be fairly similar 
to the one of Linux. If not, please remember that all you need for AROS to boot 
is just to place it`s files on a partition and boot it`s kernel. 

TODO

������ ���������
^^^^^^^^^^^^^^^^
*(�������� ��������)*

��������� InstallAROS ������ ������ ������������, ��� ���������� �������� ����������, 
������ ����� ������������ ��������� �������, ������� ��� ��������� ��� 
��������.
     
(������� ���������)::
    We currently do not recommend installation of AROS/i386-pc onto a harddrive
    [#]. But you definitely would need to install AROS to test some of 
    it`s features and workarounds must be advised. Please note that you **should 
    not** use install on your working machine, which HD contains precious data!
    We`re taking no responsibility for any data loss occured during the installation. 
    Any bug reports on installation is welcome.
    
    [#]It *is* actually possible to install AROS/i386-pc onto a harddrive, but
    the procedure is far from being automated and user-friendly and the
    necessary tools are still being heavily developed and might be quite
    buggy. Therefore we officially do not recommend harddisk installation for
    unexperienced users at the moment so this note was written. 

     

**��������� �� �������**

*��������� �� ������������ ������*

����� �� ������ ��������� AROS ��� ������������ ������� �� PC � ������������ 
������. ��� ����� ������� ������.

��� ������������ ��������� ��������������, ��� ��� ��������� ��� �� ��������� 
�����������. ��-������ ����� ������� ��� ����� �������� - ���������� ����� ����� 
������������ ��������� ��� �������� �������� ������� (�� ����� ��������, ��� 
��� ����������). ���������� ��������� ���� ��������� HDToolbox,���� ��� ������� 
� ���������� ��� ���������� hard reset ���������� ��� ����������� ������. �� 
����� ������ ����������� soft reset ������� <reboot> � ���� CLI � ������� ENTER .

��� ������ ������� ������� �� AROS CD ��� ��������� HDToolBox, ������� ��������� 
� ����� Tools. �� ������ ����� ��� ������ �������� "���������" ������ ����������. 
�������� �, �� ������� � ���� ������ ������ ���� ���������� ��� ��������. � 
���� ������� (����� � �����), �� ���������� �������� ��� ����������� IDE ���� 
(�� ����� �������� ATA). ����, ������ �� ata.device ���� ������� Devices:1 
� ���� �����, ��� ����� ��������� ������. ٸ����� �� ����� ������, �� ������  
�� ������. ���� ��� ����������� HD, �� ����� �������, ��������,QEMU Harddisk 
��� ���-�� ������� ��� VMWare. ���� ��� ��������� HD, �� ������� ��� ���, 
���� ��� - ��� ��� ���������� ���������� ���������� ����������. ٸ����� �� 
�������� ����� �� ������::

    Size: <������ HD>
    Partition Table: <��� ������� ������� ��������; ����� ���� ���������� ����� �������>
    Partitions: <����� �������� �� �����; ����� 0, ��������� ��� ��� �� �������>

������ ����� ������� ����� ������� ��������, ��� PC ��� ������� ���� *PC-MBR*. 
��� ����� ������� ������ *CreateTable* � �������� *PC-MBR* �� ������. �������� OK.

������� ����� ������ ���������. �������� ���, ������� ��� ������ ���������� � 
����� *Save Changes*. �������� *Yes* � ����������� ���������� ����. �������� 
���� HDToolbox � ������������� ������� � Live CD.

����� �������� ����� ��������� HDToolbox. ������ ��� ������ *ata.device* �� ������ 
������� ���������� "Partition table: PC-MBR, Partitions:0". ����� ������� �������,
�������� �� ����� HD, ����� ������� ������ ��������, ���� ������. ������� ������ 
*Create Entry*, �������� ��� ��������� ������������, ������� � ��������� ����� 
� ����������� *OK*. ������ � ������ �� ������� "Partition 0" , ������� �, 
������ � ������� ����::

    Size: <������ �������, ����������� ����� ������� HD>
    Partition table: Unknown <���� �� �������>
    Partition type: AROS RDB Partition table <OK>
    Active: No <�� �������>
    Bootable: No <�� ���������>
    Automount: No <�� ����������� ��� ������>


�������� ��������� ��������� - make a partition in RDB table or as usual PC-MBR
partition. RDB (Rigid Disk Block) is the choice of compatibility and was used 
in Amiga HDD`s partitioning, and we can use it too. Although, AROS supports 
FFS partitions created within a common PC-MBR table, like an usual PC partitions
like FAT/NTFS/etc. Second way can be considered somewhat more modern and more 
compatible to some new AROS programs. Let`s consider both.

*FFS in RDB*

������� ������ *Create Table* , �������� *RDB table* � �������� *OK*. ��� 
���������� ��������� ��������� ������� ����, ����� ������ *Parent*, �������� 
����� ��� HD � �������� ������ *Save Changes*. �������� *Yes* � ������������� 
�������� ������. �������� HDToolbox � ������������� �����.

*FFS in MBR*
����� �������� �������� �������� FFS � ������� ������� �������� PC-MBR, ���
��� ����� ���� �� ���������� ���������. 


After booting up, run HDToolbox (you`ve guessed that). Now the info for our
Partition 0 is the same except that the partition table is now RDB (or not). This
partition must be set to Active. To do this, click on the *Switches*
button, select the *Active* checkbox and click *OK*. Now what? Yes, save the
changes by going a level up and clicking the button. Exit and reboot.

Why are we rebooting so much? Well, HDToolbox and system libraries are
still unfinished and quite buggy, so rebooting after every step helps to
reset them to initial state.

After boot up, HDToolbox must show us that Partition 0 has become
active. That's good, now we must create our disk to install AROS on. Go
one level down by clicking on the "Partition 0" entry. Now what? Yes,
click the Add Entry button and choose all the empty space. Now you see a
"DH0" entry there, which is our disk. Clicking on it shows information::

    Size: <well...>
    Partition Table: Unknown (it's OK)
    Partition Type: Fast Filesystem Intl <OK>
    Active: No <OK>
    Bootable: No <we must switch it to Yes>
    Automount: No <we must switch it to Yes>

Now, go *2 levels up* to the HD name, click Save Changes, confirm, exit
and reboot. After booting up (pretty boring, isn't it?), what should we
do? Yes, we must set switches to the DH0 drive in HDToolbox. We go to
the DH0 entry and set switches with the relevant button and checkboxes:
*Bootable: Yes* and *Automount: Yes*. Save changes after going 2 levels
up again, confirm and reboot.

*How long is it left to go?* Well, we're more than half way to success.
After booting up and checking all the settings for DH0, we must see it's
OK now. So now we can exit HDToolbox with no hesitation left. Now it's
time for some CLI magic.

**��������������**


We must format our created DH0 drive to make it usable. Currently AROS have
a choice of two filesystems - Fast FileSystem(FFS) and Smart FileSystem(SFS). 
FFS is known to be somewhat more stable and compatible to most programs,
SFS is more fail-proof and advanced, but yet have some issues with some programs.
Currently we must set it to FFS, because GRUB bootloader is not supporting SFS
(GRUB2 will). Also please note that you can get problems using some ported 
software with SFS (such as gcc). So now open the CLI window (right click on 
upper menu and select Shell from the first Wanderer menu). At the prompt, enter 
the Info command (type ``info`` and press Enter). You should see our DH0 in 
the list as ``DH0: Not a valid DOS disk``. Now we will format it with the 
command::

    >format DRIVE=DH0: NAME=AROS FFS INTL
    About to format drive DH0:. This will destroy all data on the drive. Are 
    you sure ? (y/N)

Enter y, press Enter and wait a second. You should see the string
``Formatting...done`` displayed. If you got an error, check for all
partition parameters in HDToolbox, as you may be missing something, and
repeat.

If you`re experiencing problems with format (such as ERROR messages, especially
when using partitions in RDB), which is unlikely, then you can try a good old 
Amiga FORMAT64 utility::

    >extras/aminet/format64 DRIVE DH0: Name AROS FFS INTL


Now the Info command should show::

    >DH0: <size>  <used> <free> <full 0%> <errors> <r/w state> <FFS> <AROS>

That's it. Time for the pre-installation reboot.

.. Note:: If this all seems to be so boring that you can't stand it, there's 
          some relief if you intend to use AROS only in virtual machine. 
          First, you can get a pre-installed pack, such as *WinAROS/WinAROS
          Lite* - this system is already installed, but can be outdated. Second, 
          you can look at `AROS Archives`_ for *Installation Kit* that contains 
          ready-made virtual HD's that are already made and ready for install,
          so you can skip the previous procedure and install a fresh
          version of AROS. 


**����������� �������**


After reboot, you may notice that you can see our AROS HD on the desktop
now, and it's empty. Now we need to fill it with files.

Now after the Drag`n`Drop support developed in AROS the whole system can be 
easily copyed from LiveCD by just dragging files to DH0: drawer. It`s only
left to replace the file dh0:boot/grub/menu.lst with dh0:boot/grub/menu_dh.lst.DH0
then.

There's an installer in AROS, as incomplete as HDToolbox is, but it can
be used. At least, we can try. So, here's the first way to install.

1. Run *InstallAROS* in the Tools drawer. You will see the welcome screen
telling you the same I did - we're using the alpha version. Let's
get juice out of it ;) There's a *Proceed* button for you to click. Next,
you will see the AROS Public License, and you should accept it to go
further. Now you will see the install options window (if it`s said No,
just *uncheck* the relevant box) ::

    Show Partitioning Options...    []
        <No. As we've done that already>
    Format Partitions               []
        <No. We have done that already>
    Choose Language Options         []
        <No. It's better to do that later>
    Install AROS Core System        [V]
        <Yes, we need it. We're here to do that>
    Install Extra Software [V] 
        <Yes. Uncheck only if you want a lite installation>
    Install Development Software    []
        <No. This is mostly a placeholder at a moment>
    Show Bootloader Options         [V]
        <Yes, bootloader will not be installed otherwise>

Let me note that *Show Partitioning Options* can be unselectable and greyed out
in case if installer is unable to find any suitable partition.
After you've selected or unselected everything we need, click *Proceed*.
The next window shows us possible installation destinations::

    Destination Drive
    [default:DH0]
    
    DH0  <that's correct>
    
    Use 'Work' Partition                        [] 
        <uncheck it, we're installing all-on-one>
    Copy Extras and Developer Files to Work?    [] 
        <same as above>
    Work drive ... <skipped>
    
Now after we uncheck it, click *Proceed*. The window showing bootloader
options appears. Here we only can check, if GRUB, the *GRand Unified Bootloader*,
is to be installed to DH0 and on which device. Click *Proceed* again.

Now the window says we're ready to install. Click *Proceed* once again. Do
you like this pretty button? ;)

After that, the copying progress bar will appear as files are copied. Wait a
while until the process finishes. After that, you will get the finishing
screen and *Reboot* checkbox. Leave this checked and click Proceed. No,
that isn't all yet - wait till the last step remaining. Now our machine
will reboot with the same settings as before, from Live CD.

**��������� ����������**


Now we still see our AROS disk, and all files are there. Haven`t we
already installed the bootloader in previous steps, what are we
missing? 
Well, if you use fresh nightly build then 'GRUB <http://en.wikipedia.org/wiki/GRUB>'__ 
must be already installed
and working, you can skip this step. If not, please read.

For older versions (before nov. 2006)
there were a bug in GRUB, preventing it from installing correctly from the
first try. So if you can`t boot now and get messages like GRUB GRUB FRUB etc
please read the following.
The reinstallation on the second try usually helps to solve it. So, now we
need InstallAROS once again. Repeat all the previous steps from point 1,
but uncheck every checkbox. After the last click on Proceed, GRUB will
be reinstalled, and a window will appear asking you to confirm that
write. Answer yes as many times as needed. Now, on the last page,
uncheck the Reboot checkbox, close the Install program and power off the
machine. 

Alternatively, GRUB can be installed from shell with this command::

    c:install-i386-pc device ata.device unit 0 PN <pn> grub dh0:boot/grub kernel dh0:boot/aros-i386.gz

where PN <pn> (or PARTITIONNUMBER <pn>) is the number of partition where GRUB 
bootloader will be installed.


**������������� � �������**


We have just done our first installation alchemy course, and AROS should
be ready now. We must remove the Live CD from the CD drive (or disable
booting from CD in VM) and check it out. Hear the drum roll? ;)

If something goes wrong, there can be some answers...

**���������� �������**


Installation process is the one of most frequently ones asked about on forums,
mostly by newbees. You can check the FAQ if there an answer to your questions.
Any additions ? ...




AROS-PPC-linux
--------------

����������
""""""""""

To be written by someone.

���������
"""""""""

To be written by someone.

AROS/m68k-backport (AfA)
------------------------

��� �� ������ ������� ��� AROS, � ������ �����, ��� ������� �������� *���������*,
�.�. ����� ��������� � ����������� ������ ��� ���������� ������������ 
������������ AmigaOS. AfA �������� AROS for Amigas (AROS ��� �����). AfA
��������������� Bernd Roesch � ����� ���� ������� 
`����� <http://amidevcpp.amiga-world.de/afa_binarie_upload.php>`_.

����������
""""""""""

To be written by someone.

���������
"""""""""

+ copy the directory AfA_OS_Libs to your sys: Amigadrive Bootpartition.
  If you dont like it here you can copy it elsewhere and assign AfA_OS: 
  to the directory where the AfA_OS_Libs are located
  copy Libs:freetype2.library in your sys:libs directory
+ copy C:AfA_OS_Loader to your sys:fonts directory
+ copy Fonts: to your sys:fonts directory. If you want to have more 
  fonts, use the Fonts from AROS or MOS
+ copy prefs: to your sys:prefs directory

To start it on boot time, insert AfA_OS_Loader in your S:startup-sequence, 
short before IPrefs. It must be inserted after patching tools like MCP 
or picasso96/cgx, because they patch the AfA_OS Functions back.

If you start it with the parameter MOUSESTART (must uppercase written), 
you must hold the left mousebutton during boot time to load the modules, 
instead of skipping them.

To see if all works well, start the "TextBench" program found in this archive.
TTF antialiased speed is currently not a text render winner, it is not optimized,
see aatext source code, but I hope it is fast enough to be usable even on a 
060/50MHz.


Footnotes
=========

.. _`download page`: ../../download

.. _rawrite: http://uranus.it.swin.edu.au/~jn/linux/rawwrite.htm

.. _`AROS Archives`: http://archives.aros-exec.org

