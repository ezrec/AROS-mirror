=====================================
Ännu ett (ej kalender) år i återblick
=====================================

:Författare:   Neil Cafferkey
:Datum:        2012-10-21

Ännu ett år har passerat. Ett år av programmering, istället för nyhets uppdatering, 
ytterligare en samling höjdpunkter att förmedla.

Ny Web Läsare
-------------

AROS användbarhet tog ett stort steg frammåt i.o.m. att den modernare och web standard 
kompatibla Odyssey Web Browser (OWB) portades över till AROS. Baserad på WebKit motorn, 
inkluderar Odyssey en JIT Javaskript kompilator och har stöd för surfning via flikar, HTML5,
CSS, SVG och SSL. Odyssey erbjuder även med sitt  grafiska användar gränssnitt (GUI),                                          
hantering av bokmärken, historik, lösenords hantering med mera.


Plattformar
-----------

Supporten för ARM arkitekturen har fortsatt att mogna och variera sig, via nattliga                                              
uppdateringar så finns det numera Linux underbyggda AROS ARM system tillgängliga.                                                
Ett speciellt intresse finns i att använda AROS på ARM baserade Raspberry Pi system,                                                     
Vilket nu börjar bli möjligt då en Linux understödd AROS variant har börjat se dagens ljus                                               
för just detta system. 

Vidare så har arbetet med andra plattformar för AROS fortsatt. Den windows 
underbyggda versionen, har fått ett antal programfel åtgärdade. Och är numera 
betydligt stabilare, samtidigt som kompatibiliteten med original Amigan har 
fortsatt att förbättrats.

Nätverk
-------

Trådlös nätverks hantering har mognat i.o.m. två nya WPA kompatibla drivrutiner
Den ena för  Realtek RTL8187B baserade USB nätverkskort. Den andra är en 
uppdaterad version av Prism-II drivrutinen, som var den första drivrutinen för trådlösa
nätverk redan 2005.  Ett grafik baserat hjälpmedel för att skanna och ansluta till trådlösa 
nätverk har också sett dagens ljus under året.

Mobilt bredband är numera betydligt enklare att konfigurera och använda. Fler
USB enheter och telefoner stöds. AROS kan numera hantera delade SMB enheter
dock begränsas detta till äldre windows och linux versioner samt NAS enheter.

Andra förbättringar
-------------------

Begynnande skrivarstöd har initierats till AROS. Komponenter till dom nya strukturerna
inkluderar ett grafiskt gränssnitt, en postscript skrivar rutin samt flera traditionella 
tillbehör som ex. en grafisk fildump och en Printfiles funktion. Utskrifter kan omdirigeras 
till USB, Parallell eller Serieport eller alternativt till en fil.

Vår Intel GMA drivrutin har fått 3D support för vissa chip revisioner, dock har GMA hårdvaran 
begränsade möjligheter jämfört med modernare nVidia och AMD kort. Många äldre spel 
(av vilka flera har portats på senare tid ) fungerar dock bra. Vi har också fått en OpenGL drivrutin 
till Linux underbyggda 3D grafik system och en uppdaterad nVidia drivrutin.

Utbudet av ljud drivrutiner tillgängliga för AROS har förbättrats, bl.a. med drivrutiner till 
ES137x och CMI8738 ljud chippen. Den förstnämnda är speciellt intressant då den erbjuder 
utgående ljud under VMWare. Vår HD Audio drivrutin har också fått en bredare kompatibilitet 
vad gäller ljudåtergivning och inspelnings lägen.


Förbättringar har gjorts gällande diskåtkomst från AROS. Vi har nu en AHCI drivrutin som 
tillhandahåller grundläggande SATA tjänster på flera moderna maskiner. Tilläggas kan även att 
standard windows partitoner nu kan läsas, via en tidig version av en NTFS hanterare och 
skrivhastigheten till FAT partitoner har förbättrats. Nya open source CD brännar programmet Frying Pan 
är nu en standard komponent i AROS programmet

Sist men inte minst vårt "papercuts" initiativ ledde till att många mindre men irriterande programfel
har rättats till i hela AROS systemet.


Vidare utveckling
-----------------

AROS har nu fått sin första utgåva för original (MC680x0) Amiga plattformar.
`AROS Vision <http://www.natami-news.de/html/aros_vision.html>`__
Såväl som till  AROS systemet självt, AROS Vision inkluderar många fritt distribuerbara 3:e parts 
AmigaOS systemkomponenter och applikationer

En annan ny AROS utgåva är `AEROS <http://www.aeros-os.org/>`__ som riktar sig till att kombinera 
det bästa från AROS och Linux i en väl samansvetsad miljö. För nuvarande finns versioner för x86 och 
ARM baserade system.
