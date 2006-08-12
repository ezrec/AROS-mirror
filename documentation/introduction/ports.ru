=====
Порты AROS
=====

:Authors:   Adam Chodorowski, Matthias Rustler 
:Copyright: Copyright Љ 1995-2006, The AROS Development Team
:Version:   $Revision: 24547 $
:Date:      $Date: 2006-06-15 20:41:03 +0500 (аЇб™, 15 аИб­аН 2006) $
:Status:    Done.

.. Contents::


Введение
============

Уже упоминалось, что AROS - портируемая ОС, следовательно, она доступна на нескольких различных платформах. "Порт" AROS - это, буквально, и есть версия AROS, портированная на определенную платформу.

Since AROS is a portable operating system, it is available for several different
platforms. A "port" of AROS is exactly what the name implies, ie. a version of
AROS ported to some specific platform. 


Типы (Flavors)
--------

Все порты делятся на два разных вида, или типа (Flavors в терминологии AROS),
а именно - на "родные" ("native") и "гостевые" ("hosted")
Ports are divided up into two major groups, or "flavors" in AROS terminology,
namely "native" and "hosted". 


Native-порты  работают в непосредственном контакте с "железом" и имеют полный доступ к компьютеру. В будущем, они станут самым естественным способом существования AROS, поскольку обеспечивают наивысшую производительность и эффективность, однако, в настоящее время они недостаточно развиты (по крайней мере, очень неудобны для разработки)
ports run directly on the hardware and have total control over the
computer. They will become the recommended way to run AROS in the future since
it gives superior performance and efficiency, but they are currently too
incomplete to be useful (at least for development).

Hosted-порты работают под управлением другой ОС и не обращаются к "железу напрямую", используя мозможности ОС-хоста. Их основное преймущество - простота разработки, поскольку отпадает нужда писать драйвера и облегчается сообщение с ОС. К тому же, пока что AROS не поддерживает себя сама (пока нельзя скомпилировать AROS под ней же самой), это существенно ускоряет программирование, поскольку позволяет запускать параллельно и среду разработки, и "подопытную" ОС, не прибегая к постоянным перезагрузкам только для опробования написанной программы.
 run on top of another operating system and do not access the
hardware directly, but use the facilities provided by the host OS. The
advantages of hosted ports is that they are easier to write since it is not
necessary to write low-level drivers. Also, since AROS development is not
self-hosted yet (we cannot compile AROS from inside AROS) it greatly speeds up
programming since we can run the development environment and AROS side-by-side
without wasting time on constant reboots to try out new code.


Наименования
------

Названия портов AROS имеют вид <процессор>-<платформа>, где - <процессор> - 
архитектура используемого процессора, а <платформа> - символическое название платформы. Платформа может быть как "железной" (в случае native-портов), такой как "pc" или "amiga", или ОС (в случае hosted-портов), таких как "linux" or
"freebsd". Очевидно, предметом рассмотрения остаётся AROS, поэтому окончательно к названию добавляется приставка - "AROS/", что даёт например, "AROS/i386-pc". 

The different AROS ports are named on the form <cpu>-<platform>, where <cpu> is
the CPU architecture and <platform> is a symbolic name of the platform. The
platform of a port can either be a hardware one for native ports, such as "pc"
or "amiga", or an operating system for hosted ports, such as "linux" or
"freebsd". In cases when it is not obvious that the topic is AROS it is common
to prefix "AROS/" to the port name, giving you for example "AROS/i386-pc". 


Портируемость
-----------

Исполняемые файлы AROS пригодны для всех портов, имеющих одинаковый процессор, что гарантирует, что программы, скомпилированные для "i386-pc" будут работать равно на "i386-linux" и "i386-freebsd".
executables for a specific CPU are portable across all ports using that
CPU, which means that executables compiled for "i386-pc" will work fine on
"i386-linux" and "i386-freebsd".


Существующие порты
==============

Ниже приведен список всех портов AROS, действительно работающих или находящихся в активной разработке. Однако, для скачивания доступны не все, поскольку они либо не закончены в достаточной мере, либо имеющиеся ресурсы не соответствуют их требованиям для сборки.
Below is a list of all AROS ports that are in working order and/or actively
developed. Not all of these are available for download, since they might 
either be not complete enough or have compilation requirements that we can't 
always meet due to limited resources.


AROS/i386-pc
------------

:Flavour:    Native
:Состояние:  Работает, поддержка драйверов пока недостаточна
:Поддержка:  Есть

AROS/i386-pc native-порт предназначен для всего семейства IBM PC AT-совместимых компьютеров, имеющих процессоры x86 архитектуры. Название несколько обманчиво, поскольку данный порт требует, по меньшей мере, 486 - совместимого процессора,
благодаря использованию инструкций, отсутствующих у 386. 
(примечание переводчика - по моим данным, это всё-таки хотя бы Pentium.)


is the native port of AROS to the common IBM PC AT computers and
compatibles using the x86 family of processors. The name is actually a bit
misleading since AROS/i386-pc actually requires at least a 486 class CPU due to
usage of some instructions not available on the 386.

Этот порт вполне работоспособен, но пока имеет только самые базовые драйверы. Одним из существенных ограничений является поддержка ускорения графики только на картах nVidia and ATI. Владельцы прочих графических адаптеров вынуждены пользоваться "стандартными" (без ускорения) VGA и VBE-драйверами. В разработке находятся и прочие драйверы, но процесс идёт крайне медленно, поскольку разработчиков драйверов у нас примерно 2.5 . Порт доступен для скачивания.
This port works quite well, but we only have the most basic driver
support.  One of the biggest limitations is that we currently only have
support for accelerated graphics on nVidia and ATI graphics hardware. Other
graphics adapters must be used with generic (non-accelerated) VGA and
VBE graphics drivers. There are more drivers in the works, but
development is quite slow since we only have about 2.5 hardware hackers.
This port is available for download.


AROS/m68k-pp
------------

:Flavour:    Native 
:Состояние:  Частично работоспособен(в эмуляторе), мало драйверов
Partly working (in an emulator), incomplete driver support
:Поддержка:  Есть

AROS/m68k-pp - native-порт AROS на серию Palm-совместимых КПК ("pp" здесь означает "palm pilot", так назывались КПК-родоначальники этой серии). Это внушает надежду, что когда-нибудь AROS будет путешествовать с вами - в вашем нагрудном кармане.
AROS/m68k-pp is the native port of AROS to the Palm line of handheld computers
and compatibles ("pp" stands for "palm pilot", which was the name of the first
handhelds of this line). This means that you might be able to take AROS with you
in your pocket when traveling in the future...

В настоящее время этот порт весьма "сырой". В общем и целом он работоспособен (запускается в эмуляторе, никто пока не идёт на риск потерять свою драгоценную железку), но над ним ещё работать и работать. Есть графический драйвер, но ни единого драйвера устройств ввода. К скачиванию пока недоступен.
This port is currently very raw. It does mostly work (running in an emulator,
since nobody wants to risk trashing their expensive hardware just yet) but there
is still a lot of work left. There is a graphics driver, but not any input ones.
This port is not available for download at this time.


AROS/i386-linux
---------------

:Flavour:   Hosted
:Состояние: Работоспособен
:Поддержка: Есть

AROS/i386-linux - hosted-порт AROS на ОС Linux [#]_ для семейства процессоров x86.
AROS/i386-linux is the hosted port of AROS to the Linux operating system [#]_
running on the x86 family of processors. 

Наиболеее "зрелое" воплощение идей AROS, поскольку Linux стал основной рабочей средой её разработчиков, и этот порт почти не требует драйверов для работы. Доступен для скачивания.
This is the most complete port of AROS feature-wise, since most of the 
developers currently use Linux when developing AROS, and there are far 
fewer drivers to write. This port is available for download.


AROS/i386-freebsd
-----------------

:Flavour:    Hosted
:Состояние: Работоспособен
:Поддержка: Есть (5.x)

AROS/i386-freebsd - hosted-порт AROS на ОС FreeBSD для семейства процессоров x86.
AROS/i386-freebsd is the hosted port of AROS to the FreeBSD operating system
running on the x86 family of processors. 

Этот порт также относительно закончен, поскольку делит бОльшую часть кода с AROS/i386-linux портом, но, ввиду малой популярности FreeBSD среди наших разработчиков, всё же отстаёт от него. При построении снимков (snapshots) мы стараемся компилировать его, но это удаётся не всегда, поэтому скачивание доступно также не постоянно.

This port is relatively complete since it shares most of it's code with that of
AROS/i386-linux, but since there aren't many developers that use FreeBSD it is
lagging a little bit behind. We try to compile AROS/i386-freebsd when doing
snapshots, but it is not always possible, so it might not always be available
for download.

AROS/ppc-linux
---------------

:Flavour:    Hosted
:Состояние: Работоспособное
:Поддержка: Есть

AROS/ppc-linux - hosted-порт AROS для семейства процессоров PPC.
AROS/ppc-linux is the hosted port of AROS to the Linux operating system
running on the PPC family of processors.

Сборку можно скачать с `Sourceforge`__.
Для сборки необходима патченая версия gcc3.4.3, diff для которой можно найти в contrib/gnu/gcc.
A precompiled version can be downloaded from `Sourceforge`__.
Rebuilding requires a patched gcc3.4.3. The diff file can be found in contrib/gnu/gcc.

__ http://sourceforge.net/project/showfiles.php?group_id=43586&package_id=194077

От переводчика
Ещё известные мне порты
AROS/x86-windows - порт был прекращен разработчиком. Статус неизвестен.
AROS/68k-native (aka AfA, Aros for Amiga) - порт в разработке, работоспособен (?), требует MMU.


Заметки
=========

.. [#] Да, мы знаем, что Linux на самом деле всего лишь ядро, а не ОС в целом,         но всё же намного проще и короче писать так, нежели "операционные               системы, основанные на ядре Linux, некоторых стандартных инструментах           GNU и оконной системе X". Правда, как придирчивые читатели заметят, всё         упрощение потеряло смысл благодаря этому замечанию, но всё же ...
Yes, we know that Linux is really just a kernel and not a whole OS, but
       it is much shorter to write that rather than "operating systems based on
       the Linux kernel, some of the common GNU tools and the X windowing
       system". This size optimization is of course negated by having to write
       this explanation for the pedantic readers, but anyway...

