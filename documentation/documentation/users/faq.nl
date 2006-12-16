==========================
Veel gestelde vragen (FAQ)
==========================

:Authors:   Aaron Digulla, Adam Chodorowski, Sergey Mineychev, AROS-Exec.org
:Copyright: Copyright Ò 1995-2004, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Done.

.. Contents::

Algemene vragen
===============

Mag ik een vraag stellen?
-------------------------
Natuurlijk kunt u dat. Gaat u a.u.b. naar het `Aros-Exec forum <http://aros-exec.org/modules/newbb/viewtopic.php?topic_id=1636&start=0>`__
, lees het betreffende topic en vraag alles wat u wilt (liefst in het Engels). Deze FAQ zal geupdate worden met meer
gebruikers vragen, maar het forum is en blijft de meest up-to-date plek.


Waar gaat AROS over?
--------------------

Leest u a.u.b. de introductie_.

.. _introductie: ../../introduction/index


Wat is de legale status van AROS?
---------------------------------

De Europese wet verteld dat het legaal is om 'reverse engineering' technieken
te gebruiken om onderlinge compatibiliteit te bevorderen. Het zegt ook dat
het illegaal is om de zo verkregen kennis te verspreiden. Dit betekend dat het 
toegestaan is om alle software te disambleren om een programma te schrijven dat
compatibel is (ter voorbeeld: het is legaal om Word de disambleren om een programma
te schrijven dat Word documenten in ASCII teksten omvormt.)

Er zijn natuurlijk beperkingen: het is niet toegestaan om software te disambleren
als de te verkrijgen informatie ook via andere wegen verkregen kan worden. Ook mag
je anderen niet vertellen wat je geleerd hebt. Een boek getiteld "Windows van binnen"
is daarom illegaal en heeft op zijn minst een dubieuze legale status.  

Gezien we disambleer technieken vermijden en gewoon algemeen beschikbare kennis
gebruiken (waaronder programmeer handleidingen), welke onder geen NDA 
(geheimhoudingsverklaring) vallen, heeft het bovenstaande niet direct betrekking op AROS. 
Wat hier telt is de intentie van de wet: het is legaal om software te schrijven 
welke compatibel is met een ander stuk software. 
Daarom geloven we dat AROS beschermd is door de wet.

Patenten en header files zijn echter een ander geval. We kunnen gepatenteerde
algoritmes in Europa gebruiken sinds de Europese wet hierop geen patenten toestaat. 
Echter, code die algoritmen gebruikt die in de VS gepatenteerd zijn, mogen niet 
geïmporteerd worden in de VS. Voorbeelden van gepatenteerde algoritmes
in AmigaOS zijn het slepen van schermen en de manier waarop bepaald menu's werken.
We vermijden daarom deze functies op precies dezelfde manier te implementeren.
Header files zijn een uitzondering: deze moeten compatibel zijn, maar zoveel
mogelijk verschillen van het origineel. 

Om problemen te voorkomen hebben we een officieel OK gevraagd van Amiga Inc. Zij
zijn vrij positief over ons project, maar ook wel een beetje ongemakkelijk met 
de legale implicaties. We willen u suggereren dat het gegeven dat Amiga Inc ons 
geen "stop" brieven gestuurd heeft als een positief teken gezien mag worden.
Dan nog zijn er helaas geen rigide legale afspraken gemaakt, ondanks goede intenties
van beiden partijen.


Waarom doelen jullie alleen op compatibiliteit met 3.1?
-------------------------------------------------------

Er zijn discussies gevoerd over het schrijven van een geavanceerd OS met de 
features van AmigaOS. Dit hebben we echter niet gedaan om goede redenen. Ten eerste:
iedereen is het eens dat het huidige AmigaOS verbeterd zou moeten worden,
maar niemand weet hoe, laat staan wat er verbeterd moet worden. 
We willen bijvoorbeeld graag geheugen bescherming, maar weten niet wat de prijs
daarvoor zal zijn (het herschrijven van alle software en bijkomende snelheid vertraging).

Uiteindelijk eindigden de discussies ofwel in flame wars of een het
eindeloos herhalen van dezelfde argumenten. Dus besloten we om te beginnen
met iets waarvan we wisten hoe we ermee om moesten gaan. Om daarna, 
als we de ervaring hadden om te oordelen wat wel of niet mogelijk was, 
te beslissen over de verbeteringen.

Ook willen we AROS binair compatibel houden met het originele AmigaOS op de Amiga. 
De reden hiervoor is dat een nieuw OS zonder enig programma om erop te draaien 
gewoonweg geen overlevingskans heeft. Daarom proberen we de overstap van het 
originele OS naar het nieuwe zo pijnloos mogelijk te maken (echter niet in dermate dat 
we AROS later niet kunnen verbeteren). Zoals gewoonlijk heeft elk ding zijn prijs 
ofwel "elk voordeel heeft zijn nadeel". We proberen daarom voorzichtig in te 
schatten wat de eventuele nadelen zijn en of iedereen die acceptabel vind.


Kunnen jullie feature XYX implementeren?
----------------------------------------

Nee, omdat:

a) Als het echt belangrijk was, had het ook in het originele OS gezeten. :-)
b) Waarom doet u dit zelf niet en stuurt u de patch naar ons?

De reden voor deze attitude is dat er veel mensen zijn die denken dat hun 
toepassing de meest belangrijke is en dat AROS zonder het inbouwen daarvan 
geen toekomst zou hebben. Onze houding is dat het AmigaOS, 
dat AROS probeert te implementeren, alles
kan doen wat een modern OS kan doen. We weten dat er gebieden zijn waar het AmigaOS
verbeterd kan worden, maar als we die eerst zouden maken, wie schrijft dan de rest van 
het OS? Uiteindelijk zouden we een hoop mooie verbeteringen hebben in het originele
AmigaOS, die vervolgens de ondersteuning van alle beschikbare software zouden breken 
en deze waardeloos maken, gezien de rest van het OS ontbreekt.

Daarom hebben we ervoor gekozen alle pogingen te blokkeren om grote nieuwe features 
in het OS te bouwen, tot het tijdstip waarop deze min of meer compleet zal zijn. 
We zijn nu bijna bij dat punt, terwijl er inmiddels ook alweer een paar nieuwe 
innovaties in AROS zijn ingebouwd waarover het originele OS niet beschikte.


Hoe compatibel is AROS met AmigaOS?
-----------------------------------

Zeer compatibel. We verwachten dat AROS zonder problemen bestaande software op 
een Amiga zal draaien. Voor andere hardware zal de bestaande software echter 
gehercompileerd moeten worden. We zullen een pre-processor aanbieden die je kan
gebruiken met je eigen code, welke code zal veranderen die AROS kan laten
vastlopen of op zijn minst waarschuwt voor deze code.

Overzetten van programma's van AmigaOS naar AROS is op moment vooral een kwestie
van simpel hercompileren, met een enkele aanpassing aan de code. Er zijn 
natuurlijk programma's waarvoor dit niet opgaat, maar het geld wel voor de 
meeste moderne software. 


Voor welke hardware architecturen is AROS op moment beschikbaar?
----------------------------------------------------------------

Op moment is AROS beschikbaar in een vrij bruikbare staat als native
en hosted versie (onder Linux en FreeBSD) voor de i386 architectuur (bijv. IBM
PC AT compatibele klonen). Er zijn ports in de maak voor meer systemen,
elk in een verschillende mate van compleetheid. Hiertoe behoren een port naar 
de SUN SPARC (gehost onder Solaris) en Palm compatibele handhelds (native).


Zal er een AROS port komen voor de PPC? 
---------------------------------------

Er wordt momenteel gewerkt aan een AROS port voor PPC,
voorlopig dan alleen nog als gehoste versie onder Linux.


Waarom gebruiken jullie Linux en X11?
-------------------------------------

We gebruiken Linux en X11 om de ontwikkeling te versnellen. Ter voorbeeld: het 
implementeren van een nieuwe functie om een venster te openen kan simpelweg via
één enkele functie worden gedaan, zonder het moeten schrijven van honderden functies 
in de layers.library, graphics.library en een reeks van
andere device drivers die deze functie misschien zou aanroepen. 

Het doel van AROS is natuurlijk om onafhankelijk te draaien van Linux en X11 
(maar het zou er nog steeds op kunnen draaien als mensen dit echt wilden), wat
nu langzaam realiteit wordt met de native versies van AROS. Voorlopig is 
Linux nog wel nodig voor de ontwikkeling, gezien sommige ontwikkelaars
tools nog niet geport zijn naar AROS.


Hoe willen jullie AROS overdraagbaar maken?
-------------------------------------------

Een van de grote nieuwe features in AROS in vergelijking met AmigaOS is de
HIDD (Hardware Onafhankelijke Device Drivers) systeem, dat ons toestaat AROS
vrij makkelijk naar andere hardware over te zetten. In essentie roepen de kern
OS libraries niet meer rechtstreeks de hardware aan, maar doen dit via de
HIDDs. Deze zijn geprogrammeerd volgens een object georiënteerd systeem dat het 
makkelijk maakt HIDDs te vervangen en code te hergebruiken.


Waarom denken jullie dat AROS het zal maken?
--------------------------------------------

We horen bijna dagelijks van mensen dat AROS het niet zal maken. De meeste
van hen weten om te beginnen al niet wat we doen, of denken dat de Amiga al 'dood' is. 
Nadat we eerstgenoemde verduidelijken denken de meesten dat ons werk toch nog haalbaar 
is. Maar het laatstgenoemde is lastiger uit te leggen: is de Amiga nu dood? 
Degenen die hun Amiga nog gebruiken zullen je waarschijnlijk vertellen van niet. 
En kritisch gezegd: ging je A500 of A4000 kapot toen Commodore bankroet ging? 
Gebeurde dit toen Amiga Technologies ten onder ging?

Het feit dat er nog altijd een klein beetje nieuwe software ontwikkeld wordt 
voor de Amiga (al weet Aminet nog altijd zeer veel te zien) en dat de hardware
nog altijd met een vertraagd tempo ontwikkeld word (de meest indrukwekkende dingen 
verschijnen deze dagen).
De Amiga gemeenschap (die nog altijd levend is) lijkt af te wachten. En als iemand
een product uit zou geven dat een beetje is zoals de Amiga was terug in 1984, dan 
zal die machine ongetwijfeld weer populariteit genieten. En wie weet: 
misschien krijgt u bij die machine ook wel een CD gelabeld "AROS". :-)


Wat te doen als het compileren van AROS niet lukt?
--------------------------------------------------

Plaatst u a.u.b. een bericht met aanvullende details (bijvoorbeeld
de error boodschap(pen) die u krijgt) in het Help forum op `Aros-Exec`__.
U kunt ook ontwikkelaar worden en uzelf abonneren op de AROS ontwikkelaars lijst 
en daar het tegengekomen probleem posten. In beiden gevallen zal iemand u proberen
te helpen.

__ http://aros-exec.org/


Krijgt AROS geheugen bescherming, SVM, RT, ...?
-----------------------------------------------

Vele honderden Amiga experts (tenminste, dat dachten ze van zichzelf)
hebben drie jaar lang geprobeerd een methode te ontwikkelen om geheugen bescherming
(MP) in AmigaOS toe te passen. Het lukte ze niet. U zult het gewoon als feit moeten 
accepteren dat het gewone AmigaOS nooit MP zal krijgen zoals Unix of Windows NT.

Maar niet alles is verloren. Er zijn plannen om een variant van MP in AROS
te integreren, die bescherming toestaat aan nieuwe programma's die hierop berekend
zijn. Enkele van de ontwikkelingen in dit gebied lijken veelbelovend. Toch, kritisch
bekeken, is het eigenlijk wel een probleem als uw machine vastloopt? Laat ons uitleggen,
voordat u geïrriteerd raakt. :-) Het probleem is eigenlijk niet dat de machine 
vastloopt, maar dat:

1. U heeft geen idee waarom deze vastliep. Kortgezegde komt dit erop neer
   dat u met een naald in een hooiberg zoekt.
2. U verliest uw werk. De machine herstarten is in verhouding een klein probleem.

Wat we wel kunnen proberen is een systeem te maken dat op zijn minst waarschuwt
als iets dubieus aan de hand is, wat u daarna verteld wat er gebeurde toen de
machine vastliep én u tevens toestaat uw werk op te slaan om *daarna* te crashen. 
Er zal ook een mogelijk komen om hetgeen u opgeslagen hebt te controleren 
zodat u verder kunt werken zonder kapotte data.

Hetzelfde geld voor SVM (swappable virtual memory), RT (resource tracking) en
SMP (symmetric multiprocessing). We plannen op moment hoe we deze willen 
implementeren, zodat het toevoegen van deze features uiteindelijk vlekkeloos zal
verlopen. Desondanks hebben deze nu niet de hoogste prioriteit. Een simpele
variant van RT is desondanks al geïmplementeerd. 


Kan ik een beta tester worden?
------------------------------

Natuurlijk, geen probleem. Beter nog, we willen zoveel mogelijk beta testers
als mogelijk, dus iedereen is welkom! We houden overigens geen lijst van beta
testers bij: u hoeft alleen AROS te downloaden, testen wat u wilt
en ons daarna een rapport sturen.


Wat is de relatie tussen AROS en UAE?
-------------------------------------

UAE is een Amiga emulator en heeft daardoor andere doelstellingen dan AROS.
UAE streeft ernaar óók binair compatibel te zijn voor spellen en hardware aansprekende
code, terwijl AROS ten doel heeft eigen native applicaties te hebben. Daardoor
is AROS sneller dan UAE, maar kan er aan de andere kant weer meer software 
gedraaid worden onder UAE.

We hebben los contact met de maker van UAE, wat de kans groot maakt dat er
code van UAE in AROS zal verschijnen en visa versa. Ter voorbeeld: de UAE ontwikkelaars
zijn geïnteresseerd in de broncode van het OS omdat UAE sommige applicaties veel
sneller zou kunnen draaien als sommige of alle OS functies vervangen konden worden
met native code. AROS op haar beurt kan profijt trekken van een geïntegreerde
Amiga emulatie.

Gezien de meeste programma's vanaf de start niet beschikbaar waren voor AROS, heeft
Fabio Alemagna een port van UAE gemaakt naar AROS zodat u al uw oude programma's
op zijn minst in een emulatie box kan draaien.


Wat is de relatie tussen AROS en Haage & Partner?
-------------------------------------------------

Haage & Partner hebben delen van AROS gebruikt in AmigaOS 3.5 en 3.9, waaronder
het kleurenwiel, de kleurverloop-slider gadgets en het SetENV commando. Dit betekend
dat AROS, op een manier, deel is geworden van het officiële AmigaOS. Het wil 
echter niet zeggen dat er een formele relatie is tussen AROS en Haage & Partner.
AROS is een open source project, waarvan iedereen de code in eigen projecten mag
gebruiken -indien- zij de de licentie volgen. 


Wat is de relatie tussen AROS en MorphOS?
-----------------------------------------

De relatie tussen AROS en Morphos is eigenlijk dezelfde als tussen AROS en 
Haage & Partner. MorphOS gebruikt delen van AROS om hun ontwikkeling te versnellen;
onder de regels van onze licentie. Zoals met Haage & Partner heeft dit
voordeel voor beide teams: het MorphOS team krijgt zo een versnelling in
hun ontwikkeling dankzij AROS, terwijl het AROS team de goede verbeteringen 
mag overnemen van het MorphOS team. Er is dus geen formele relatie tussen 
AROS en MorphOS; dit is simpelweg hoe open source ontwikkeling werkt.


Welke programmeer talen zijn beschikbaar?
-----------------------------------------

Het grootste deel van de ontwikkeling voor AROS gebeurd met ANSI C, door
de bronnen te crosscompilen onder verschillende besturingssystemen zoals
Linux of FreeBSD. Fabio Alemagna heeft een vroege GCC port gemaakt voor
i386 native. Deze is echter nog niet bijgevoegd op de ISO of geïntegreerd in het
build systeem.

Tot de talen die native beschikbaar zijn behoren Python_, Regina_ en False_:

+ Python is een scripting taal die erg populair geworden is, o.a. vanwege het goede
  ontwerp en de features (object-georiënteerd programmeren, module systeem, 
  veel bijgesloten en handige modules, opgeruimde syntax, ...). Een apart
  project is inmiddels gestart voor de AROS port en kan gevonden worden op 
  http://pyaros.sourceforge.net/.

+ Regina is een overzet bare ANSI compliant REXX interpreter.
  Het doel van de AROS port is om compatibel te zijn met de ARexx interpreter van
  het klassieke AmigaOS.

+ False kan geclassificeerd worden als een exotische taal, waardoor deze niet
  direct in aanmerking komt voor serieuze ontwikkeling werk, al kan het werken
  met deze taal erg leuk zijn. :-) 

.. _Python: http://www.python.org/
.. _Regina: http://regina-rexx.sourceforge.net/
.. _False:  http://wouter.fov120.com/false/

Waarom zit er geen m68k emulator in AROS?
-----------------------------------------

Om oude Amiga programma's te draaien hebben we UAE_ geport naar AROS. AROS's
versie van UAE zal zelfs nog iets sneller draaien dan andere versies, gezien
AROS minder middelen verbruikt dan andere besturing systemen (wat betekend dat
UAE meer processor tijd krijgt). Wel willen we proberen om de Kickstart ROM
te patchen in UAE, zodat deze AROS functies aanspreekt wat een kleine prestatie
verbetering zal geven. Uiteraard gaan genoemde zaken alleen op voor de native 
versies van AROS en niet de hosted varianten.

Maar waarom implementeren we niet gewoon een m68k CPU virtualiser? Wel, het probleem
is dat de m68k software verwacht dat alle data in het zogeheten 'big endian' formaat komt,
terwijl AROS ook op 'little endian' CPU's draait. Het probleem ontstaat dan dat de 
little endian routines in de AROS core ook met grote endian routines zouden moeten 
werken tijdens de emulatie. Automatische conversie lijkt onmogelijk (ter voorbeeld:
er is een veld in een structuur van het AmigaOS dat de ene keer een enkele ULONG bevat
en de andere keer twee WORDs), omdat we niet kunnen weten hoe een paar bytes in
het ram gecodeerd worden.

.. _UAE: http://www.freiburg.linux.de/~uae/


Zal er een AROS Kickstart ROM komen?
------------------------------------

Heel misschien, als iemand een native Amiga port van AROS maakt en al het werk
uitvoert om een Kickstart ROM te maken. Tot zover heeft echter nog niemand zich
opgegeven voor deze taak.


Software vragen
===============

Hoe krijg ik toegang tot AROS' disk images vanuit UAE?
------------------------------------------------------

De floppy disk image kan als hardfile gemount worden en dan als een 1.4 MB
harddisk gebruikt worden binnen UAE. Nadat u de bestanden naar de hardfile
disk image gekopieerd hebt (of iets anders dat u wilde doen), kan deze naar een floppy
geschreven worden.

De geometrie van de hardfile is als volgt::

    Sectors    = 32
    Surfaces   = 1
    Reserved   = 2
    Block Size = 90


Hoe benader ik AROS's disk images vanuit hosted versies van AROS?
-----------------------------------------------------------------

Kopieer de disk image naar de DiskImages map in AROS (SYS:DiskImages, bijv: 
bin/linux-i386/AROS/DiskImages) en hernaam deze naar "Unit0". Na het starten van
AROS kan deze disk gemount worden met::

    > mount AFD0: 


Wat is Zune?
------------

In geval je op deze site de naam Zune gelezen hebt: het is een open source
implementatie van MUI, wat een krachtige (als in gebruikers- en ontwikkelaars-
vriendelijk) object-georiënteerd shareware GUI toolkit is, tevens de-facto standaard
onder AmigaOS. Zune is de geprefereerde GUI toolkit voor de ontwikkeling van 
native AROS applicaties. En betreft de naam zelf, het betekend niets, maar klinkt
goed.


Hoe kan ik de Prefs naar standaard herstellen? 
-----------------------------------------------

Open in AROS de CLI shell, ga naar Envarc: en wis de relevante bestanden
voor de pref die u naar de standaard instellingen wilt herstellen.

Wat is Grafisch en ander geheugen in Wanderer?
----------------------------------------------

Deze geheugen verdeling is vooral een reliek uit het Amiga verleden, toen
grafisch geheugen het primaire geheugen was totdat je ander geheugen toevoegde,
dat als FAST RAM werd betiteld. Het FAST RAM was daarna het geheugen dat de 
applicaties gebruikten, terwijl grafische objecten, geluiden en enkele systeem 
structuren in het grafisch geheugen bleven.

In AROS-hosted bestaat er geen ander geheugen dan Anders (FAST), maar alleen GFX,
terwijl AROS-native een GFX maximum hanteert van 16MB. Weet wel dat dit geen 
enkele reflectie bevat op de geheugenstaat van uw grafische adapter, laat staan
de hoeveel geheugen die uw video adapter heeft. 

*Het langdradige antwoord*

Grafisch geheugen in i386-native duid de lagere 16MB van het systeemgeheugen aan.
Deze 'lagere' 16MB is de ruimte waar ISA kaarten hun DMA regelen. Het alloceren
van geheugen met MEMF_DMA of MEMF_CHIP zal hierbij gevoegd worden, de rest bij
het overige (FAST) geheugen.

Gebruik het C:Avail HUMAN CLI commando voor meer geheugen informatie. 

Wat doet de Wanderer Snapshot <all/window> actie eigenlijk? 
-----------------------------------------------------------

Dit commando onthoud de icoon positie voor alle (of één) vensters.

Hoe verander ik de screensaver/achtergrond?
-------------------------------------------

De enigste methode om de screensaver te veranderen -op moment- is door er zelf 
één te schrijven. De Blanker commodity kan getuned worden met Exchange, maar het
kan alleen maar een "sterrenveld" tonen met een gegeven aantal sterren.
De achtergrond in Wanderer wordt ingesteld via de Pref tool Prefs/Wanderer.
De achtergrond van Zune Windows wordt ingesteld via de Zune prefs Prefs/Zune. U
kunt ook specifieke applicatie instellingen vastleggen via het Zune <applicatie> 
commando.

Ik heb AROS-hosted gelanceerd maar deze faalde
----------------------------------------------

Dit kan waarschijnlijk verholpen worden door een WBStartup directory te maken
in de AROS directory. Als u als root werkt en AROS crashed bij het opstarten,
doe "xhost +" alvorens "sudo && ./aros -m 20". U moet ook geheugen toewijzen
met de -m optie zoals getoond. Vergeet tot slot ook niet de BackingStore optie
toe te voegen in de sectie Device van uw xorg.conf.

Wat zijn de command line opties voor de AROS-hosted executable?
---------------------------------------------------------------

U kunt hiervan een lijst krijgen door het runnen van ./aros -h command.

Hoe kan ik een window refresh van zwart maken in AROS-hosted?
-------------------------------------------------------------

U moet de volgende string (zoals weergegeven!) toevoegen aan uw
/etc/X11/xorg.conf (of Xfree.conf)::
    
    Option  "BackingStore"

Wat zijn de AROS-native kernel opties voor de GRUB CLI?
-------------------------------------------------------

Dit zijn er enkele::

    nofdc - Schakelt de floppy driver uit.
    noclick - Schakelt de floppy disk verandering detectie uit (en het klikken)
    ATA=32bit - Schakelt 32-bit I/O aan in de hdd driver (veilig)
    forcedma - Forceert DMA om actief te zijn in de hdd driver (zou veilig moeten zijn, 
    maar niet gegarandeerd)
    gfx=<hidd name> - Gebruik de genoemde hidd als gfx driver
    lib=<name> - Laad en init de genoemde library/hidd

Deze zijn hoofdletter gevoelig. 

Hoe kan ik bestanden kopiëren naar een virtuele machine met AROS?
-----------------------------------------------------------------

De eerste en simpelste manier om de bestanden op een ISO image te zetten en
deze met de VM te mounten. Er zijn veel programma's die het maken/aanpassen van
ISO's toestaan, om enkele te noemen: UltraISO, WinImage of mkisofs. Tweede methode
is om een netwerk tussen AROS en een FTP server op te zetten op uw hosted machine.
U kunt dan de FTP client voor AROS gebruiken om bestanden over te zetten (zoek
naar MarannoFTP). Dit is echter complex genoeg om hier nu te stoppen; 
de gebruikers documentatie bevat een hoofdstuk over netwerken, 
beter dat u daar kijkt.


Hardware vragen
===============

Waar kan ik een AROS Hardware Compatibiliteit lijst vinden?
-----------------------------------------------------------

U kunt er één vinden op de `AROS Wiki <http://en.wikibooks.org/wiki/Aros/Platforms/x86_support>`__ 
pagina. Er kunnen ook andere lijsten zijn gemaakt door AROS gebruikers (meer informatie volgt).

Waarom kan AROS niet van een IDE harddrive in SLAVE mode starten? 
-----------------------------------------------------------------

Wel, AROS zou moeten booten als de drive in SLAVE mode draait MITS er ook
een drive als MASTER aangesloten is. Dit blijkt de correctie verbindingsmethode 
te zijn volgens de IDE specificatie, welke AROS volgt.

Mijn systeem hangt met een rode cursor op een (leeg) scherm
-----------------------------------------------------------

Één reden hiervoor kan het gebruik van een seriële muis zijn (deze worden nog 
niet ondersteund). U moet voorlopig nog een PS/2 muis gebruiken met AROS. Een andere
reden kan zijn dat u in het bootmenu een video modus hebt gekozen die niet 
ondersteund wordt door uw hardware. Reboot en probeer een andere modus.