=====
Ports
=====

:Authors:   Adam Chodorowski, Matthias Rustler 
:Copyright: Copyright © 1995-2006, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Done.

.. Contents::


Introductie
===========

Omdat AROS een overdraagbaar besturing systeem is, is het systeem beschikbaar 
voor verschillende platformen. Zo'n overgezette AROS versie voor een specifiek
platform heet een "port". Voor het gemak houden we enkele "originele" Engelse termen 
aan in deze Nederlandse vertaling.


Soorten
-------

Ports worden opgedeeld in twee hoofdgroepen, of "soorten" in AROS terminologie,
namelijk "native" en "hosted".

Native ports kunnen direct op de hardware gedraaid worden en hebben totale
controle over de computer. Dit zal in de toekomst de aangeraden methode worden 
om AROS te draaien, sinds dit superieure prestaties en efficiency toestaat. Helaas
zijn de native ports op moment nog zeer incompleet wat ze niet echt nuttig maakt 
(tenminste, voor ontwikkeling).

Hosted ports draaien onder een ander besturingssysteem. Ze spreken de hardware 
niet direct aan, maar gebruiken in plaats daarvan 
de faciliteiten die het host OS levert. De voordelen van een gehoste port is dat 
deze makkelijker te schrijven is nu het niet nodig is om alle zogeheten low-level 
drivers te moeten schrijven (bijv. grafische drivers). Voordeel is ook dat 
de ontwikkeling in een hosted omgeving veel sneller gaat. Nadeel is namelijk dat AROS
nog niet zelf-hosted kan draaien (ofwel: we kunnen AROS niet 
compileren onder AROS). In een hosted omgeving kan dit wel, wat het toestaat 
om de ontwikkel omgeving en AROS zij-aan-zij te draaien zonder tijd te 
verspillen aan het constante rebooten om nieuwe code uit te proberen.


Benaming
--------

De verschillende AROS ports worden vernoemd volgens de methode <cpu>-<platform>,
waar <cpu> de CPU architectuur is en <platform> de symbolische naam is van het
platform. Een platform of port kan ofwel een hardware versie zijn voor native
versies, zoals "pc" of "amiga", of een besturingssysteem voor gehoste ports, zoals
"linux" of "freebsd". In gevallen waar het niet vanzelfsprekend is dat het onderwerp
AROS is wordt de prefix "AROS/" toegevoegd aan de port naam, wat 
bijvoorbeeld "AROS/i386-pc" geeft.


Overdraagbaarheid
-----------------

AROS executables voor een specifieke CPU zijn overzetbaar naar alle ports die dezelfde
CPU gebruiken. Dat betekend dat een executable die gecompileerd is voor "i386-pc"
ook prima werkt op "i386-linux" en "i386-freebsd".


Bestaande ports
===============

Hierna volgt een lijst van alle AROS ports die werken en/of actief ontwikkeld
worden. Deze zijn niet allemaal beschikbaar als download, gezien sommigen niet
compleet genoeg zijn of compilatie eisen hebben waar we niet over beschikken
door gelimiteerde middelen.


AROS/i386-pc
------------

:Soort:    Native
:Status:     Werkend, incomplete driver ondersteuning
:Onderhouden: Ja

AROS/i386-pc is de native port van AROS voor algemene IBM PC AT computers en 
compatibele die de x86 processor familie gebruiken. De naam is een beetje 
misleidend sinds AROS/i386-PC minimaal een 486 klasse CPU nodig heeft - sommige 
gebruikte instructies zijn namelijk niet beschikbaar zijn op de 386.

Deze port functioneert vrij goed, al is er alleen basic drivers ondersteuning. 
Grootste limitatie is dat we alleen versnelde grafische ondersteuning bieden 
voor de grafische kaarten van nVidia en ATI. Andere grafische adapters moeten 
het doen met de algemene (niet-versnelde) VGA en VBE drivers. Er zijn meer 
drivers in de maak, alleen gaat de ontwikkeling daarvan zeer traag gezien we 
maar ongeveer 2.5 hardware hackers hebben. Deze port kan gedownload worden.


AROS/m68k-pp
------------

:Soort:    Native 
:Status:     Gedeeltelijk werkend (in een emulator), incomplete driver ondersteuning
:Onderhouden: Ja

AROS/m68k-pp is de native port van AROS voor de Palm serie van handheld 
computers en compatibele ("pp" staat voor "palm pilot", wat de naam was van de 
eerste handhelds uit deze serie). Dit betekend misschien dat je in de toekomst 
AROS  nog eens in je broekzak kan meenemen.

Deze port is op moment nog erg ruw. Het werkt meestal (in een emulator, sinds 
niemand zijn dure hardware wil riskeren), dan nog moet er veel werk aan 
gebeuren. Er is een grafische driver, maar geen voor de input. Deze port is op 
moment niet beschikbaar als download.

AROS/i386-linux
---------------

:Soort:    Hosted
:Status:     Werkend
:Onderhouden: Ja

AROS/i386-linux is de gehoste versie van AROS voor Linux gebaseerde 
besturingssystemen [#]_ draaiend op de x86 processor familie.

Dit is de meest complete port van AROS qua features, omdat de meeste van 
onze ontwikkelaars momenteel Linux gebruiken voor de ontwikkeling van AROS.
Ook hoeven er maar weinig drivers geschreven te worden. 
Deze port is beschikbaar als download.



AROS/i386-freebsd
-----------------

:Soort:    Hosted
:Status:     Werkend
:Onderhouden: Ja (5.x)

AROS/i386-freebsd is de gehoste port van AROS naar het FreeBSD besturingssysteem 
draaiend op de x86 processor familie.

Deze port is relatief compleet omdat deze een groot deel van de code deelt met 
AROS/i386-linux. Alleen wil het dat weinig van onze ontwikkelaars FreeBSD 
gebruiken, waardoor deze port enigszins achterloopt qua nieuwigheden. We proberen 
AROS/i386-freebsd ook te compileren als we snapshots maken, dit is echter niet 
altijd mogelijk waardoor deze soms niet beschikbaar is als download.


AROS/ppc-linux
--------------

:Soort:    Hosted
:Status:     Werkend
:Onderhouden: Ja

AROS/ppc-linux is een gehoste port van AROS voor Linux gebaseerde 
besturingssystemen draaiend op de PPC processor familie.

Een reeds gecompileerde versie is te downloaden van `Sourceforge`__. 
Deze compileren van de broncode vraagt om een gepatchte versie van gcc3.4.3. 
Het diff bestand daarvoor kan gevonden worden in contrib/gnu/gcc.

__ http://sourceforge.net/project/showfiles.php?group_id=43586&package_id=194077


Voetnoten
=========

.. [#] Jawel, we weten dat Linux eigenlijk alleen een kernel is en geen compleet
       besturingssysteem, alleen is dit zo veel korter om te schrijven dan 
       "besturingssystemen die gebaseerd zijn op de Linux kernel, enkele van de 
       bekende GNU tools en het X schermsysteem.". Dit is optimalisatie van de 
       omvang die hier inmiddels alweer tenietgedaan is door deze uitleg voor 
       de kritische lezers, maar vooruit...