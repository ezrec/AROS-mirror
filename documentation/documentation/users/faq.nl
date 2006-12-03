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
gebruiken (waaronder programeer handleidingen), welke onder geen NDA 
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


Why are you only aiming for compatibility with 3.1?
---------------------------------------------------

There have been discussions about writing an advanced OS with the features of
the AmigaOS. This has been dropped for a good reason. First, everyone agrees
that the current AmigaOS should be enhanced, but no one knows how to do that or
even agrees on what has to be enhanced or what is important. For example, some
want memory protection, but don't want to pay the price (major rewrite of the
available software and speed decrease).

In the end, the discussions ended in either flame wars or reiteration of the
same old arguments over and over again. So we decided to start with something we
know how to handle. Then, when we have the experience to see what is possible or
not, we decide on improvements.

We also want to be binary compatible with the original AmigaOS on Amiga. The
reason for this is just that a new OS without any programs which run on it has
no chance to survive. Therefore we try to make the shift from the original OS to
our new one as painless as possible (but not to the extent that we can't improve
AROS afterwards). As usual, everything has its price and we try to decide
carefully what that price might be and if we and everyone else will be willing
to pay it.


Can't you implement feature XYZ?
--------------------------------

No, because: 

a) If it was really important, it would be in the original OS. :-) 
b) Why don't you do it yourself and send a patch to us?

The reason for this attitude is that there are plenty of people around who think
that their feature is the most important and that AROS has no future if that
feature is not built in right away. Our position is that AmigaOS, which AROS
aims to implement, can do everything a modern OS should do. We see that there
are areas where AmigaOS could be enhanced, but if we do that, who would write
the rest of the OS? In the end, we would have lots of nice improvements to the
original AmigaOS which would break most of the available software but be worth
nothing, because the rest of the OS would be missing.

Therefore, we decided to block every attempt to implement major new features in
the OS until it is more or less completed. We are getting quite close to that
goal now, and there have been a couple of innovations implemented in AROS that
aren't available in AmigaOS.


How compatible is AROS with AmigaOS?
------------------------------------

Very compatible. We expect that AROS will run existing software on the Amiga
without problems. On other hardware, the existing software must be recompiled.
We will offer a preprocessor which you can use on your code which will change
any code that might break with AROS and/or warn you about such code.

Porting programs from AmigaOS to AROS is currently mostly a matter of a simple
recompilation, with the occasional tweak here and there. There are of course
programs for which this is not true, but it holds for most modern ones.


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
één enkele functie worden gedaan, zonder het schrijven van honderden functies 
in de layers.library, graphics.library en een reeks van
andere device drivers die deze functie misschien zou moeten gebruiken. 

The goal for AROS is of course to be independent of Linux and X11 (but it would
still be able to run on them if people really wanted to), and that is slowly
becoming a reality with the native versions of AROS. We still need to use Linux
for development though, since some development tools haven't been ported to AROS
yet.


How do you intend to make AROS portable?
----------------------------------------

One of the major new features in AROS compared to AmigaOS is the HIDD (Hardware
Independent Device Drivers) system, which will allow us to port AROS to
different hardware quite easily. Basically, the core OS libraries do not hit the
hardware directly but instead go through the HIDDs, which are coded using an
object oriented system that makes it easy to replace HIDDs and reuse code.


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
zal die machine ongetwijfeld weer populairiteit genieten. En wie weet: 
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


What is the relation between AROS and UAE?
------------------------------------------

UAE is een Amiga emulator en heeft daardoor andere doelstellingen dan AROS.
UAE streeft ernaar óók binair compitable te zijn voor spellen en hardware aansprekende
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


What is the relation between AROS and Haage & Partner?
------------------------------------------------------

Haage & Partner hebben delen van AROS gebruikt in AmigaOS 3.5 en 3.9, waaronder
het kleurenwiel, de kleurverloop-slider gadgets en het SetENV commando. Dit betekend
dat AROS, op een manier, deel is geworden van het officiele AmigaOS. Het wil 
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
The languages that are available natively are Python_, Regina_ and False_:

+ Python is een scripting taal die erg populair geworden is, o.a. vanwege het goede
  ontwerp en de features (object-georiënteerd programmeren, module systeem, 
  veel bijgesloten en handige modules, opgeruimde syntax, ...). Een apart
  project is inmiddels gestart voor de AROS port en kan gevonden worden op 
  http://pyaros.sourceforge.net/.

+ Regina is een overzetbare ANSI compliant REXX interpreter.
  Het doel van de AROS port is om compitable te zijn met de ARexx interpreter van
  het klassieke AmigaOS.

+ False kan geclassificeerd worden als een exotische taal, waardoor deze niet
  direct in aanmerking komt voor serieuze ontwikkelings werk, al kan het werken
  met deze taal erg leuk zijn. :-) 

.. _Python: http://www.python.org/
.. _Regina: http://regina-rexx.sourceforge.net/
.. _False:  http://wouter.fov120.com/false/

Waarom zit er geen m68k emulator in AROS?
-----------------------------------------

Om oude Amiga programma's te draaien hebben we UAE_ geport naar AROS. AROS's
versie van UAE zal zelfs nog iets sneller draaien dan andere versies, gezien
AROS minder middelen verbruikt dan andere besturings systemen (wat betekend dat
UAE meer processor tijd krijgt). Wel willen we proberen om de Kickstart ROM
te patchen in UAE, zodat deze AROS functies aanspreekt wat een kleine prestatie
verbetering zal geven. Uiteraard gaan genoemde zaken alleen op voor de native 
versies van AROS en niet de hosted varianten.

Maar waarom implementeren we niet gewoon een m68k CPU virtualiser? Wel, het probleem
is dat de m68k software verwacht dat alle data in het zogeheten 'big endian' formaat komt,
terwijl AROS ook op 'little endian' CPU's draait. Het probleem onstaat dan dat de 
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


Software questions
==================

Hoe krijg ik toegang tot AROS' disk images vanuit UAE?
------------------------------------------------------

De floppy disk image kan als hardfile gemount worden en dan als een 1.4 MB
harddisk gebruikt worden binnen UAE. Nadat u de bestanden naar de hardfile
disk image gekopieert hebt (of iets anders dat u wilde doen), kan deze naar een floppy
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

What are the AROS-native kernel options used in GRUB line?
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