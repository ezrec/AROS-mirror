=========
Meehelpen
=========

:Authors:   Adam Chodorowski 
:Copyright: Copyright © 1995-2002, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Done. 

.. Contents::

.. Include:: contribute-abstract.nl


Beschikbare taken
=================

Dit is een lijst met enkele taken die nog gedaan moeten worden, maar
waar niemand op moment aan werkt. Het is in geen geval een compleet overzicht 
en bevat alleen de meest prominente zaken waar we in AROS hulp bij nodig hebben.


Programmeren
------------

+ Implementatie van ontbrekende libraries, resources, devices of delen hiervan.
  Zie het gedetailleerde status rapport voor meer informatie over welke onderdelen
  nog ontbreken.

+ Implementeren of verbeteren van hardware device drivers:
  
  - AROS/m68k-pp:
    
    + Grafisch
    + Input (touchscreen, buttons)
    + Geluid
 
  - AROS/i386-pc:
    
    + Specifieke drivers voor grafische kaarten (we hebben alleen algemene, 
      niet erg geaccelereerde versies). Een korte wenslijst:
      
      - nVidia TNT/TNT2/GeForce (gestart, maar incompleet) 
      - S3 Virge
      - Matrox Millenium
    
    
    + USB
    + SCSI
    + Specifieke IDE chipsets
    + Geluid
    + ...alle andere zaken die u kan bedenken.


  - Algemene printer ondersteuning.
 
  - Algemene geluid ondersteuning.


+ Porten naar andere architecturen. Een aantal voorbeelden van hardware waarvoor 
  nog geen onderhouden AROS port bestaat of is gestart:

  - Amiga, zowel m68k als PPC.
  - Atari.
  - HP 300 serie.
  - SUN Sparc.
  - iPaq.
  - Macintosh, zowel m68k als PPC.

+ Implementeren van ontbrekende Preferences editors:

  - IControl
  - Overscan
  - Palette
  - Pointer
  - Printer
  - ScreenMode
  - Geluid
  - WBPattern
  - Workbench 
 
+ Verbeteren van de C link library

  Dit betekend het implementeren van ontbrekende ANSI (en sommige POSIX) functies
  in de clib, wat het makkelijker zal maken om UNIX software te porten 
  (zoals GCC, make en binutils). De grootste ontbrekende functie is ondersteuning
  voor POSIX stijl signalering, naast enkele andere functies.

+ Implementatie van meer datatypes en het verbetering van de bestaande versies

  Het aantal datatypes beschikbaar in AROS is vrij klein. Voorbeelden van
  datatypes die nog verbetering behoeven om bruikbaar te worden of van begin
  af aan opgebouwd moeten worden zijn:

  - amigaguide.datatype
  - sound.datatype
    
    + 8svx.datatype

  - animation.datatype
    
    + anim.datatype
    + cdxl.datatype
    
  
+ Porten van programma's van derden:

  - Text editors zoals ViM en Emacs.
  - Ontwikkeling tools waaronder: GCC, make, binutils en andere GNU 
    ontwikkeling tools.
  

Documentatie
------------

+ Schrijven van gebruikers documentatie. Dit omvat het schrijven van een 
  algemene (Engelstalige) Gebruikers Handleiding voor nieuwkomers en experts, 
  met daarbij referentie documentatie voor alle standaard AROS programma's.

+ Schrijven van documentatie voor ontwikkelaars. Ondanks dat deze documentatie
  in een iets betere staat verkeerd dan de gebruikers versie, moet er nog
  altijd veel werk aan gebeuren. Er is bijvoorbeeld nog geen echt goede 
  handleiding voor beginnende programmeurs. Een AROS tegenhanger van de ROM 
  Kernel Handleiding zou ook een mooi iets zijn om te hebben.


Vertaling
-----------

+ Het vertalen van AROS zelf in meerdere talen. Op moment worden alleen de volgende
  talen min of meer compleet ondersteund: 

  - English
  - Deutsch
  - Svenska
  - Norsk
  - Italiano

+ Vertaling van de documentatie en website in meerdere talen. Op moment is deze 
  alleen compleet beschikbaar in Engels. Delen zijn vertaald naar Noors, maar er is 
  nog altijd veel werk te doen.

Andere zaken
------------

+ Coördinatie van het GUI ontwerp voor AROS programma's, zoals het 
  prefs programma, tools en utilities.

Deelnemen aan het team
======================

Wilt u het meedoen met de ontwikkeling? Graag! Geef u dan op voor de 
`ontwikkelaars mailing lijst`__ van uw interesse (het abonneren op de hoofd 
ontwikkeling lijst is *zeer* aan te raden) en vraag tevens om toegang tot het 
Subversion archief. Dat is alles. :) (Noot: enige beheersing van de Engelse óf
Duitse taal is wel handig) 

We raden u aan om daarna een mail naar de ontwikkelaars lijst te schrijven 
met een beknopte introductie van uzelf en waaraan u wilt meehelpen. Mocht u nog 
problemen tegenkomen, twijfel dan niet om een mail te sturen naar de lijst of 
om rond te vragen op de `IRC kanalen`__. Wilt u aan iets specifieks gaan werken
vergeet dan a.u.b. niet eerst een mail naar de lijst te schrijven met de 
melding wat u wilt gaan doen en het verzoek om de takenlijst te updaten. Op die
manier zijn we er zeker van dat per ongeluk geen twee mensen tegelijk aan iets 
werken.

__ ../../contact#mailing-lists
__ ../../contact#irc-channels


Het Subversion archief
----------------------

Het AROS broncode archief draait op een paswoord beveiligde Subversion 
server, wat betekend dat u eerst om toegang zal moeten vragen voordat u kunt
deelnemen aan de ontwikkeling. De paswoorden komen in een geëncrypteerde vorm, 
die u online kunt generen met onze `online paswoord encryptie tool`__.

Mailt u daarna dit geëncrypteerde paswoord samen met uw geprefereerde 
gebruikersnaam en uw echte naam naar `Aaron Digulla`__ en wacht op antwoord. 
Voor een zo snel mogelijke afhandeling verzoeken wij u om a.u.b. de titel 
in te stellen op "Access to the AROS SVN server" en het bericht zelf als 
"Please add <gebruikersnaam> <passwoord>". Ter voorbeeld::

    Please add digulla xx1LtbDbOY4/E

Aaron heeft het vaak druk, wees dus a.u.b. geduldig. 
Het kan zo best enkele dagen duren eer u toegang krijgt.

Voor informatie over het gebruik van de AROS SVN server verwijzen wij u door 
naar de (nog Engelstalige) pagina "`Werken met SVN`__". Zelfs als u al het één en
ander af weet van SVN is het nog handig om deze pagina door te lezen. 
Deze bevat namelijk specifieke informatie en tips gericht op het AROS archief 
(zoals hoe in te loggen).  

__ http://aros.sourceforge.net/tools/password.html 
__ mailto:digulla@aros.org?subject=[Access%20to%20the%20AROS%20SVN%20server]
__ svn