==========================
Frequently Asked Questions
==========================

:Authors:   Aaron Digulla, Adam Chodorowski, Sergey Mineychev, AROS-Exec.org
:Copyright: Copyright Š 1995-2007, The AROS Development Team
:Version:   $Revision: 26886 $
:Date:      $Date: 2007-09-29 17:05:46 +0200 (Sat, 29 Sep 2007) $
:Status:    Done.

.. Contents::

Vanliga frågor
==============

Får jag fråga en fråga?
-----------------------

Naturligtvis! Gå till `AROS-Exec forum`__ och läs trådarna
och fråga allting som du vill. Den här FAQ är uppdaterad med användarfrågor,
men forumet är alltid mest aktuellt.

__ http://aros-exec.org/modules/newbb/viewtopic.php?topic_id=1636&start=0


Vad handlar AROS om? 
--------------------

Läs gärna denna introduktion_.

.. _introduktion: ../../introduction/index


Vad säger lagen om AROS?
------------------------

Europeisk lag säger att det är lagligt att använde omvänd utvecklingsteknik 
(reverse engineering) för att få kompabilitet. Den säger även att det är
olagligt att distribuera kunskapen som man får av dessa tekniker. Det som
egentligen menas med detta är att du får dissemblera eller studera vilken
mjukvara som helst för att skriva ett program som är kompatibelt med detta
(till exempel så skulle det vara lagligt att dissemblera Word för att skriva
ett program som kan konvertera Word-dokument till ASCII-text).

Det finns naturligtvis undantag: du får inte dissemblera mjukvaran om informationen
som du är ute efter går att få tag på med andra sätt. Du får heller inte informera
andra om vad du har lärt dig. En bok med titeln "Windows inside" är därför
olagligt eller åtminstone tvivelaktigt laglig.

Eftersom vi undviker dissembleringstekniker och istället använder den kunskap
som redan finns (vilket inkluderar programmeringsmanualer)  som inte går under
någon liknande lag, så kan man inte applicera detta med AROS. Det som räknas här
är intentionerna i lagen: det är lagligt att skriva mjukvara som är kompatibel
med annan mjukvara. Därför är våran övertygelse att AROS är skyddat av lagen.

Patent och header files är ett annat ämne. Vi kan använda patenterade algoritmer
i europe eftersom europesik lag inte tillåter patent på algoritmer.
Dock får kod som använder algoritmer som är patenterade i USA inte importeras
till USA. Exempel på patenterade algoritmer i AmigaOS är t.ex. skärmdragning
och hur t.ex. menyer fungerar. Därför undviker vi att implementera dessa
funktioner på exakt samma sätt. Header files måste å andra sidan vara kompatibla
men så olika orginalet som möjligt.

För att undvika problem så har vi frågat om ett officiellt OK från Amiga Inc. De
är ganska positiva till vårat arbete men väldigt obekväma angående den lagliga
innebörden. Vi vill uppmärksamma dig på det faktum att Amiga Inc inte har
skickat oss brev där de uppmanat oss att fortsätta eller upphöra med utvecklingen.
Olyckligtvis så har ingen överenskommelse ännu blivit gjord, förutom att båda parter
har goda intentioner.


Varför siktar ni på kompabilitet med AmigaOS 3.1?
-------------------------------------------------

Det har pågått diskussioner om att skriva ett avancerat operativsystem med
funktioner från AmigaOS. Dessa diskussioner har avslutats av en bra anledning.
Först och främst så är alla överens om att nuvarande AmigaOS kan bli bättre,
men ingen vet hur det ska göras eller kan komma överens om vad som ska förbättras
eller vad som är viktigt. Till exempel så vill en del ha minnesskydd (memory
protection), men vill inte betala priset för detta (Stora omskrivningar av
tillgänglig mjukvara och hastighetssänkningar).

I slutändan så har diskussionerna slutat i heta diskussioner eller återgång till
samma argument om och om igen. Så vi beslutade att starta med någonting som
vi viste att vi kunde hantera. Sen när vi har erfarenheter för att se vad som
är möjligt eller inte, så kan vi besluta om förbättringar.

Vi vill även ha binär kompabilitet med AmigaOS på Amigadatorer. Anledningen till
detta är just att ett nytt operativsystem utan program inte har någon chans att
överleva. Därför försöker vi att få övergången från AmigaOS till det nya att gå
så smärtfritt som möjligt (men inte till den grad att vi inte kan förbättra AROS
i efterhand). Som vanligt, allting har sitt pris och försöker att göra genomtänkta
beslut om vilket pris som det kostar och om alla andra är villiga att betala det.

Kan ni inte implementera funktionen XYZ?
----------------------------------------

Nej, därför: 

a) Om det verkligen är så viktigt så borde det finnas i AmigaOS. :-) 
b) Varför inte göra det själv och skicka patchen till oss?

Anledningen till denna attityd är att det finns väldigt många som tycker att deras
funktion är viktigast och att AROS inte har någon framtid om inte funktionen 
omedelbart implementeras. Våran ståndpunkt är att AmigaOS, som AROS siktar på att
implementera, kan göra allting som ett modernt operativsystem kan göra. Vi ser
att det finns områden där AmigaOS skulle behöva förbättras inom, men om vi gör det,
vem skulle skriva resten av operativsystemet? I slutändan så skulle vi då ha en massa
fina förbättringar jämfört med AmigaOS som skulle göra det mycket svårare att använda
redan existerande mjukvara, eftersom resten av operativystemet skulle saknas.

Därför har vi beslutat att vänta med varje försök till att implementera stora
nya funktioner i operatisystemet tills att operativsystemet är mer eller mindre
klart. Vi har kommit ganska så nära målet nu och det har utvecklats en del funktioner
i AROS som inte finns tillgängligt i AmigaOS.


Hur kompatibelt är AROS med AmigaOS?
------------------------------------

Väldigt kompatibelt. Vi förväntar oss att AROS kommer att köra existerande
mjukvara på Amigan utan problem. På annan hårdvara så måste mjukvaran
rekompileras. Vi kommer att erbjuda en preprocessor som du kan använda på din
kod som kommer ändra eventuell kod som eventuellt krashar med AROS och/eller
varna om sådan kod.

Portning av program från AmigaOS till AROS handlar mestandels om en enkel
rekompilering, med vissa förändringar. Det finns naturligtvis program med
undantag, men det stämmer för de flesta moderna program.


För vilka hårdvaruplattformar finns AROS tillgängligt? 
------------------------------------------------------

För tillfället så finns AROS tillgängligt i en ganska användbar version som
native och hosted (I Linux och FreeBSD) för i386 arkitekturen (IBM PC AT
kompatibla kloner). Det finns portningar under utveckling till SUN SPARC
(Som går under Solaris) och Palm-kompatibla handdatorer (native).

Kommer det att finnas en portning av AROS till PPC?
---------------------------------------------------

För närvarande så försöker vi utveckla en portning av AROS till PPC,
initialt hostat under Linux.

Varför använder ni Linux och X11?
---------------------------------

Vi använder Linux och X11 för att snabba upp utvecklingen. Som exempel, om du
implementerar en ny funktion för att öppna ett fönster så kan du enkelt skriva den
funktionen och inte behöva skriva hundratals andra funktioner i layers.library,
graphics.library, en bunt device driver och övriga som den funktionen kan tänkas behöva.

Målet med AROS är naturligtvis att bli oberoende av Linux och X11 (Men det skulle
fortfarande vara möjligt att köra på dessa om användare verkligen ville), det börjar
långsamt bli verklighet med native-verisonerna av AROS. Vi måste dock fortfarande 
använda Linux för utveckling, eftersom utvecklingsverktygen inte har blivit portade
till AROS ännu.

Hur ska ni lyckas med att göra AROS portabelt?
----------------------------------------------

En av de stora nya funktionerna i AROS jämfört med AmigaOS är HIDD (Hardware
Independent Device Drivers), som tillåter oss att porta AROS till olika
typer av hårdvara relativt enkelt. I princip så pratar libraries till 
operativsystemets kärna inte direkt med hårdvaran, utan går via HIDD. vilket är
kodat med hjälp av ett objektorienterat system som gör det enkelt att byta ut
HIDD och återanvända koden.

Varför tror ni att AROS kommer att lyckas?
------------------------------------------

Varje dag hör vi från massor av människor som tror att AROS inte kommer att lyckas.
De flesta vet inte vad vi egentligen håller på med eller att de tror att Amigan
redan är död. Efter att vi har förklarat vad vi sysslat med så håller de flesta med
om att det är möjligt, men det sistnämnda är svårare att förklara. Är Amigan död?
Dom som fortfarande använder Amigan kommer troligen säga att den inte är död.
Slutade din A500 eller A4000 att fungera när Commodore gick i konkurs? Gick den
sönder när Amiga Technologies konkursade?

Faktum är att det idag inte utvecklas så mycket ny mjukvara för Amiga (även om
Aminet fortfarande tuffar och går rätt så fint) och att ny hårdvara även utvecklas
mycket långsammare (men de coolaste pryttlarna verkar dyka upp nu).  Amigas Community
(Som fortfarande existerar) verkar sitta och vänta och om någon släpper en produkt som
liknar Amigan från 1984, då kommer den datorn att få en revival. Vem vet, kanske får du en
CD med din nya dator märkt med "AROS". :-)


Vad gör jag om AROS inte vill compileras?
-----------------------------------------

Skicka ett meddelande med detaljer (Till exempel, felmeddelandena som du får)
i hjälpforumet på `AROS-Exec`__ eller bli en utvecklare och prenumerera
på "AROS Developer list" och skicka meddelandet där, så får du hjälp.

__ http://aros-exec.org/


Kommer AROS ha minnesskydd (memory protection), SVM, RT, ...?
-------------------------------------------------------------

Flera hundra Amigaexperter (det är iallafall vad de säger om sig själva) försökte
för tre år sedan att finna en lösning för att implementera minnesskydd (MP) för
AmigaOS. Dom misslyckades. Faktum är att AmigaOS aldig kommer att ha MP som
Unix eller Windows NT.

Men man ska inte hoppa över ån förrens man sagt hej. Det finns planer att
integrera en variant av MP i AROS, som kommer tillåta minnesskydd för åtminstone nya
program med stöd för detta. En del försök med detta ser verkligen lovande ut. Är det även
ett stort problem om din dator krashar? Låt mig förklara innan du spikar upp mig
på ett träd. :-) Problemet är inte att datorn krashar, utan snarare:

1. Du har ingen aning om varför den krashade, egentligen så slutar det med att
du försöker peta med en 30 meter lång påle i ett träsk med tjock dimma.
2. Du tappar allt du jobbat med, omstart av datorn är inte något stort problem.

Något som vi kunde försöka konstruera är ett system som åtminstone varnar om
något suspekt händer och som kan säga dig i detalj om vad som hände när datorn
kraschade, som tillåter dig att spara ditt arbete och *sen* krascha. Det kommer
även finnas funktioner för att kontrollera vad som har sparats så att du kan vara
säker på att du inte får korrupt data.

Samma sak gäller för SVM (swappable virtual memory), RT (resource tracking)
och SMP (symmetric multiprocessing). Vi planerar för tillfället om hur vi ska
implementera dom, så vi är säkra på att lägga till dessa processer kommer att
bli relativt smärtfritt. Men, dom har inte högsta prioritet just nu, dock har 
en väldigt enkel RT utvecklats.


Kan jag bli betatestare?
------------------------

Absolut, inga problem. Faktiskt vill vi ha så många betatestare som möjligt,
så alla är välkomna! Vi för dock ingen lista över betatestare, så allt du
behöver göra är att tanka hem AROS, testa precis vad du vill och skicka oss
en rapport.

Vad har AROS och UAE för relation till varandra?
------------------------------------------------

UAE är en amiga-emulator, och har därför lite andra mål än vad AROS har.
UAE vill bli binär-kompatibel även för spel och kod med direktåtkomst till hårdvaran,
medans AROS vill ha native-applikationer. Därför är AROS mycket snabbare än
UAE, men du kan köra mer mjukvara i UAE.

Vi har viss kontakt med utvecklaren av UAE och därför finns det stora
möjligheter att koden för UAE kommer att finnas i AROS och vice versa. Till exempel,
UAE-utvecklarna är intresserade av källkoden i AROS eftersom UAE skulle kunna köra
applikationer mycket snabbare om en del OS-funktioner kunde ersättas med
native kod. Å andra sidan, AROS kan dra fördel av att ha en integrerad Amiga-emulator.

Eftersom de flesta program inte kommer att vara tillgängliga på AROS i början så
har Fabio Alemagna portat UAE till AROS så att du åtminstone kan köra gamla program i en
emuleringsbox.

Även `E-UAE`__ finns tillgängligt, vilket är UAE som är förbättrat med några
funktioner från `WinUAE`__.

__ http://www.rcdrummond.net/uae/
__ http://www.winuae.net/


Vad har AROS och Haage & Partner för relation till varandra?
------------------------------------------------------------

Haage & Partner har använt delar i AROS i AmigaOS 3.5 och 3.9, till exempel
Colorwheel och Gradientslider gadgets samt SetEnv-kommandot. I princip betyder
detta att AROS har blivit en del av det officiella AmigaOS. Detta betyder dock
inte att det finns en formell överenskommelse mellan AROS och Haage & Partner.
AROS är ett open source-projekt, därför kan vem som helst använda våran kod
i sina egna projekt förutsatt att de efterföljer licensavtalet.


Vad har AROS och MorphOS för relation till varandra?
----------------------------------------------------

Relationen mellan AROS och MorphOS är i princip densamma som mellan AROS
och Haage & Partner. MorphOS använder delar i AROS för att snabba upp deras
utveckling; enligt licensvillkoren. Precis som med Haage & Partner så är detta
bra för båda parter eftersom MorphOS kan snabba upp deras utveckling från AROS
och AROS i sin tur får förbättringar till vår källkod från MorphOS. Det finns
inget formellt avtal skrivet mellan AROS och MorphOS; detta är hur 
open source-utveckling fungerar.


Vilka programmeringsspråk finns tillgängliga?
---------------------------------------------

Mest utveckling i AROS sker med hjälp av ANSI C genom att crosskompila
källkoderna i ett annat operativsystem, som till exempel Linux eller FreeBSD.
Fabio alemagna har gjort klart en initial portning av GCC till i386 native. Men den
finns för tillfället inte i ISO:n eller integrerad i build-systemet.

De språk som finns tillgängliga i native är Python_, Regina_, Lua_, Hollywood: och False_:

+ Python är ett skriptspråk som har blivit ganska så populärt, pga designen
  och funktionerna (objektorienterad programmering, modult system, många
  användbara moduler inkluderade, ren syntax, ...) Ett separat projekt har
  startat för AROS-portning och kan hittas på
  http://pyaros.sourceforge.net/.

+ Regina är en portabel ANSI compilant REXX interpreter. Målet för AROS-portningen
  är att bli kompatibel med ARexx interpreter i AmigaOS.

+ Lua är en kraftfull, snabb, liten, embedded skriptspråk. AROS-portningen
  har blivit förbättrad med två moduler: Siamiga och Zulu. Siamiga har några enkla
  grafik-kommandon, Zulu är ett interface till Zune.

+ Hollywood är ett kommersiellt programmeringsspråk för multimediaapplikationer
  som inkluderar spel. CD-ROM:en innehåller en version för i386-AROS.

+ False kan klassifieras som ett exotiskt språk och kommer mest troligt att
  inte användas för seriös utveckling, men det är ganska kul. :-)

.. _Python: http://www.python.org/
.. _Regina: http://regina-rexx.sourceforge.net/
.. _Lua: http://www.lua.org/
.. _Hollywood: http://www.airsoftsoftwair.com/
.. _False:  http://wouter.fov120.com/false/


Varför finns det ingen m68k-emulator i AROS?
--------------------------------------------

För att kunna få gamla program att köras i AROS så har vi protat UAE: till AROS.
AROS version av UAE kommer troligtvis att vara lite snabbare än äldre versioner
av UAE eftersom AROS behöver färre resurser än andra operativsystem. (vilket betyder
att UAE kommer att få mer CPU-tid). Vi kommer även att försöka att patcha Kickstart ROM
i UAE för att ropa på AROS funktioner som ger en liten förbättring. Naturligtvis
så gäller detta endast native-versionerna av AROS och inte hosted.

Men varför implementerar vi inte en virtuell m68k CPU vilket gör att vi kan köra
mjukvaran direkt i AROS? Problemet är att m68k-mjukvara förväntar att datan att
vara i "big endian format" när AROS även kör "little endian CPU". Problemet är att
"little endian"-rutiner i AROS kärna skulle behöva arbete med "big endian"-data i
emuleringen. Atumatisk konvertering verkar i princip vara omöjligt (Till exempel:
Det finns ett fält i strukturen i AmigaOS vilket ibland innehåller ULONG och ibland
två WORD) eftersom vi inte kan säga hur ett par bytes i RAM är enkodade.

.. _UAE: http://www.freiburg.linux.de/~uae/


Kommer det att finnas en AROS Kickstart ROM?
--------------------------------------------

Eventuellt om någon skapar en native Amiga-portning av AROS och gör allt det andra
jobbet som behövs för att skapa en Kickstart ROM. För tillfället så är det ingen
som har ansökt om jobbet.

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

Copy the disk image to the DiskImages directory in AROS (SYS:DiskImages, e.g.
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

*The long-winded answer*
Graphics memory in i386-native signifies the lower 16MB of memory 
in the system. That lower 16MB is the area where ISA cards can do DMA. Allocating 
memory with MEMF_DMA or MEMF_CHIP will end up there, the rest in the other (fast)
memory.

Use C:Avail HUMAN command for memory info.


What do the Wanderer Snapshot <all/window> action actually do? 
--------------------------------------------------------------

This command remembers icon's placement of all (or one) windows.


How do I change the screensaver/background?
-------------------------------------------

At the moment the only way to change screensaver is to write your own one.
Blanker commodity could be tuned with Exchange, but it is able to do only 
"starfield" with given amount of stars.
Background of Wanderer is set by Pref tool Prefs/Wanderer.
Background of Zune Windows is set by Zune prefs Prefs/Zune. You can also set 
your chosen application preferences by using the Zune <application> command.


I've launched AROS-hosted but it's failed
-----------------------------------------

If you are root and AROS crashes at launch, do "xhost +" before 
"sudo && ./aros -m 20". You must also give it some memory with -m option as 
shown. The space between "-m" and the value is important. Also don't forget
about BackingStore option in section Device of your xorg.conf.


What are the command line options for AROS-hosted executable?
-------------------------------------------------------------

You can get a list of them by running ./aros -h command.


How can I make windows refresh from black on AROS-hosted?
---------------------------------------------------------

You must supply the following string (as is!) to the "Device" section of
your /etc/X11/xorg.conf (or XFree.conf)::
    
    Option  "BackingStore"

See Installation__ for details.

__ installation#running


What are the AROS-native kernel options used in GRUB line?
----------------------------------------------------------

Here's some::

    nofdc           - Disables the floppy driver completely.
    noclick         - Disables the floppy disk change detection (and clicking)
    ATA=32bit       - Enables 32-bit I/O in the hdd driver (safe)
    forcedma        - Forces DMA to be active in the hdd driver (should be safe, but       
                      might not be)
    gfx=<hidd name> - Use the named HIDD as the gfx driver
    lib=<name>      - Load and initiate the named library/HIDD

Please note that they are case-sensitive.


How can I transfer files to virtual machine with AROS?
------------------------------------------------------

First and simplest way is to put files to the ISO image and and connect it to VM. 
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

1) Create a sub-directory S and add a file with name 'Package-Startup' with the DOS 
commands to it.

2) Create a variable in the envarc:sys/packages file which contains the path to the S 
directory of your package.

Example directory layout::

    sys:Extras/myappdir
    sys:Extras/myappdir/S
    sys:Extras/myappdir/S/Package-Startup
    
The variable in envarc:sys/packages could have the name 'myapp' (name doesn't 
matter), the content would then be 'sys:extras/myappdir'

The Package-Startup script would then be called by the startup-sequence.
    

How do I clear the shell window? How do I set it permanently?
-------------------------------------------------------------

Type this command in the shell::

    Echo "*E[0;0H*E[J* "
    
You can edit your S:Shell-Startup and insert this line somewhere, so 
you'll have a new "Cls" command::

    Alias Cls "Echo *"*E[0;0H*E[J*" "

BTW: here is my new S:Shell-Startup modified to start the shell in black and 
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

AROS icons are actually renamed PNG files. But if you want icons in two states 
(normal/selected) use this command::

    join img_1.png img_2.png TO img.info
    

How to mount an ISO image on AROS? And can I update my nightly build this way?
------------------------------------------------------------------------------

+ Get the ISO into AROS (by wget or else way)
+ Copy the ISO into sys:DiskImages (drawer must be created if it isn`t exist). 
+ Rename ISO to Unit0 in that dir.
+ You must add this to your Devs:Mountlist ::

    ISO:
    FileSystem = cdrom.handler
    Device = fdsk.device
    Unit = 0

+ Then mount ISO:
  You can copy anything from ISO:. Additionally, you can create a script to update your 
  nightly build like this::

        copy ISO:boot/aros-pc-i386.gz sys:boot/
        copy ISO:C sys:C all quiet
        copy ISO:Classes sys:Classes all quiet
        copy ISO:Demos sys:Demos all quiet

And so on for each directory except Prefs, Extras:Networking/Stacks, and 
devs:mountlist itself. Prefs have to be kept if you want it. Also you can set 
AROSTcp to keep its settings in separate directory.

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

+ Call it somehow, PC0 for example. Set this file default tool to c:mount in 
  properties (or put mountfile to devs:dosdrivers or sys:storage/dosdrivers)
+ Double click on it.
+ Insert a FAT formatted floppy.
+ See the icon appearing on Wanderer`s desktop.


How to mount a real HD FAT partition with the FAT.handler?
----------------------------------------------------------

First you`d need to read the drive`s geometry and write down some values. 
You can use HDToolbox or Linux fdisk for that. The BlocksPerTrack value is taken 
from the sectors/track value. Note that it has absolutely nothing to do with 
the physical disk geometry - FAT only uses it as a multiplier.
If you get the Cylinders e.g. from HDToolbox or using the Linux fdisk like this::

    sudo fdisk -u -l /dev/hda, 
    
Then you'll need to set BlocksPerTrack=63. 
To ensure you have numbers in cylinders look for Units=Cylinders in output. If 
you got fdisk output in sectors (Units=sectors), set BlocksPerTrack=1.

LowCyl and HighCyl are partition`s cylinders like::

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

+ Call it somehow, FAT0 for example
+ Set this file`s default tool to c:mount in properties
  (or put mountfile to devs:dosdrivers or sys:storage/dosdrivers)
+ Double click on it
+ See the icon appearing on Wanderer`s desktop

Note: Formula for counting the blocks:
block = ((highcyl - lowcyl) x surfaces + head) x blockspertrack + sec


Hardware questions
==================

Where can I find an AROS Hardware Compatibility List?                   
-----------------------------------------------------

You can find one on the `AROS Wiki <http://en.wikibooks.org/wiki/Aros/Platforms/x86_support>`__ 
page. There can be another lists made by the AROS users.


Why AROS can't boot from my drive set as the SLAVE on IDE channel?
------------------------------------------------------------------

Well, AROS should boot if the drive is SLAVE but ONLY if there's a drive also 
on MASTER. That's appeared to be a correct connection respecting to the IDE 
specification, and AROS follows it.


My system hangs with red cursor on screen or a blank screen
-----------------------------------------------------------

One reason for this can be use of the serial mouse (these is not supported yet).
You must use PS/2 mouse with AROS at the moment. Another can be if you've chosen
a video mode unsupported by your hardware in boot menu. Reboot and try a different one.
