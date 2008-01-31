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

В настоящее время AROS находится в активной разработке. Это привело к тому, что
*cнимки* (более стабильные и частично теcтированные сборки) более недоступны, поскольку
начали слишком быстро устаревать, отставая от развития ОС. Вместо них рекомендуется
использовать ночные сборки.

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
на основе других ОС, их установка весьма проста. Просто скачайте соответствующий 
архив для вашей архитектуры со `страницы закачек <../../download>`__ 
и распакуйте в удобный для вас каталог::

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

    В результате, секция устройств может выглядеть следующим образом::

        Section "Device"
            Identifier      "Matrox G450"
            Driver          "mga"
            BusID           "PCI:1:0:0"
            Option          "backingstore"
        EndSection


AROS/i386-pc и AROS/X86-64x Native
----------------------------------

.. Примечание:: 
    
    Несмотря на то, что AROS может быть установлена на жёсткий диск, учтите, что
    процесс установки на данный момент является испытательным. Разделы не должны
    подвергаться никаким изменениям, если не было указано, но некоторый риск
    всё равно присутствует.  Со временем в процессе тестирования, установка станет
    более стабильной и надёжной. Таким образом, помните, что **не стоит** 
    использовать установку на компьютеры, жёсткие диски которых содержат важные
    данные. Мы не можем нести ответственность за утерю данных, если она 
    произойдёт в процессе установки, но с большим вниманием отнесемся к любым
    вашим сообщениям об ошибках!
    
Аппаратные требования
"""""""""""""""""""""

AROS достаточно "среднего" PC-AT (на базе i486 или выше) с шиной *PCI*,  мышью
PS/2 или USB, клавиатурой AT,PS/2 или USB, жёстким диском IDE и CDROM на PATA,
видеокартой (S)VGA и соответствующим монитором. Для запуска потребуется как 
минимум 24 MB оперативной памяти. Рекомендуется использование VESA-совместимой
видеокарты VGA. В состав ОС входят общие драйвера с 2D-ускорением для карт от 
ATI и nVidia.

Также могут быть использованы большинство из доступных виртуальных машин (VM).
Достоверно известно, что с AROS работают: QEMU, VmVare (Server/Workstation/Fusion), 
Q, Bochs, MS VPC. Существует дистрибутив AROS под названием 
`VmwAROS <http://vmwaros.org>`_, который содержит AROS и ряд програм, 
предустановленных в виртуальной машине VmWare.

Чтобы опробовать возможности AROS в работе с сетями можно использовать 
сетевую плату с интерфейсом PCI (например, pcnet32 или intel e100). Если
вы столкнулись с трудностями при настройке сети, поищите в FAQ информацию о
поддержке вашего устройства.

Чтобы работать со звуком в AROS, лучшим выбором на текущий момент являются
платы на базе чипов Creative серии 10k.

Порт X86-64x имеет схожие требования с поправкой на то, что поддерживаются
64-битные процессоры Intel/AMD и соответствующие чипсеты. Поддержка чипсетов
на данный момент ограничена. Этот вариант ОС пока довольно молод, и его
ожидает длительное тестирование.

Установочные носители
"""""""""""""""""""""

Рекомендованным носителем для установки AROS является CDROM, на один диск 
целиком умещается и система, и программы для неё. Это также весьма упрощает 
процесс установки, поскольку лишает вас необходимости переносить систему
на множестве отдельных дискет.

В настоящее время диски с AROS не продаются, следовательно, вам потребуется 
доступ к пишущему CD приводу, чтобы записать диск самостоятельно.

CDROM
^^^^^

Запись
''''''

Сперва скачайте ISO-образ LiveCD с AROS со `страницы закачек <../../download>`__ 
и запишите его на CD-диск с помощью любой доступной программы записи. Существует 
множество различных программ для записи дисков, пользователям Windows мы можем 
посоветовать `InfraRecorder <http://infrarecorder.sourceforge.net>`__ - простую,
небольшую и удобную свободную программу для записи дисков. Также есть и другие
программы для записи дисков, например CDBurnerXP, DeepBurn, AstroBurn для Windows,
k3B, Brasero для Linux. Для Amiga и AROS есть программа FryingPan.

Загрузка
''''''''

Проще всего загрузить AROS с CD, если ваш компьютер поддерживает загрузку с CD.
Может потребоваться включить загрузку с CD в BIOS, поскольку на многих компьютерах
она выключена по умолчанию. Просто вставьте диск с AROS в первый привод CD 
и перезагрузите компьютер. Загрузка полностью автоматизирована и, если всё 
в порядке, вскоре вы увидите экран ОС.

Если загрузка с CD не поддерживается на вашем PC, можно создать загрузочную 
дискету и использовать её для загрузки с CD. Достаточно просто вставить носители
соответствующие устройства и перезагрузиться. Начнётся загрузка с дискеты,
и после того, как некоторые важные части (например, хэндлер файловой системы CDROM) 
будут загружены, загрузка продолжится с CD.

Дискета
^^^^^^^

В настоящее время дискеты бывают полезны только для загрузки на PC, 
BIOS которого не поддерживает загрузку с CD, однако их образы по-прежнему
доступны.

Запись
''''''

Чтобы создать загрузочную дискету, вам потребуется скачать образ дискеты со
`страницы закачек <../../download>`__, распаковать архив и записать образ на 
дискету. В случае использования ОС семейства UNIX (например, Linux или FreeBSD), 
это можно сделать следующим образом::

    > cd AROS-<version>-i386-pc-boot-floppy
    > dd if=aros.bin of=/dev/fd0

Если вы работаете в Windows, для записи образа на дискету вам понадобится 
программа rawrite_. Пожалуйста, следуйте её документации по использованию. 
Также существует версия с графическим интерфейсом - rawwritewin.

Загрузка
''''''''
Вставьте дискету в дисковод и перезагрузите компьютер. Загрузка полностью
автоматизирована, и если всё в порядке, вскоре вы увидите рабочий стол AROS.

Установка на жёсткий диск
"""""""""""""""""""""""""

Ранее мы уже предупреждали вас, что установка AROS находится в стадии 
*тестирования* и может быть **опасной** для ваших данных, поэтому 
необходимо убедиться в их отсутствии или сделать полную резервную копию.
Рекомендуется использовать виртуализацию, поскольку она практически безопасна
и позволяет использовать AROS практически на любом компьютере.  

Подготовка
^^^^^^^^^^

Сперва, подготовьте ваш жёсткий диск (реальный или виртуальный) к использованию.
Настоящий диск может потребовать установки перемычек, подключению к компьютеру
и настройки в BIOS. Для виртуального диска, скорее всего, потребуется лишь 
создать новый образ диска, и указать его как одно из возможных устройств загрузки
виртуальной машины (однако CD-привод всё же должен быть первым в этом списке
до того, как вы установите систему).

Дальнейшие действия зависят от ваших потребностей.

Установка AROS без других ОС
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Наиболее простой случай имеет место, если необходимо установить только AROS
на один жёсткий диск, новый или с некими ненужными данными. Также вы можете 
использовать под AROS дополнительный жёсткий диск.

В настоящее время, установка должна производиться при помощи инструмента
InstallAROS, расположенного в директории *Tools* на LiveCD. Запустите её
двойным щелчком по её иконке. Когда она запустится и покажет вам пригласительное
окно, нажмите кнопку ``Proceed`` для перехода к окну с вариантами установки. 

Здесь вы можете увидеть тип текущего устройства для установки (ata.device) и его
номер (0). Если вы хотите произвести установку на дополнительный диск, номер 
следует изменить. Узнать необходимый номер можно с помощью утилиты */Tools/HDToolbox*.
Если вы хотите сохранить разделы на диске как есть и лишь использовать имеющееся 
на нём свободное от разделов место, выберите вариант ``Only use free space``,
или, если диск следует **очистить**, то выберите ``Wipe disk``. Вы также можете 
задать размер создаваемого раздела, и включить создание дополнительного раздела 
WORK для установки программ. После нажатия кнопки "Proceed", инсталятор AROS
создаст разделы и выдаст запрос на перезагрузку. После загрузки следует снова
запустить InstallAROS.

*пока не переведено, может измениться*

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


Установка AROS совместно с Windows(R)/DOS(R)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Установка AROS с Windows довольно проста. В общем и целом, необходимо лишь 
следовать подсказкам инсталятора (как было описано ранее). Инсталятор 
способен определить наличие установки Windows на диске и в этом случае 
автоматически добавит Windows к загрузочному меню GRUB.
 
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

     

**Разбиение на разделы**

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


Возможно несколько вариантов - make a partition in RDB table or as usual PC-MBR
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
Также возможно создание разделов FFS в обычной таблице разделов PC-MBR, что
для Амиги было бы нетипичной ситуацией. 


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

**Форматирование**


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


**Копирование системы**


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

**Установка загрузчика**


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


**Подготовления к запуску**


We have just done our first installation alchemy course, and AROS should
be ready now. We must remove the Live CD from the CD drive (or disable
booting from CD in VM) and check it out. Hear the drum roll? ;)

If something goes wrong, there can be some answers...

**Разрешение проблем**


Installation process is the one of most frequently ones asked about on forums,
mostly by newbees. You can check the FAQ if there an answer to your questions.
Any additions ? ...




AROS/PPC-linux
--------------

Требования
""""""""""

To be written by someone.

Извлечение
""""""""""

Распакуйте архив со `страницы закачек <../../download>`__ в удобный для вас 
каталог::

    > tar -vxjf AROS-<version>-ppc-<platform>-system.tar.bz2

Если вы скачали дополнительный архив программ, вы можете распаковать и его::

    > tar -vxjf AROS-<version>-ppc-all-contrib.tar.bz2

Запуск
""""""

To be written by someone.

AROS/m68k-backport (AfA)
------------------------

Это не вполне обычный вид AROS, а скорее нечто, что принято называть *бэкпортом*,
т.е. набор библиотек и исполняемых файлов для расширения возможностей 
оригинальной AmigaOS. AfA означает AROS for Amigas (AROS для Амиги). AfA
разрабатывается Bernd Roesch и может быть найдена 
`здесь <http://amidevcpp.amiga-world.de/afa_binarie_upload.php>`_.

Требования
""""""""""

To be written by someone.

Установка
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

