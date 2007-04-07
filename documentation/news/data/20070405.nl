=============
Status Update
=============

:Auteur:   Paolo Besser
:Datum:     2007-04-05

Skin-bare Wanderer
------------------

Een belangrijke stap is gemaakt voor het instellen van AROS:
Darius Brewka en Georg Steger hebben een nieuw decor systeem toegevoegd
aan Wanderer. Dit staat de gebruiker toes om thema's te veranderen en 
aan te passen. Damir Sijakovic's ICE thema wordt hiervoor voorlopig als voorbeeld
gebruikt en is nu het standaard thema voor de AROS nightly builds. Ter voorbeeld
deze screenshot (`hier de volledige versie`__):

.. image:: /images/20070405.jpeg
   :alt: AROS 'gedecoreerde' desktop
   :align: center

Mooi niet? Ieder geval, we proberen AROS stijlvoller en 
visueel aantrekkelijker te maken. Daardoor is ook elk soort artistieke 
contributie (thema's, iconen, bureaublad achtergronden, gadgets enz.) zeer welkom. 
Neem a.u.b. een kijkje op `AROS-Exec`__ voor meer informatie.


Fantastisch nieuws
------------------

Michal Schulz vordert met de toetsenbord driver voor zijn USB stack. 
Hij heeft een eerste commit gemaakt die al vrij goed werkt 
(toetsen worden correct afgehandeld, alleen de LEDs werken nog niet). 
Hou a.u.b. wel rekening dat AROS alleen nog compatibel is met de UHCI USB 
controllers. 

Het SFS bestandssysteem compileert nu automatisch. Dit staat de gebruiker toe om 
SFS partities te mounten en gebruiken op een veiligere en makkelijkere manier. 
Pavel Fedin heeft het format commando geupdate om met verschillende bestandssystemen
om te gaan. Om nu een SFS partitie te formatteren, moet u deze eerst creëren met HDToolBox, 
het type veranderen naar SFS en daarna het volgende shell commando uitvoeren:

  FORMAT DRIVE=DH1: NAME=MijnVolume

Let wel: GRUB kan AROS nog niet booten vanaf SFS. U zult dus een kleine
FSS (of EXT3, FAT) partitie moeten creëren om alle kernel en opstart bestanden op te 
zetten, zodat GRUB zijn werk kan doen.

Pavel werkt op moment aan een nieuw CD bestandssysteem genaamd
CDVDFS, dat momenteel CD en DVD's ondersteund met joliet extensies.
Zo gauw enkele fouten zijn verholpen wordt deze opgenomen in de nightly builds.

Matthias Rustler heeft AROS LUA geupdate naar versie 5.1.2. `Hier`__ 
is een (Engelse) lijst van verholpen fouten te vinden. Matthias heeft ook een nieuw 
DepthMenu commodity gemaakt. Als een gebruiker rechts-klikt op het 'diepte(venster) 
gadget', opent dit een popup menu met all open vensters, wat toestaat sneller te
selecteren.

Staf Verhaegen heeft de ondersteuning voor ARexx/Regina scripts verbeterd. 
Er is een nieuw rx commando in AROS, dat wacht om al uw ARexx scripts uit te voeren.

Dankzij Bernd Roesch, kan ZUNE nu ook overweg met MUI4 afbeeldingen (.mim bestanden). 
Pavel Fedin heeft een crash verholpen in de 'pendisplay class' die optreed als er geen
vrije 'pens' op het scherm zijn. Tot slot zijn enkele Gorilla Iconen toegevoegd door Paolo Besser. 


__ http://aros-exec.org/modules/xcgal/displayimage.php?pid=239
__ http://aros-exec.org/modules/news/article.php?storyid=227
__ http://www.lua.org/bugs.html#5.1.1
