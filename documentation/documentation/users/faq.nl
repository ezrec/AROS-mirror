==========================
Veel gestelde vragen (FAQ)
==========================

:Authors:   Aaron Digulla, Adam Chodorowski, Sergey Mineychev, AROS-Exec.org
:Copyright: Copyright Ò 1995-2004, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Done.

.. Note:: De Nederlandse vertaling van deze pagina is momentaal onder constructie

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


Zal er AROS port komen voor de PPC? 
-----------------------------------

Er wordt momenteel werk ondernomen om AROS te porten naar PPC,
initieel als een gehoste versie onder Linux.


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


Will AROS have memory protection, SVM, RT, ...?
-----------------------------------------------

Several hundred Amiga experts (that's what they thought of themselves at least)
tried for three years to find a way to implement memory protection (MP) for
AmigaOS. They failed. You should take it as a fact that the normal AmigaOS will
never have MP like Unix or Windows NT.

But all is not lost. There are plans to integrate a variant of MP into AROS
which will allows protection of at least new programs which know about it. Some
efforts in this area look really promising. Also, is it really a problem if your
machine crashes? Let me explain, before you nail me to a tree. :-) The problem
is not that the machine crashes, but rather: 

1. You have no good idea why it crashed. Basically, you end up having to poke 
   with a 100ft pole into a swamp with a thick fog. 
2. You lose your work. Rebooting the machine is really no issue.

What we could try to construct is a system which will at least alert if
something dubious is happening and which can tell you in great detail what was
happening when the machine crashed and which will allow you to save your work
and *then* crash. There will also be a means to check what has been saved so you
can be sure that you don't continue with corrupted data.

The same thing goes for SVM (swappable virtual memory), RT (resource tracking)
and SMP (symmetric multiprocessing). We are currently planning how to implement
them, making sure that adding these features will be painless. However, they do
not have the highest priority right now. Very basic RT has been added, though.


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
bin/linux-i386/AROS/DiskImages) en hernoem deze naar "Unit0". Na het starten van
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

*The longwinded answer*
Graphics memory in i386-native signifies the lower 16MB of memory 
in the system. That lower 16MB is the area where ISA cards can do DMA. Allocating 
memory with MEMF_DMA or MEMF_CHIP will end up there, the rest in the other (fast)
memory.

Use C:Avail HUMAN command for memory info.

What do the Wanderer Snapshot <all/window> action actually do? 
--------------------------------------------------------------

This conmmand remembers icon`s placement of all (or one) windows.

How do I change the screensaver/background?
-------------------------------------------

At the moment the only way to change screensaver is to write your one.
Blanker commodity could be tuned with Exchange, but it is able to do only 
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

Wat zijn de AROS-native kernal opties voor de GRUB CLI?
-------------------------------------------------------

Dit zijn er enkele::

    nofdc - Schakelt de floppy driver uit.
    noclick - Schakelt de floppy disk verandering detectie uit (en het klikken)
    ATA=32bit - Schakelt 32-bit I/O aan in de hdd driver (veilig)
    forcedma - Forceerd DMA om actief te zijn in de hdd driver (zou veilig moeten zijn, 
    maar niet gegarandeerd)
    gfx=<hidd name> - Gebruik de genoemde hidd als gfx driver
    lib=<name> - Laad en init de genoemde library/hidd

Deze zijn hoofdletter gevoelig. 

How can I transfer files to virtual machine with AROS?
------------------------------------------------------

First and simpliest way is to put files to the ISO image and and connect it to VM. 
There`s a lot of programs able to create/edit ISO`s like UltraISO, WinImage, 
or mkisofs. Second, you can set up the network in AROS and FTP server on your 
host machine. Then you can use FTP client for AROS to transfer files
(look for MarranoFTP). This is tricky enough to stop at this point. User 
documentation contains a chapter about networking, go for it.


Hardware vragen
===============

Waar kan ik een AROS Hardware Compatibiliteits lijst vinden?
------------------------------------------------------------

U kunt er één vinden op de `AROS Wiki <http://en.wikibooks.org/wiki/Aros/Platforms/x86_support>`__ 
pagina. Er kunnen ook andere lijsten zijn gemaakt door AROS gebruikers (meer informatie volgt).

Waarom kan AROS niet van een IDE harddrive in SLAVE mode starten? 
-----------------------------------------------------------------

Well, AROS zou moeten booten als de drive in SLAVE mode draait MITS er ook
een drive als MASTER aangesloten is. Dit blijkt de correctie verbindingmethode 
te zijn volgens de IDE specificatie, welke AROS volgt.

Mijn systeem hangt met een rode cursor op een (leeg) scherm
-----------------------------------------------------------

Één reden hiervoor kan het gebruik van een seriele muis zijn (deze worden nog 
niet ondersteund). U moet voorlopig nog een PS/2 muis gebruiken met AROS. Een andere
reden kan zijn dat u in het bootmenu een video modus hebt gekozen die niet 
ondersteund wordt door uw hardware. Reboot en probeer een andere modus.