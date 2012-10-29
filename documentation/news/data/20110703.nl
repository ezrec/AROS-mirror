=======================
Een jaar in vogelvlucht
=======================

:Author:   Neil Cafferkey
:Date:     2011-07-03

Wederom heeft het nieuws lang op zich laten wachten, voornamelijk omdat 
we te druk bezig zijn geweest met features en verbeteringen voor AROS. 
Het laatste jaar is er in feite zoveel gebeurd dat hieronder alleen de 
hoogtepunten beschreven worden.

AROS 68K en andere nieuwe platformen
------------------------------------

Een belangrijk nieuw hoofdstuk in de geschiedenis van AROS is 
aangebroken met de ontwikkeling van een stand-alone versie voor originele 
Amiga-computers. Dit zou ook interessant moeten zijn voor mensen die 
WinUAE gebruiken om oude Amiga-applicaties te draaien. De 
uitwisselbaarheid is zodanig dat de AROS-ROM-image zelfs originele 
Workbench-schijven kan opstarten, zo oud als V1.3. Deze ontwikkeling zal 
hopelijk ook leiden tot een verbeterde integratie van originele Amiga-
software in AROS voor x86 -en andere platformen.

De platformondersteuning van AROS is tevens in andere richtingen verder 
uitgebreid. Er is een Linux-hosted ARM-conversie verschenen, en nieuwe 
hosted-versies voor MAC OS X ondersteunen drie verschillende CPU-
architecturen. En AROS kan binnenkort nog een stuk 'handzamer' worden met 
de recente ontwikkelingen van de nieuwe IOS- en Android-conversies.

Grafisch
--------

Op het gebied van de PC-compatibiliteit en X86 is er ook een hoop 
gebeurd, vooral op grafisch gebied. Het stuurprogramma voor de grafische 
ondersteuning voor Intel GMA is uitgebreid om een grotere 
verscheidenheid aan chipsets te ondersteunen en werkt, 
naast voor externe beeldschermen, nu ook voor LCD-schermen van laptops. 
Het op Nouveau en Gallium gebaseerde stuurprogramma voor de grafische 
kaarten van nVidia is ook verbeterd: Naast de hogere snelheid en 
verbeterde hardwarematige ondersteuning is het tevens het eerste 
stuurprogramma met hardwarematig-versnelde alpha-blending.

Een opmerkelijke grafische feature uit ons Amiga-erfgoed heeft ook in 
AROS zijn opwachting gemaakt: de meeste inherente grafisch 
stuurprogramma's ondersteunen nu het slepen en schuiven van schermen. 
Daaronder vallen zowel onze stuurprogramma's voor nVidia- en Intel GMA-
chipsets als het generieke VESA-stuurprogramma.

Afgezien van de grafisch stuurprogramma's heeft AROS ook een aantal 
extra grafische libraries gekregen: egl.library en openvg.library. 
Daarvan biedt de eerste universele interfaces voor GL-initialisatie en 
-vensterbeheer, terwijl de tweede een library voor vectorafbeeldingen is 
die versneld wordt door Gallium. Onze conversie van de SDL-library is 
ook verder bijgeschaafd, met verbeteringen van functionaliteit zoals 
SDL-naar-GL-integratie, wat zorgt voor een verbeterde speel-ervaring.

Draadloos netwerken
-------------------

Twee veelbetekenende mijlpalen zijn bereikt met netwerkondersteuning 
voor laptops. Ten eerste is een moderne en beveiligde netwerk-stack aan 
AROS toegevoegd.  Het eerste stuurprogramma dat deze infrastructuur 
gebruikt, ondersteunt verschillende populaire kaarten met Atheros 
chipsets. Ten tweede is ondersteuning toegevoegd voor een grote 
verscheidenheid aan USB-apparaten voor mobiel breedband.

Andere verbeteringen
--------------------

Het ondersteunen van debuggen en het melden van fouten zijn verbeterd. 
Misbruik en wanbeheer van geheugen en andere hulpbronnen door 
applicaties wordt nu veel sneller gedetecteerd en foutmeldingen bevatten 
nu stack-traces die aangeven waar de fout is opgetreden.

Aan verschillende onderdelen zijn heel wat verbeteringen aangebracht, 
zowel in het gebruiksgemak als aan de onderdelen zelf. De handler van 
het FAT-bestandssysteem heeft een aantal belangrijke foutverbeteringen 
gehad. Inherente grafisch stuurprogramma's worden nu over het algemeen 
automatisch geactiveerd voor grafische kaarten die worden herkend en de 
AC97- en HDAudio-geluidsdrivers hoeven normaliter voor gebruik niet meer 
met de hand geconfigureerd te worden. En eindelijk zit AROS niet meer 
opgescheept met een onveranderbare muiswijzer: de gebruikte standaard-
wijzer kan nu worden ingesteld met een programma waar de voorkeur van de 
muiswijzer kan worden aangegeven en applicaties kunnen de muiswijzer 
aanpassen naar gelang de situatie. De AROS shell is geavanceerder 
geworden, en heeft nu een schuifbare uitvoergeschiedenis, alsmede 
ondersteuning voor meer modi en zijn er een aantal fouten hersteld. 
AROS' afhandeling van GUI thema's is ook beduidend sneller geworden. 
Tenslotte is er weer een stukje van de puzzel opgelost m.b.t. de 
backwards-compatibiliteit, door de transformatie van Regina naar een 
veel completere AREXX-kloon.

Verdere ontwikkelingen
----------------------

AROS heeft er nu een derde distributie bijgekregen, naast de regelmatig 
bijgewerkte `Icaros Desktop <http://www.icarosdesktop.org/>`__ en `AROS 
Broadway <http://www.aros-broadway.de/>`__. De nieuwe `AspireOS-
distributie <http://www.arosaspire.org/>`__ richt zich voornamelijk op 
de Acer Aspire One 110 en 150 netbooks, maar kan ook interessant zijn 
voor gebruikers van andere pc's die op zoek zijn naar een lichtgewicht-
distributie.

Afgelopen maanden zijn vele applicaties en spellen aan de software-
selectie van AROS toegevoegd. Enkele hoogtepunten van applicaties zijn 
Amifig, Portrekkr en Open Universe. 3D spellen zoals Cube, AssaultCube en 
Super Tux Kart (met een populaire Kitty kart!) maken goed gebruik van de 
grafische 3D verbeteringen in AROS, terwijl ook veel 2D-spelen, zoals 
Giana's Return en Mega Mario, toegevoegd zijn. We hebben ook een aantal 
emulatoren toegevoegd voor nog meer retro-speel-plezier.
