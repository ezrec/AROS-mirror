========================================================
Wederom een (niet-kalender-geliëerd) jaar in vogelvlucht
========================================================

:Author:   Neil Cafferkey
:Date:     2012-10-21

Wederom een jaar voorbij, wederom een jaar van programmeren in plaats van
het schrijven van nieuws, wederom een opsomming van de hoogtepunten.

Nieuwe web browser
------------------

Het gebruik van AROS heeft grote vooruitgang geboekt met de conversie van
de moderne en aan standaarden-houdende Odyssey web browser. Gebaseerd op de
basis van WebKit, bevat Odyssey een JIT-Javascript compiler en ondersteund
tabbladen tijdens het surfen, HTML5, CSS, SVG en SSL. Er zijn ook veel
instellingsmogelijkheden, met een grafische gebruikersinterface die het
mogelijk maakt om bookmarks, cookies, inhoudsblokkades, het surfverleden,
wachtwoorden en overige zaken te beheren.

Platformen
----------

Ondersteuning voor de ARM-architectuur is verder doorontwikkeld, met
nachtelijke compilaties voor Linux-hosted versies van AROS die nu beschikbaar
zijn. Er is in het bijzonder interesse onder gebruikers om AROS op de
ARM-gebaseerde Raspberry Pi te draaien, al kan dat momenteel alleen
Linux-hosted.

Ook zijn er verdere ontwikkelingen voor andere AROS platformen. Vele fouten
zijn verwijderd uit de Windows-hosted versie en is nu veel stabieler, terwijl
de compatibiliteit, van de versie voor de oorspronkelijke Amiga met
betrekking tot klassieke software, steeds beter wordt.

Netwerken
---------

Ondersteuning voor draadloze netwerken is gegroeid, met de toevoeging van
twee nieuwe WPA-ondersteunende stuurprogramma's: één is voor de op Realtek
RTL8187B-gebaseerde USB-apparaten, en de andere is een vernieuwde versie
van het Prism-II stuurprogramma dat AROS in 2005 voor het eerst draadloos
netwerken bracht. Ook is er een tool met grafische interface geïntroduceerd
welke dynamisch kan zoeken naar en verbinden met draadloze netwerken.

Mobiel breedband is nu veel eenvoudiger in te stellen, en ook extra
USB-apparaten en telefoons worden ondersteund. En AROS kan nu SMB-gedeelde
schijven aanmelden, alhoewel deze mogelijkheid momenteel gelimiteerd is tot
oudere versies van Windows, alsmede Linux en stand-alone NAS-apparatuur,

Andere verbeteringen
--------------------

De eerste stappen voor het ondersteunen van printen in AROS zijn
geïntroduceerd. Onderdelen van het nieuwe systeem omvatten een grafisch
voorkeursinstellingenprogramma, een stuurprogramma voor een PostScript
printer en verscheidene traditionele utilities zoals PrintFiles en
GraphicDump. Uitvoer kan worden gericht naar USB, een paralelle- of
seriëele poort of naar een bestand.

Ons stuurprogramma voor Intel-GMA heeft 3D-ondersteuning gekregen voor
bepaalde revisies van de chip. Hoewel de GMA-hardware meer beperkte
3D-mogelijkheden heeft dan recente Nvidia- en AMD-kaarten, werken veel oudere
spellen (waarvan velen de laatste tijd zijn geconverteerd) prima. We hebben
er ook een OpenGL-stuurprogramma voor 3D-graphics in Linux-hosted
bijgekregen, alsmede een vernieuwd Nvidia stuurprogramma.

Het keuzeaanbod van audio-stuurprogramma's voor AROS is verbeterd, met nieuwe
stuurprogramma's die beschikbaar zijn gekomen voor de ES137x en
CMI8738 geluidschips. De eerstgenoemde is belangrijk vanwege het feit dat
deze audio-uitvoer aanbiedt onder VMWare.
Ons HDAudio-stuurprogramma heeft ook een grotere compatibiliteit gekregen
bij zowel het afpelen- als opnemen.

Er zijn verbeteringen om vanuit AROS toegang te krijgen tot harde schijven.
We hebben nu een AHCI-stuurprogramma dat SATA-ondersteuning biedt op veel
moderne computers. Daarnaast kunnen nu ook standaard Windows-partities 
worden gelezen door gebruik te maken van een vroege versie voor de
ondersteuning van het NTFS-bestandsysteem, en de snelheid van het schrijven
naar FAT-partities is verbeterd. De onlangs verschenen vrije-broncode van de
Frying Pan CD-brander-software is nu ook een standaard onderdeel van AROS.

Als laatste, maar zeker niet minste, heeft ons Papercuts-initiatief
geleid tot het verbeteren van vele en vervelende foutjes die in AROS
aanwezig waren.

Verdere ontwikkelingen
----------------------

AROS heeft nu zijn eerste distributie voor het oorspronkelijke (MC680x0)
Amiga platform, `AROS Vision <http://www.natami-news.de/html/aros_vision.html>`__.
Net zoals AROS zelf bevat AROS Vision vele vrij-verpreidbare AmigaOS 
systeem-componenten en applicaties van derden.

Een andere nieuwe AROS distributie is `AEROS <http://www.aeros-os.org/>`__, en
tracht de beste eigenschappen van AROS en Linux te combineren door ze samen te
voegen in een naadloze omgeving. Er zijn momenteel versies voor x86 -en
ARM-computers.

