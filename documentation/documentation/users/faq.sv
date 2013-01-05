=====================
Frågor och svar (FAQ)
=====================

:Authors:   Aaron Digulla, Adam Chodorowski, Sergey Mineychev, AROS-Exec.org
:Copyright: Copyright © 1995-2007, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Done.

.. Contents::

Vanliga frågor
==============

Får jag ställa en fråga?
------------------------

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

Europeisk lag säger att det är lagligt att använda omvänd utvecklingsteknik 
(reverse engineering) för att få kompabilitet. Den säger även att det är
olagligt att distribuera kunskapen som man får av dessa tekniker. Det som
egentligen menas med detta är att du får dissemblera eller studera vilken
mjukvara som helst för att skriva ett program som är kompatibelt med detta
(till exempel så skulle det vara lagligt att dissemblera Word för att skriva
ett program som kan konvertera Word-dokument till ASCII-text).

Det finns naturligtvis undantag: du får inte dissemblera mjukvaran om informationen
som du är ute efter går att få tag på med andra sätt. Du får heller inte informera
andra om vad du har lärt dig. En bok med titeln "Windows inside" är därför
olaglig eller åtminstone tvivelaktigt laglig.

Eftersom vi undviker dissembleringstekniker och istället använder den kunskap
som redan finns (vilket inkluderar programmeringsmanualer) vilka inte går under
någon liknande lag, så kan man inte applicera detta med AROS. Det som räknas här
är intentionerna i lagen: det är lagligt att skriva mjukvara som är kompatibel
med annan mjukvara. Därför är våran övertygelse att AROS är skyddat av lagen.

Patent och header files är ett annat ämne. Vi kan använda patenterade algoritmer
i europa eftersom europeisk lag inte tillåter patent på algoritmer.
Dock får kod som använder algoritmer som är patenterade i USA inte importeras
till USA. Exempel på patenterade algoritmer i AmigaOS är t.ex. skärmdragning
och hur t.ex. menyer fungerar. Därför undviker vi att implementera dessa
funktioner på exakt samma sätt. Header files måste å andra sidan vara kompatibla
men så olika orginalet som möjligt.

För att undvika problem så har vi frågat om ett officiellt OK från Amiga Inc. De
är ganska positiva till vårat arbete men känner sig väldigt obekväma angående den lagliga
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
vi visste att vi kunde hantera. Sen när vi har erfarenheter för att se vad som
är möjligt eller inte, så kan vi besluta om förbättringar.

Vi vill även ha binär kompabilitet med AmigaOS. Anledningen till
detta är just att ett nytt operativsystem utan program inte har någon chans att
överleva. Därför försöker vi att få övergången från AmigaOS till det nya att gå
så smärtfritt som möjligt (men inte till den grad att vi inte kan förbättra AROS
i efterhand). Som vanligt, allting har sitt pris och vi försöker att göra genomtänkta
beslut om vilket pris som det kostar och om alla andra är villiga att betala det.

Kan ni inte implementera funktionen XYZ?
----------------------------------------

Nej, därför: 

a) Om det verkligen är så viktigt så borde det finnas i AmigaOS. :-) 
b) Varför inte göra det själv och skicka patchen till oss?

Anledningen till denna attityd är att det finns väldigt många som tycker att deras
funktion är viktigast och att AROS inte har någon framtid om inte funktionen 
omedelbart implementeras. Vår ståndpunkt är att AmigaOS, som AROS siktar på att
implementera, kan göra allting som ett modernt operativsystem kan göra. Vi ser
att det finns områden där AmigaOS skulle behöva förbättras inom, men om vi gör det,
vem skulle skriva resten av operativsystemet? I slutändan så skulle vi då ha en massa
fina förbättringar jämfört med AmigaOS som skulle göra det mycket svårare att använda
redan existerande mjukvara, eftersom resten av operativystemet skulle saknas.

Därför har vi beslutat att vänta med varje försök till att implementera stora
nya funktioner i operatisystemet tills att operativsystemet är mer eller mindre
klart. Vi har kommit ganska så nära målet nu och det har faktisktutvecklats en del funktioner
i AROS som inte finns tillgängligt i AmigaOS.


Hur kompatibelt är AROS med AmigaOS?
------------------------------------

Väldigt kompatibelt. Vi förväntar oss att AROS kommer att kunna köra existerande
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


Vad gör jag om AROS inte vill kompileras?
-----------------------------------------

Skicka ett meddelande med detaljer (Till exempel, felmeddelandena som du får)
i hjälpforumet på `AROS-Exec`__ eller bli en utvecklare och prenumerera
på "AROS Developer list" och skicka meddelandet där, så får du hjälp.

__ http://aros-exec.org/


Kommer AROS ha minnesskydd (memory protection), SVM, RT, ...?
-------------------------------------------------------------

Flera hundra Amigaexperter (det är iallafall vad de säger om sig själva) försökte
för tre år sedan att finna en lösning för att implementera minnesskydd (MP) för
AmigaOS. Dom misslyckades. Faktum är att AmigaOS aldrig kommer att ha MP som
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
behöver göra är att tanka hem AROS, testa precis vad du vill och skicka 
en rapport till oss.

Vad har AROS och UAE för relation till varandra?
------------------------------------------------

UAE är en Amiga-emulator, och har därför lite andra mål än vad AROS har.
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
ingen formell överenskommelse mellan AROS och MorphOS; detta är hur 
open source-utveckling fungerar.


Vilka programmeringsspråk finns tillgängliga?
---------------------------------------------

Mest utveckling i AROS sker med hjälp av ANSI C genom att crosskompila
källkoderna i ett annat operativsystem, som till exempel Linux eller FreeBSD.
Fabio Alemagna har gjort klart en initial portning av GCC till i386 native. Men den
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
.. _False:  http://strlen.com/false-language


Varför finns det ingen m68k-emulator i AROS?
--------------------------------------------

För att kunna få gamla program att köras i AROS så har vi portat UAE: till AROS.
AROS version av UAE kommer troligtvis att vara lite snabbare än äldre versioner
av UAE eftersom AROS behöver färre resurser än andra operativsystem. (vilket betyder
att UAE kommer att få mer CPU-tid). Vi kommer även att försöka att patcha Kickstart ROM
i UAE för att ropa på AROS funktioner som ger en liten förbättring. Naturligtvis
så gäller detta endast native-versionerna av AROS och inte hosted.

Men varför implementerar vi inte en virtuell m68k CPU vilket gör att vi kan köra
mjukvaran direkt i AROS? Problemet är att m68k-mjukvaran förväntar att datan ska
vara i "big endian format" när AROS även kör "little endian CPU". Problemet är att
"little endian"-rutiner i AROS kärna skulle behöva arbeta med "big endian"-data i
emuleringen. Automatisk konvertering verkar i princip vara omöjligt (Till exempel:
Det finns ett fält i strukturen i AmigaOS vilket ibland innehåller ULONG och ibland
två WORD) eftersom vi inte kan säga hur ett par bytes i RAM är enkodade.

.. _UAE: http://www.amigaemulator.org/


Kommer det att finnas en AROS Kickstart ROM?
--------------------------------------------

Eventuellt om någon skapar en native Amiga-portning av AROS och gör allt det andra
jobbet som behövs för att skapa en Kickstart ROM. För tillfället så är det ingen
som har ansökt om jobbet.

Mjukvarufrågor
==============

Hur accessar jag AROS disk-images från UAE?
-------------------------------------------

Diskett-imagen kan mountas som en fil på hårddisken och sen användas som en
1.4 MB hårddisk i UAE. Efter att du har lagt i filerna i disk-imagen 
(Eller vad du nu vill göra), så kan du skriva den till en diskett.

Geometrin i disk-imagen är enligt nedan::

    Sectors    = 32
    Surfaces   = 1
    Reserved   = 2
    Block Size = 90


Hur accessar jag AROS disk images från hosted-versioner av AROS?
----------------------------------------------------------------

Kopiera disk-imagen till Diskimages-mappen i AROS (SYS:DiskImages, 
bin/linux-i386/AROS/DiskImages) och döp om den till "Unit0". Efter att ha
startat AROS så kan du mounta imagen med::

    > mount AFD0: 


Vad är Zune?
------------

Om det är på denna hemsida som du läst om Zune, så är det egentligen bara
en open-source återimplementation av MUI, vilket är ett kraftfullt
(som i användar- och -utvecklingsvänligt) objektorienterad shareware
GUI toolkit för att utveckla native AROS-applikationer med. Angående
namnet i fråga, så betyder det ingenting, det låter bara bra.


Hur kan jag återställa mina inställningar (Prefs) till default?
---------------------------------------------------------------

I AROS, öppna ett CLI-fönster, gå till Envarc: och ta bort relevanta filer
för den inställning (pref) som du vill få tillbaka till default.

Vad är Graphical(Grafiskt) och other(annat) memory(minne) i Wanderer?
---------------------------------------------------------------------

Denna minnesupdelning är mest en relik från Amigans ursprung, när grafiskt minne
var applikationsminne innan du lade till mer minne, FAST RAM, ett minne där applikationerna
hamnade, medans grafiken, ljudet och en del system-strukturer fortfarande residerade
i grafikminnet.

I AROS-hosted så finns det inte något minne som Other (FAST), endast GFX, medans
det finns på Native AROS, GFX kan ha max 16MB, men detta återspeglar ej minnesstorleken på
grafikkortet... Det har ingen koppling till hur stort minnet är på ditt grafikkort.

*Det utförligare svaret*
Grafikminnet i i386-native visar det undre 16MB minnet i systemet. De undre 16MB är
i området där ISA-kort kan utföra DMA. Allokering av minne med MEMF_DMA eller MEF_CHIP
kommer att hamna där, resterande hamnar i other (fast) -minnet.

Använd C:Avail HUMAN -kommandot för minnes-info.


Vad gör egentligen Wanderer Snapshot <all/window>? 
--------------------------------------------------

Detta kommando sparar ikonernas placering av alla (eller ett) fönster.


Hur ändrar jag skärmsläckare/bakgrundsbild?
-------------------------------------------

För tillfället är det enda sättet att ändra skärmsläckare att skriva din egen.
Blanker commodity kan ändras med Exchange, men den finns endast till för
att ändra "starfield" med hur många stjärnor man vill ha.
Bakgrundsbilden i Wanderer ställs in med Pref-verktyget Prefs/Wanderer.
Bakgrundsbilden i Zune Windows ställs in med Zune-verktyget Prefs/Zune

Jag har startat AROS-hosted med den hänger sig
----------------------------------------------

Om du är root och AROS krashar vid uppstart, kör "xhost +" innan
du kör "sudo && ./aros -m 20". Du måste även ge programmet minne med -m
optinen enl. instruktion. Mellanslaget mellan "-m" och värdet är viktigt.
Glöm även inte BackingStore-valen i sektionen Device i din xorg.conf.


Vad finns det för command line options för AROS-hosted exekverbara filer?
-------------------------------------------------------------------------

Du kan få en lista på dessa genom att köra ./aros -h kommandot.


Hur kan jag få fönsterna att uppdateras från svart på AROS-hosted?
------------------------------------------------------------------

Du måste skriva nedanstående sträng (precis som den är!) till "Device"-delen
av din /etc/X11/xorg.conf (eller Xfree.conf)::
    
    Option  "BackingStore"

Läs Installation__ för detaljer.

__ installation#running


Vad finns det för optioner till AROS-native kernel i GRUB line?
---------------------------------------------------------------

Här är några::

    nofdc           - Avaktiverar floppy driver fullständigt.
    noclick         - Avaktiverar floppy disk change detection (och klickande)
    ATA=32bit       - Aktiverar 32-bit I/O i hdd driver (säkert)
    forcedma        - Tvingar DMA att vara aktivt i hdd driver (borde vara säkert, men inte 100%)
    gfx=<hidd name> - Använder namngiven HIDD som gfx-drivrutin
    lib=<name>      - Laddar och initierar namngett library/HIDD

Kom ihåg att kommandona är skiftlägeskänsliga (case-sensitive)


Hur överför jag filer till en virtuell dator med AROS?
------------------------------------------------------

Det första och enklaste sättat är att lägga i filer i ISO-imagen och ansluta den
till VM. Det finns massvis med program som man kan använda för att skapa/editera
ISO som t.ex. UltraISO, WinImage, eller mkisofs. Nummer två är att sätta upp ett
nätverk i AROS och en ftp-server på din lokala dator.  Då kan du använda
ftp-klienten i AROS för att överföra filer (leta efter MarranoFTP). Det kan vara
ganska så krångligt. Användardokumentationen innehåller ett kapitel om nätverk,
kolla i denna. Nu finns det även ett lovande verktyg (AFS Util) som gör det 
möjligt att läsa (går inte att skriva ännu) filer från AROS AFFS/OFS hårddiskar och
disketter.


Kompileringsfel
---------------

Q: Jag har kompilat AROS med gcc4 men sett kompilerade AROS-hosted segfaults 
med -m > 20, och om jag kompilerar AROS-native så startar den inte (svart skärm)

A: Lägg till -fni-strict-aliasing till scripts/aros-gcc.in och försök kompila igen.


Är det möjligt att göra ett DOS-skript som automatiskt körs när ett paket(package) är installerat?
--------------------------------------------------------------------------------------------------

Det här skriptet borde göra en del assigns, lägg även till värderna i PATH.

1) Skapa ett underbibliotek S och lägg till en fil med namnet 'Package-Startup med DOS
kommando till det.

2) Skapa en variabel i envarc:sys/packages -filen som innehåller sökvägen till S-biblioteket.

Exempel på mappstruktur::

    sys:Extras/myappdir
    sys:Extras/myappdir/S
    sys:Extras/myappdir/S/Package-Startup
    
Variablen i envarc:sys/packages kan ha namnet 'myapp' (namn spelar ingen roll),
innehållet är sedan 'sys:extras/myappdir'

Package-Startup-skriptet blir sedan anropat av startup-sequence.
    

Hur rensar jag shell-fönstret? Hur gör jag det permanent?
---------------------------------------------------------

Skriv detta kommandi i shell::

    Echo "*E[0;0H*E[J* "
    
Du kan editera ditt s:Shell-Startup och lägga till denna rad någonstans, så
att du har ett nytt "Cls" kommand::

    Alias Cls "Echo *"*E[0;0H*E[J*" "

Här är förresten mitt egen s:Shell-Startup modifierat för att starta shell i svart
och med en modifierad prompt::

    Alias Edit SYS:Tools/Editor
    Alias Cls "Echo *"*E[0;0H*E[J*" "
    Echo "*e[>1m*e[32;41m*e[0;0H*e[J"
    Prompt "*n*e[>1m*e[33;41m*e[1m%N/%R - *e[30;41m%S>*e[0m*e[32;41m "
    date

Lite om printer escape sequences::

    Esc[0m
    Standard Set

    Esc[1m and Esc[22m
    Fetstil

    Esc[3m and Esc[23m
    Kursiv

    Esc[4m and Esc[24m
    Understruket

    Esc[30m to Esc[39m
    Välj front-färg

    Esc[40m to Esc[49m
    Välj bakgrundsfärg

Med värderna menas::

    30 grå tecken    -- 40 grå cell   -- >0 grå bakgrund   ---- 0 alla attribut av
    31 svarta tecken  - 41 svart cell  - >1 svart bakgrund  --- 1 fetstil
    32 vita tecken    - 42 vit cell    - >2 vit bakgrund    --- 2 faint
    33 blå tecken    -- 43 blå cell   -- >3 blå bakgrund   ---- 3 kursiv
    34 grå tecken    -- 44 grå cell   -- >4 grå bakgrund   ---- 4 underscore
    35 svarta tecken  - 45 svart cell  - >5 svart bakgrund  --- 7 reverse video
    36 vita tecken    - 46 vit cell    - >6 vit bakgrund    --- 8 osynlig
    37 blå tecken    -- 47 blå cell   -- >7 blå bakgrund

Koderna kan kombineras genom att separera dom med semikolon.


Hur startar jag AROS-hosted i helskärm?
---------------------------------------

Anropa "export AROS_X11_FULLSCREEN=1" i ett shell. Starta AROS och ändra
skärmupplösningen i screenmode preferenses. Avsluta AROS och starta igen.


Hur gör jag 2-status AROS ikoner?
---------------------------------

AROS-ikoner är faktiskt omdöpta PNG-filer. Men om du vill ha ikoner i 2-status
(normal/vald) använd detta kommando::

    join img_1.png img_2.png TO img.info
    

Hur mountar jag en ISO-image i AROS? Kan jag uppdatera nightly build på detta sätt?
-----------------------------------------------------------------------------------

+ Lägg in ISO:n i AROS (med hjälp av wget eller annat)
+ Kopiera ISO:n till sys:DiskImages (mappen måste bli skapad om den inte finns).
+ Döp om ISO:n till Unit0 i den mappen.
+ Du måste lägga till detta till din Devs:Mountlist ::

    ISO:
    FileSystem = cdrom.handler
    Device = fdsk.device
    Unit = 0

+ Mounta sedan ISO:n:
  Du kan kopiera allting från ISO:. Du kan även skapa ett skript för att uppdatera dina
  nightly builds::

        copy ISO:boot/aros-pc-i386.gz sys:boot/
        copy ISO:C sys:C all quiet
        copy ISO:Classes sys:Classes all quiet
        copy ISO:Demos sys:Demos all quiet

Och så vidare för varje mapp förutom Prefs, Extras:Networking/Stacks, och
devs:mountlist. Prefs måste behållas om du vill ha det. Du kan även ställa in
AROSTcp att spara inställningarna i en separat mapp.

Om du vill skriva över allting::

    copy ISO:C sys:C all quiet newer  
    
Hur gör jag en unmount på en volym?
-----------------------------------

Kör dessa två kommandon i CLI::
    
    assign DOSVOLUME: dismount
    assign DOSVOLUME: remove

där DOSVOLUME är DH0:, DF0:, etc.


Hur mountar jag en FAT floppy med FAT.handler?
----------------------------------------------

Skapa en mountfile (textfil) med de 3 magiska raderna::

    device = trackdisk.device
    filesystem = fat.handler
    unit = 0

+ Anropa med t.ex. PCO. Sätt denna fils default tool till c:mount i properties
  (eller lägg mountfile i devs:dosdrivers eller sys:storage/dosdrivers)
+ Dubbelklicka på filen
+ Sätt i en FAT-floppy.
+ Se ikonen framträda på Wanderer skrivbordet.


Hur mountar jag en HD FAT partition med FAT.handler?
----------------------------------------------------

Först så måste du läsa hårddiskens geometri och skriva ner värdena.
Du kan använda HDToolbox eller Linux fdisk. BlocksPerTrack-värdet tas från
sectors/track-värdet. Notera att det inte har någonting att göra med den fysiska
diskens geometri -  Fat använder endast detta som en multiplier.
Om du kan få värderna för antal cylindrar från HDToolbox eller med hjälp av
Linux fdisk::

    sudo fdisk -u -l /dev/hda, 
    
Sen så måste du ange värderna BlocksPerTrack=63
För att vara säker på värderna om cylindrar, leta efter Units=Cylinders. Om 
du har fått fdisk att visa resultatet i sektorer (sectors)(Units=sectors), ange
värdet BlocksPerTrack=1.

LowCyl och HighCyl är partitionens cylindrar::

    mark@ubuntu:~$ sudo fdisk -l -u /dev/hda
    ...
    /dev/hda1 * 63 20980889 10490413+ c W95 FAT32 (LBA)

Sammanfattningsvis, LowCyl är 63 och HighCyl är 20980889, blockspertrack=1

Skapa en mountfile (textfil) med dessa rader::

    
    device = ata.device
    filesystem = fat.handler,
    Unit = 0

    BlocksPerTrack = 1
    LowCyl = 63
    HighCyl = 20980889
    Blocksize=512

+ Anropa den på valfritt sätt, FAT0 till exempel
+ Sätt värderna på filens defautl toll till c:mount i properties
  (eller lägg mountfile i devs:dosdrivers eller sys:storage/dosdrivers)
+ Dubbelklicka på filen
+ Se ikonen framträda på Wanderers skrivbord

Notering: Formel för att räkna antal blocks:
block = ((highcyl - lowcyl) x surfaces + head) x blockspertrack + sec


Hårdvarufrågor
==============

Var kan jag hitta en AROS Hardware Compability List?                   
----------------------------------------------------

Du kan finna en på `AROS Wiki <http://en.wikibooks.org/wiki/Aros/Platforms/x86_support>`__ .
Det kan även finnas andra listor av AROS-användare.

Varför kan inte AROS boota från hårddisken om hårddisken är satt som SLAVE?
---------------------------------------------------------------------------

AROS kan boota om hårddisken sitter på SLAVE med ENDAST om det även sitter en
hårddisk på MASTER. Detta är en korrekt anslutning vilket efterföljer IDE-specifikationerna,
och AROS efterföljer dessa.

Min dator hänger sig med en röd markör på skärmen eller en svart skärm
----------------------------------------------------------------------

En anledning till detta kan vara att man använder en seriell mus (dessa är inte supportade
ännu). Du måste använda PS/2-mus med AROS för tillfället. En annan anledning kan vara
att du valt en upplösning i boot-menyn som inte är stöds av din hårdvara. Starta om
och testa med en annan.
