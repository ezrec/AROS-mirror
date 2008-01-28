=============================
Руководство по установке AROS
=============================

:Authors:   Stefan Rieken, Matt Parsons, Adam Chodorowski, Neil Cafferkey, Sergey Mineychev
:Copyright: Copyright Љ 1995-2008, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Дополняется. 
:Abstract:
    Данное руководство посвятит вас в необходимые шаги для установки различных версий AROS.
    
    .. Warning:: 
        
        AROS в данный момент имеет статус альфа-версии. Это означает, что в 
        данной стадии она в хороша основном, для экспериментов и разработки. 
        Если вы пришли сюда в надежде, что это законченная и полностью рабочая 
        система, вас ждёт горькое разочарование. AROS далека от этого, хотя 
        медленно и верно двигается к этой цели.
        

.. Contents::


Получение
=========

В настоящее время AROS находится в активной разработке. В результате, Вам предстоит 
выбор между стабильностью и возможностями. В настоящее время существует 2 типа бинарных 
пакетов (и исходников), доступных для скачивания: снимки и ночные сборки.

Снимки (более стабильные и частично тетированные сборки) более недоступны, поскольку
начали слишком быстро устаревать, отставая от развития ОС. Всем желающим рекомендуется
взамен их использовать ночные сборки.

Ночные сборки, соответственно названию, собираются автоматически каждую ночь прямо 
из исходников в репозитории Subversion и содержат все последние изменения. 
Однако, они никоим образом не тестируются, и при особом невезении могут быть нерабочими 
или ужасно глючными. Правда, как правило, они работают совершенно нормально.

Для того, чтобы скачать доступные снимки и ночные сборки, посетите нашу 
`страницу закачек <../../download>`__.


Установка
=========

AROS/i386-linux и AROS/i386-freebsd
-------------------------------------

Требования
""""""""""

Для того, чтобы запустить AROS/i386-linux или AROS/i386-freebsd потребуется следующее:


+ Рабочая инсталляция FreeBSD 5.x или Linux (не имеет особого значения, какой дистрибутив
  вы используете, если он относительно свежий)

+ Настроенный и функционирующий X-сервер (например, X.Org или XFree86) для работы 
  x11.hidd или установленную библиотеку libsdl (для sdl.hidd).

Собственно, всё.

Извлечение
""""""""""

Поскольку AROS/i386-linux и AROS/i386-freebsd - hosted-виды AROS, работающие
на основе других ОС, их установка весьма проста. Просто скачайте соответствующие 
архивы для вашей архитектуры со `страницы закачек <../../download>`__ 
и распакуйте в удобное для вас место::

    > tar -vxjf AROS-<version>-i386-<platform>-system.tar.bz2

Если вы скачали дополнительный архив программ, вы можете распаковать и его::

    > tar -vxjf AROS-<version>-i386-all-contrib.tar.bz2


Запуск
"""""""

После распаковки всех файлов, запустите AROS::

    > cd AROS
    > ./aros -m <количество предоставляемой памяти в МБ, число>


.. Примечание:: 
    
    Если используемый XFree86 версии 3.x или более старой, возможно нарушение 
    нормального обновления окон AROS (например, после того, как они закрываются
    окнами других приложений). Это следствие того, что AROS использует функцию
    "backingstore" X-сервера, по умолчанию выключенную в XFree86 4.0 и более 
    поздних. Чтобы включить эту опцию, добавьте следующую строчку к разделу 
    *Device* видеокарты в файле конфигурации X-сервера (обычно он называется 
    ``/etc/X11/xorg.conf``, ``/etc/X11/XF86Config-4`` или ``/etc/X11/XF86Config``)::
    
        Option "backingstore"

    В результате, секция устройства может выглядеть следующим образом::

        Section "Device"
            Identifier      "Matrox G450"
            Driver          "mga"
            BusID           "PCI:1:0:0"
            Option          "backingstore"
        EndSection


AROS/i386-pc and AROS/X86-64x Native
------------------------------------

Аппаратные требования
"""""""""""""""""""""

AROS достаточно "среднего" PC-AT (на базе i486 или выше) с шиной *PCI*,  мышью
PS/2 или USB, клавиатурой AT,PS/2 или USB, жёстким диском IDE и CDROM на PATA,
видеокартой (S)VGA и соответствующим монитором.

Also most of available PC-compatible VM (virtual machine) can be used. QEMU, 
VmVare (Server/Workstation/Fusion), Q, Bochs, MS VPC are known to work.
At least 24 MB of RAM and VESA-compliant VGA card is recommended. 
There`s a generic accelerated drivers (HIDD`s) for ATI and nVidia cards. 

You can add the PCI networking interface card (like pcnet32 or intel e100) to 
try AROS networking. If in trouble please check the FAQ if it contains any 
information on support of your hardware.

If you want to try sound on AROS, the best choice at this moment must be 
Creative 10k-based sound cards.

X86-64x port has the similar requirements, assuming that 64-bit capable Intel/AMD
CPU is used. Chipset support currently is limited. This port is early, please 
report bugs.


.. Note:: 
    
    Although AROS can be installed to the harddrive, please remember, that 
    installer is still in testing stage. It`s must not remove or wipe any
    partitions if not asked to do so, but this still presents some risk. 
    As the testing goes, the process will become much more stable and robust.
    So please note that you **should not** use install on your working machine, 
    which HD contains precious data! We`re taking no responsibility for any 
    data loss occured during the installation. Any bug reports on installation 
    process will be appreciated!

Установочные носители
"""""""""""""""""""""

The recommended installation media for AROS/i386-pc is CDROM, since we can fit
the whole system onto a single disc (and also all the contributed software).
This also makes the installation easier, since you don't have to go through
hoops transferring the software on several floppies.

Since nobody currently sells AROS on CDROM (or any other media for that matter),
you will need access to a CD burner to create the installation disk yourself.


CDROM
^^^^^

Запись
''''''

Simply download the ISO image from the `download page`_ and burn it to a CD
using your favorite CD burning program. There`s a number of freeware 
cd burning programs for any system, and we can point Windows users to the 
`InfraRecorder <http://infrarecorder.sourceforge.net>`__ - it`s free, 
it`s small and fast, and it`s just Nero-killing simple. 


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


Floppy
^^^^^^

These days floppies can be found useful only to boot if your PC`s BIOS doesn`t 
support booting from CD or on some really obsolete PC`s. But it`s still maitaned.


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
it. There`s also an GUI version called rawwritewin.


Загрузка
''''''''

Simply insert the boot floppy into the drive and reboot the computer. The boot
is fully automatic, and if everything works you should see a nice screen after
a while.

Установка на жёсткий диск
"""""""""""""""""""""""""

Well, note that you have been **WARNED** that HD installation is
in testing stage at the moment and can be **dangerous** to your data, so make 
sure the PC you're using for install does not contain any useful data or have 
a full backup. Using a virtual machine is recommended, as it minimises any 
possible risk and allows AROS to be used and tested on any working machine. 
There are many free VM`s available now, such as VirtualPC, Q, QEMU and VMWare, 
which are known to work with AROS. 

Подготовка
^^^^^^^^^^

First, set up your HD - either real or a virtual drive image - for use.
For a real drive, this may involve plugging it into the machine (always
a good start) and setting it up in the BIOS. For a virtualiser's or
emulator's virtual drive, you probably just need to select an option to
create a new drive image, and set it as one of the virtual PC's boot
devices (the CD drive must be the first boot device during installtion
of AROS however).

The following options are depending on what you want to do.

Установка AROS без других ОС
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The most simple situation is that with AROS alone on the whole disk. Your 
disk can be new or with an unneeded data on it. Select *Wipe disk* to **erase** 
existing data on hard drive. If this step fails, you can use any tool from any 
LiveCD you prefer.

Currently the installation is meant to be made by means of InstallAROS
program, which is located in the *Tools* drawer on your BootCD. Please launch
it by clicking on it`s icon. See it`s launched up and showing you the 
prompt. Then click PROCEED button in InstallAROS window, to get a screen with 
installing options. The option "Only use free space" must be checked. You can 
set the size of new AROS partition if you wish, and add an extra WORK partition
to install programs on it. After you click "Proceed" button, AROS going to 
make partitions and ask you to reboot. After reboot please run InstallAROS again.

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
    

After you make your choice and proceed installer will show the GRUB installation
device and path to GRUB files for you to check it. Proceeding, you will see 
the las screen before installation, that warns you about pre-alpha status of
AROS install process. The last clicking "Proceed" and you will see installer
doing it`s work. You can be asked to select your keyboard type and locale 
settings, then the files are being copied. Please wait a while. 

After the install finishes you can remove AROS Live CD out of CD drive an
reboot into your new installed system.

Установка AROS совместно с Windows(R)/DOS(R)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Installing AROS along with Windows must be an easy task. Generally, you`ll need to 
just follow the installer prompts as shown above to make this working. Installer 
is designed to detect your Windows installation and put it to GRUB menu 
automatically.

 
There can be problems with some older and newer Windows versions (like 95/98 and Vista). 
For Vista you must use steps, similar to the ones for Linux with GRUB installer.
This involves use of some programs like BCD.

TODO more...

Установка AROS совместно с Linux/BSD
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Installing AROS along with Linux or BSD systems is almost the same as the one 
for Windows. You`d need to create a free space for AROS with available tools.
Then use InstallAROS to do the partitioning and formatting the AROS partition
and copy system to it (you can use additional WORK partition if you want to), 
but it`s better to not install the bootloader (uncheck the corresponding checkbox)::

    todo

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

Use root
You can change the kernel`s parameters to set the screen resolution.

If you happen to use lilo or any other booloader, this trick won`t be that easy.
(lilo is too hard to make AROS kernel booting). You will need to somehow 
chainload AROS GRUB and set it to start up a kernel.

After the reboot, you must be able to see AROS entry in GRUB menu and boot it.


Установка AROS совместно с другими ОС
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There`s a lot of another systems out there on the platforms AROS 
supports. IF your system uses GRUB bootloader the process must be fairly similar 
to the one of Linux. If not, please remember that all you need for AROS to boot 
is just to place it`s files on a partition and boot it`s kernel. 

TODO

Ручная установка
^^^^^^^^^^^^^^^^
*(Сведения устарели)*
Поскольку InstallAROS теперь вполне функционален, эта информация является устаревшей, 
однако может представлять некоторый интерес, поэтому она сохранена для 
изучения.
     
(Прежние замечания)::
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

     

Разбиение на разделы
''''''''''''''''''''

*Установка на единственный раздел*

Здесь мы изучим установку AROS как единственной системы на PC в единственный 
раздел. Это самый простой способ.

Нас подстерегает множество неожиданностей, так как процедура еще не полностью 
проработана. Во-первых общее правило для этого процесса - перегрузка после любых 
значительных изменений при создании файловой системы (мы будем отмечать, где 
это необходимо). Перегрузка закрывает окно программы HDToolbox,если оно открыто 
и происходит так называемый hard reset компьютера или виртуальной машины. Вы 
также можете осуществить soft reset набирая <reboot> в окне CLI и нажимая ENTER .

Для начала найдите утилиту на AROS CD под названием HDToolBox, которая находится 
в ящике Tools. На первое время она станет основным "мучителем" Вашего винчестера. 
Запустив её, вы увидите в окне диалог выбора типа устройства для разметки. В 
этом примере (здесь и далее), мы используем реальный или виртуальный IDE диск 
(их также называют ATA). Итак, щелчок по ata.device вход покажет Devices:1 
в окне слева, это число доступных дисков. Щёлкнув по этому пункту, мы увидим  
их список. Если это виртуальный HD, мы можем увидеть, например,QEMU Harddisk 
или что-то похожее для VMWare. Если это настоящий HD, Вы увидите его имя, 
если нет - ещё раз попробуйте произвести подготовку винчестера. Щёлкнув на 
названии диска мы увидим::

    Size: <Размер HD>
    Partition Table: <тип текущей таблицы разделов; может быть неизвестен после очистки>
    Partitions: <число разделов на винте; равно 0, поскольку они ещё не созданы>

Теперь нужно создать новую таблицу разделов, для PC это таблица типа *PC-MBR*. 
Для этого нажмите кнопку *CreateTable* и выберите *PC-MBR* из списка. Щелкните OK.

Настало время внести изменения. Сделайте это, выделив имя Вашего винчестера и 
нажав *Save Changes*. Выберите *Yes* в открывшемся диалоговом окне. Закройте 
окно HDToolbox и перезагрузите систему с Live CD.

После загрузки вновь запустите HDToolbox. Теперь при выборе *ata.device* вы должны 
увидеть информацию "Partition table: PC-MBR, Partitions:0". Время создать разделы,
щелкните по имени HD, чтобы вывести список разделов, пока пустой. Нажмите кнопку 
*Create Entry*, выберите все доступное пространство, щелкнув в свободном месте 
и подтвердите *OK*. Теперь в списке Вы увидите "Partition 0" , выделив её, 
увидим в боковом окне::

    Size: <размер раздела, практически равен емкости HD>
    Partition table: Unknown <Пока не создана>
    Partition type: AROS RDB Partition table <OK>
    Active: No <Не активен>
    Bootable: No <Не загружаем>
    Automount: No <Не монтируется при старте>

Here can be some difference - make a partition in RDB table or as usual PC-MBR
partition. RDB (Rigid Disk Block) is the choice of compatibility and was used 
in Amiga HDD`s partitioning, and we can use it too. Although, AROS supports 
FFS partitions created within a common PC-MBR table, like an usual PC partitions
like FAT/NTFS/etc. Second way can be considered somewhat more modern and more 
compatible to some new AROS programs. Let`s consider both.

*FFS in RDB*

Нажмите кнопку *Create Table* , выберите *RDB table* и щелкните *OK*. Для 
сохранения изменений перейдите уровнем выше, нажав кнопку *Parent*, выберите 
снова имя HD и щелкните кнопку *Save Changes*. Ответьте *Yes* в открывающихся 
диалогах дважды. Закройте HDToolbox и перегрузитесь снова.

*FFS in MBR*
...to be added

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

Форматирование
''''''''''''''

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


Copying the system
''''''''''''''''''

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

Installing the bootloader
'''''''''''''''''''''''''

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


Preparing to boot
'''''''''''''''''

We have just done our first installation alchemy course, and AROS should
be ready now. We must remove the Live CD from the CD drive (or disable
booting from CD in VM) and check it out. Hear the drum roll? ;)

If something goes wrong, there can be some answers...

Troubleshooting
'''''''''''''''

Installation process is the one of most frequently ones asked about on forums,
mostly by newbees. You can check the FAQ if there an answer to your questions.
Any additions ? ...

Installing AROS along with the other systems
''''''''''''''''''''''''''''''''''''''''''''

In the steps described before we had installed AROS as the *only* system on HD.
But can it be installed to multiboot with other systems on HDD? Yes. But 
again this task will be difficult. 

AROS and Windows

Let`s consider the situation when you have only Windows(tm) XP installed and 
want to put AROS to this HDD.
Windows NT systems can be installed on both FAT and NTFS filesystems. 
While NTFS is way more secure and robust, it`s not supported by GRUB (unfortunately)

AROS and Linux (and other OS which uses GRUB bootloader)

Lets consider the situation when you want to have 3 systems on your HDD -
Windows, Linux and AROS.  

Preparing the HDD

To be continued ...

AROS/i386-PPC-hosted
--------------------

Requirements
""""""""""""

To be written by someone.

AROS/m68k-backport aka AfA
--------------------------

This is not usual native/hosted flavour of AROS, but a thing that can be called
a *backport*. Actually, it`s a set of libraries and binaries to enhance 
the capabilities of original AmigaOS. AfA stands for AROS for Amigas. 

Requirements
""""""""""""

To be written by someone.

Installation
""""""""""""

Installation:

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

