========================
ЧАсто задаваемые ВОпросы
========================

:Authors:   Aaron Digulla, Adam Chodorowski, Sergey Mineychev
:Copyright: Copyright © 1995-2007, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Done.

.. Contents::

Общие вопросы
=============

Могу ли я задать вопрос?
------------------------

Определенно! Для этого зарегистрируйтесь на
`форуме Aros-Exec <http://aros-exec.org/modules/newbb/viewtopic.php?topic_id=1636&start=0>`
и задайте свой вопрос в его соответствующей ветке.

Что вообще такое AROS ? 
-----------------------

Об этом читайте во введении_.

.. _введении: ../../introduction/index


Каково официальное положение AROS?
----------------------------------

Согласно европейским законам, считается законным применение технологий инженерного анализа для достижения взаимодействия систем. Также определено, что является незаконным распространение полученных таким образом сведений. Фактически, это означает, что возможно дизассемблирование или переписание программ для обеспечения совместимости (например, законно будет дизассемблировать Word, чтобы создать программу, конвертирующую документы Word в текст ASCII).

Разумеется, это влечет за собой ограничения: не позволяется дизассемблировать 
программы, если собранная в результате информация могла быть получена другими средствами. Также её нельзя никому передать. С такой точки зрения, книга вроде "Windows изнутри" является незаконной, или, по крайней мере, сомнительной законности.

Поскольку мы избегаем дизассемблирования и используем вместо него общедоступные сведения (включая руководства по программированию) которые не попадают под какие-либо NDA, перечисленное выше неприменимо непосредственно к AROS. Важна здесь суть закона: законным является написание программ, совместимых с другими программами. Таким образом, мы считаем, что закон на стороне AROS.

Тем не менее, патенты и заголовочные файлы - другое дело. Мы можем применять запатентованные алгоритмы в Европе, поскольку европейские законы не разрешают патентовать алгоритмы. Однако, код, использующий алгоритмы, запатентованные в США, не может быть импортирован в США. Примеры запатентованных алгоритмов AmigaOS включают перетаскивание экрана и специфичную работу меню. Соответственно, мы стараемся не реализовывать эти вещи в точности такими же. Заголовочные файлы же должны быть максимально совместимы, но вместе с тем и максимально отличны от исходных.

Во избежание проблем, мы затребовали официальное подтверждение от Amiga Inc. Однако, несмотря на их весьма положительное отношение к самой идее, с ними нелегко относительно законности. Остается принять факт, что Amiga Inc. не прислала нам ни подтверждения, ни отказа, как хороший знак. К сожалению, пока что никакого официального заключения сделано не было, несмотря на добрые намерения с обеих сторон.

Почему ваша цель - совместимость только с AmigaOS 3.1?
------------------------------------------------------

Когда-то обсуждалось создание улучшенной ОС с возможностями AmigaOS, однако,
исходя из лучших побуждений, от этого отказались. Во-первых, все были согласны, что AmigaOS требовала доработки, но никто не знал, как этого добиться; не было даже общего согласия, что именно требует доработки, что важно. К примеру, некоторые разработчики хотели бы ввести защиту памяти, но не такой ценой (полное переписание имеющегося ПО и снижение производительности).

В конце концов, обсуждение перетекало в флэймовые войны или зацикливалось на одних и тех же аргументах. Так что мы решили начать с того, что нам знакомо. Затем, обретя опыт и знание того, что возможно, а что - нет, можно подумать и об улучшениях.

Также планировалась совместимость на бинарном уровне с оригинальной AmigaOS на Амиге. Это имело очень простое объяснение - новая ОС без программ, идущих на ней, не имеет шансов на выживание. Поэтому мы старались сделать переход от оригинальной к новой ОС как можно более безболезненным (но не настолько, чтобы лишить AROS возможности к совершенствованию). Как всегда, за всё приходится платить и каждый раз приходится взвешивать, чем придётся расплатиться, и хотелось бы нам этого или нет.


Можно ли ввести возможность XYZ?
--------------------------------

Нет, потому что: 

а) Если бы это было так важно, это уже было бы :-) (в оригинальной ОС)
b) Почему бы вам самим не сделать и не прислать нам патч?

Причина такого подхода в том, что большинство считают нужную им возможность самой важной и что AROS обречена, если не применить её правильным образом. Наша же позиция - в том, что AmigaOS, которую AROS старается воплотить, способна делать всё то же, на что способны все современные ОС. Мы замечаем, что есть области, где AmigaOS можно доработать, но если это сделать, кто будет дописывать всё остальное ? К тому же, это привело бы к появлению множества мелких доработок над оригинальной AmigaOS, которые бы мешали нормальной работе всех приложений, но при этом в целом ничего бы не стоили, так как остальные функции были бы утеряны.

Так что мы решили предотвращать все попытки внедрения кардинально новых возможностей в ОС до тех пор, пока она не будет более-менее закончена. Мы уже близки к цели, и был введен ряд инноваций, которых не было в оригинальной AmigaOS.


Насколько AROS совместима с AmigaOS?
------------------------------------

Весьма совместима. Мы ожидаем, что под AROS для Амиги без проблем будут работать 
имеющиеся программы для Амиги. На других платформах эти программы должны быть 
перекомпилированы. Мы добавим препроцессор, который при проверке вашего кода 
либо изменит его несовместимые части, либо предупредит о них.

Перенос программ с AmigaOS на AROS в настоящее время, скорее, задача простой 
рекомпиляции, и, возможно, изменения кода в некоторых местах. Разумеестся, есть 
исключения из сказанного, но это применимо также для многих современных программ.


Для каких процессорных платформ доступна AROS?
----------------------------------------------

Сейчас AROS доступна в более-менее пригодном состоянии в native- и hosted-виде 
(на Linux и FreeBSD) для архитектуры i386 (то есть IBM PC-AT совместимых 
компьютеров). Также в разработке находятся порты  различных степенях готовности 
на SUN SPARC (hosted под Solaris) и Palm-совместимых PDA (native).


Будет ли AROS портирована на PPC? 
---------------------------------

В настоящее время идёт работа по переносу AROS на PPC, первоначально под Linux.


Почему Вы используете Linux и X11?
----------------------------------

Мы применяем Linux и X11 для ускорения разработки. Например, если требуется 
простая новая функция открытия окна, можно написать одну эту функцию, не вводя 
множество других функций в layers.library, graphics.library, ряд драйверов и 
другое, что ещё этой функции потребуется.

Задача AROS, тем не менее, быть всё же независимой от Linux и X11 (но она всё 
же будет способна, при надобности, работать с ними), и это становится 
реальностью благодаря native-версиям AROS. Тем не менее, Linux всё ещё нужен 
при разработке, поскольку ещё не все требуемые инструменты разработки 
перенесены на AROS.


Как же вы собираетесь добиться переносимости AROS?
--------------------------------------------------

Одим из самых значительных отличий AROS от AmigaOS является применение системы 
HIDD (Hardware Independent Device Drivers), позволяющей с лёгкостью портировать 
AROS на различные платформы. Вкратце, базовые библиотеки ОС обращаются к оборудованию
не напрямую, а через HIDDы, которые написаны с применением объектно-ориентированного
подхода, позволяющего легко заменять HIDDы, и использовать код повторно.


Вы верите в успех AROS? 
-----------------------

Каждый день мы слышим от многих, что у AROS ничего не выйдет. Большинство из них
либо не знают, что именно мы делаем, либо считают, что Амига давно мертва. После того,
как первым объяснили, в чём наша задача, большинство из них согласилось, что это 
возможно. Со вторыми хлопот больше. Ладно, Амига уже мертва ? Пользователи Амиги
определенно с ними не согласятся. Разве их A500 или A4000 взорвались,
когда Commodore объявили банкротом ? А когда та же участь постигла Amiga Technologies ?

Однако фактом является малое количество новых программ для Амиги (что не мешает
Aminet жить и здравствовать) и медленность разработки оборудования (впрочем, как 
раз начали появляться интересные гаджеты). Сообщество Amiga, живое до сих пор,
замерло в ожидании. И если появится продукт, достойный Amiga, какой она была в 1984,
его вновь ждёт успех. Как знать, быть может, к машине будет прилагаться диск с надписью
"AROS"? :-)


Что мне делать, если не удаётся скомпилировать AROS?
----------------------------------------------------

Пожалуйста, отправьте подробное сообщение (например, с сообщениями об ошибках, которые
вы получили) в раздел Help форума `AROS-Exec`__ , или запишитесь в разработчики и подпишитесь на список рассылки разработчиков AROS, и опубликуйте его там,
кто-нибудь попробует вам помочь.

__ http://aros-exec.org/


Будет ли в AROS защита памяти, SVM, RT, ...?
--------------------------------------------

Несколько сотен экспертов Amiga (по крайней мере, считавщих себя таковыми)
в течение трёх лет пытались найти способ внедрить защиту памяти (MP) в AmigaOS.
Их ждала неудача. Так что приходится принимать как факт, что обычная AmigaOS никогда
не будет иметь MP, подобно Unix или Windows NT.

Но не всё потеряно. Есть планы ввести подвид MP в AROS, предоставляющий защиту
по меньшей мере новым программам, учитывающим эту возможность. Некоторые из попыток
внедрения выглядят весьма многообещающе. К тому же, насколько страшен отказ вашей
системы ? Не спешите казнить, дайте объяснить :-) Проблема - не в отказе самой системы,
а в следующем:

1. Вы не имеете особого представления, почему произошёл сбой. В сущности, 
попытки выяснения подобны копанию 100-метровой палкой в болоте при густом тумане.
2. Вы потеряли ваши данные. Перезагрузка, разумеется, не решает проблемы. 

Что можно попробовать сделать, это разработать систему, которая, по крайней мере,
будет предупреждать о сомнительных действиях, и способную детально описать сбой 
системы, которая позволит сохранить данные *перед* сбоем. Также должно быть
средство, позволяющее выяснить, что именно сохранилось, что позволит быть 
уверенным, что данные остались в целости и можно продолжить работу.

То же самое относится и к SVM (swappable virtual memory, выгружаемой виртуальной памяти)
RT (resource tracking, отслеживание ресурсов), и SMP (symmetric multiprocessing). 
В настоящее время, мы прикидываем, как внедрить их таким образом, чтобы 
доставить меньше всего хлопот. Тем не менее, сейчас они не считаются 
приоритетными. Очень простое RT уже было введено.


Можно ли стать бета-тестером?
-----------------------------

Конечно, без проблем. В действительности, чем больше бета-тестеров, тем
лучше, так что приглашаются все! Мы не ведем списка бета-тестеров, так что 
всё, что вам надо сделать - это скачать AROS, опробовать всё что вы захотите, и
прислать нам отчёт.


Какова связь между AROS и UAE?
------------------------------

UAE - это эмулятор Amiga, и поэтому имеет другие цели, нежели AROS. UAE старается
быть бинарно-совместимым даже для игр и программ, работающих с аппаратной частью,
в то время, как AROS предпочитает иметь свои приложения. Поэтому AROS значительно
быстрее UAE, но программ под UAE значительно больше.

Мы поддерживаем контакт с автором UAE и поэтому есть хорошие шансы, что поддержка
UAE будет введена в AROS, и наооборот (в настоящее время, UAE портирован, но не 
интегрирован). Например, разработчики UAE заинтересованы в исходниках AROS, поскольку
UAE может работать намного быстрее, если некоторые (или все) функции OS заменить
на "родной" код. С своей стороны, AROS может выиграть от встроенной эмуляции
Amiga.

Поскольку большинство програм не будут доступны под AROS с самого начала, Fabio 
Alemagna портировал UAE на AROS с тем, чтобы можно было запускать старые 
программы, по крайней мере, в окне эмуляции.


Какова связь между AROS и Haage & Partner?
------------------------------------------

Haage & Partner использовали фрагменты AROS в AmigaOS 3.5 и 3.9, например, такие
элементы, как colorwheel и gradientslider и команда SetENV. Это означает, что по
ходу своего развития, AROS пополнила AmigaOS, став её частью. Но это не означает, 
что имели место какие-то официальные отношения между AROS и Haage & Partner. 
AROS - проект с открытым исходным кодом, и любой может использовать наш
код в своих проектах, если будет использовать его в рамках нашей лицензии.


Какова связь между AROS и MorphOS?
----------------------------------

Отношения между AROS и MorphOS практически те же, что и между AROS и Haage & 
Partner. MorphOS использует части AROS для ускорения собственных разработок, в 
рамках нашей лицензии. Как и в случае с Haage & Partner, от этого выигрывают
обе стороны, поскольку MorphOS получает дополнительный стимул к ускорению 
разработок, а AROS - дополнения к коду, разработанные для MorphOS. Никаких 
официальных отношений не существует, такова разработка open source - проектов.


Какие есть языки программирования?
----------------------------------

Большинство кода для AROS написано для ANSI C с помощью кросс-компилирования под 
другими ОС, такими, как Linux и FreeBSD. Fabio Alemagna завершил первоначальный 
порт GCC на i386-native. Однако, пока он не присутствует на образах дисков и 
не внесен в сборочную систему.

Доступные сейчас нативно языки - это Python_, Regina_ и False_:

+ Python - скриптовый язык, набирающий популярность благодаря удобному дизайну и
  возможностям (объектно-ориентированное программирование, система модулей, 
  имеется множество полезных модулей, чёткий синтаксис, ...). Был оформлен 
  отдельным проектом, расположенным по адресу http://pyaros.sourceforge.net/.

+ Regina - переносимый ANSI - совместимый интерпретатор REXX. Задача порта для 
  AROS - получение совместимого с ARexx для классической AmigaOS интерпретатора.

+ False можно воспринимать как экзотический язык, и он, скорее всего, не будет    использоваться в серьёзных проектах, однако порой он весьма забавен. :-)

.. _Python: http://www.python.org/
.. _Regina: http://regina-rexx.sourceforge.net/
.. _False:  http://strlen.com/false-language


Почему в AROS не встроен эмулятор m68k?
----------------------------------------

Чтобы сделать возможным запуск старых программ Amiga на AROS, был портирован UAE_.
Эта версия, возможно, окажется несколько быстрее остальных версий UAE, поскольку
AROS требует меньше ресурсов, чем прочие системы (что значит, что UAE может получить
больше процессорного времени), и мы пытаемся пропатчить Kickstart ROM в UAE для
обращения к функциям AROS, что может дать дополнительный прирост скорости. 
Разумеется, сказанное касается native-видов AROS, но не hosted.

Но почему же мы попросту не введем виртуальный процессор m68k, что бы такие 
программы можно было запускать напрямую ? Главная проблема здесь - в том, что 
программам для m68k нужны данные в формате big endian, в то время, как AROS
работает также и на little endian - процессорах. Сама проблема состоит в
обеспечении работы little endian - подпрограмм в ядре AROS с big endian - данными
эмуляции. Автоматическое их преобразование не представляется возможным (в 
качестве примера - есть поле одной из структур в AmigaOS, которое иногда состоит
из одного ULONG - значения, а иногда - из двух WORD), поскольку нельзя сказать
точно, каков именно формат байт информации в памяти.

.. _UAE: http://www.amigaemulator.org/


Будет ли в AROS Kickstart ROM? 
------------------------------

Такое возможно, если кто-либо сделает native-порт AROS на Amiga и сделает всю 
прочую работу для создания Kickstart ROM. Пока что никто не взялся за это.


"Программные" вопросы (software)
================================

Как прочесть диски AROS под UAE?
--------------------------------

Образ дискеты можно смонтировать как hardfile и затем использовать как жёсткий
диск на 1.4 МБ в UAE. После того, как вы скопировали все необходимые файлы на
образ диска (или сделали прочие нужные вам действия), его можно записать на дискету.

Геометрия hardfile имеет следующий вид::

    Sectors    = 32
    Surfaces   = 1
    Reserved   = 2
    Block Size = 90


Как прочесть образы дисков AROS под hosted-видами AROS?
-------------------------------------------------------

Скопируйте образ диска в директорию DiskImages (SYS:DiskImages, пр.
bin/linux-i386/AROS/DiskImages) и переименуйте его в "Unit0". После запуска 
AROS образ диска можно присоединить командой::

    > mount AFD0: 


Что такое Zune?
---------------

Если вы прочитали на этом сайте о Zune, то знайте, что это просто
реинкарнация MUI в открытом исходном коде. MUI - это мощный, удобный и для
пользователей, и для разработчиков объектно-ориентированный пакет разработки
GUI с shareware-лицензией, стандарт де-факто в AmigaOS. Zune - основной пакет 
разработки GUI-приложений AROS. Само имя ничего особенного не означает, но
звучит прикольно. =)

Как восстановить Настройки по умолчанию?
----------------------------------------

Загрузив AROS, откройте окно CLI, перейдите в Envarc: (командой cd) 
и удалите файлы, соответствующие настройке, которую необходимо восстановить.

Какова разница между Графической и обычной памятью в Wanderer?
--------------------------------------------------------------

Это разделение памяти, в основном, является лишь отголоском прошлого Амиги,
когда графическая память была основной памятью, доступной приложениям,
пока не добавлялась другая память, называвшаяся FAST RAM, где и располагались
приложения. При этом графика, звуки и некторые системные структуры оставались
в графической памяти. 

В AROS-hosted не существует такой памяти, как FAST, есть только графическая.
В Native AROS максимальный размер этой памяти не превышает 16МБ, но при этом
никак не отображает размер памяти графического адаптера. Фактически, 
этот параметр не имеет никакого отношения к памяти графического адаптера.

*Развернутый ответ*
Графическая память в i386-native относится к нижним 16МБ системной памяти.
В пределах этой области карты шины ISA могут использовать DMA. Выделение 
памяти при помощи функций MEMF_DMA и MEMF_CHIP возможно только в её пределах,
остальных - в прочей памяти, аналоге FAST.

Чобы получить подробные сведения о доступной памяти используйте команду
C:Avail HUMAN.

Для чего служит команда Wanderer Запомнить <окно/все>? 
------------------------------------------------------

Эта команда должна запомнить расположение иконок одного или всех окон.


Как изменить использемые хранитель экрана и обои?
-------------------------------------------------

At the moment the only way to change screensaver is to write your one.
Blanker commodity could be tuned with Exchange, but it able to do only 
"starfield" with given amount of stars.
Background of Wanderer is set by Pref tool Prefs/Wanderer.
Background of Zune Windows is set by Zune prefs Prefs/Zune. You can also set 
your chosen application preferences by using the Zune <application> command.

I`ve launch AROS-hosted but it`s failed
---------------------------------------

This could be probably fixed by creating a WBStartup directory in the AROS 
directory. If you are root and AROS crashes at launch, do "xhost +" before 
"sudo && ./aros -m 20". You must also give it some memory with -m option as 
shown. Also don`t forget about BackingStore option in section Device of your 
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

Here`s some::

    floppy=<disabled/nomount>	Sets the trackdisk device options
	disabled		- completely disable trackdisk.device initialisation
	nomount			- initialise trackdisk.device but do not create DOS devices
		
    ATA=32bit - Enables 32-bit I/O in the hdd driver (safe)
    forcedma - Forces DMA to be active in the hdd driver (should be safe, but       
    might not be)
    gfx=<hidd name> - Use the named hidd as the gfx driver
    lib=<name> - Load and init the named library/hidd
	
На прежних сборках (от r28786)::

	nofdc - Disables the floppy driver completely.
    noclick - Disabled the floppy disk change detection (and clicking)

Please note that they are case-sensitive.

How can I transfer files to virtual machine with AROS?
------------------------------------------------------

First and simpliest way is to put files to the ISO image and and connect it to VM. There`s alot of programs able to edit ISO`s like UltraISO, WinImage, 
or mkisofs. Second, you can set up the network in AROS and FTP server on your 
host machine. Then, you can use FTP client for AROS to transfer files. This is 
tricky enough to stop at this point. User documentation must contain chapter on 
networking.Also, now
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



Вопросы по оборудованию (hardware)
==================================


Где найти список совместимого оборудования для AROS?                   
----------------------------------------------------

You can find one on the `AROS Wiki <http://en.wikibooks.org/wiki/Aros/Platforms/x86_support>`__ page. There can be
another lists made by the AROS users.

Why Aros can`t boot from my drive set as the SLAVE on IDE channel?
------------------------------------------------------------------

Well, AROS should boot if the drive is SLAVE but ONLY if there`s a drive also 
on MASTER. That`s appeared to be a correct connection respecting to the IDE 
specification, and AROS follows it.

My system hangs with red cursor on screen
-----------------------------------------

One reason for this can be use of the serial mouse (these is not supported yet).
You must use PS/2 mouse with AROS at the moment. Another can be if you`ve chosen 
an video mode unsupported by your hardware, in boot menu. Reboot and try a different one.
