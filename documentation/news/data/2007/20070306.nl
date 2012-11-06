=============
Status Update
=============

:Auteur:   Paolo Besser
:Datum:     2007-03-06

Habemus DOSpackets!
-------------------

Robert Norris heeft de eerste commit gemaakt van het AROS packets
systeem, met ook een (alleen-lezen) FAT filesysteem handler.
Hij voegde dit eraan toe:

"packet.handler is een "proxy" AROS-stijl bestandssysteem dat in eerste instantie 
verantwoordelijk is voor converteren van FSA/IOFileSys naar DosPackets, en het 
doorzenden daarvan naar de packet-gebaseerde handler.

Belangrijk is ook dat packet.handler ontworpen is om de AmigaOS handler omgeving
zo precies mogelijk na te bootsen; dit om het werk te minimaliseren dat nodig is om een
bestandssysteem over te zetten naar AROS.

fat.handler is een FAT12/16/32 handler. Nog alleen-lezen, en met een paar fouten, maar het
bewijst het concept. Toch is er nog veel werk te doen voordat ik de taak gedaan acht. Opmerkingen
zijn zeer welkom."

DOSPackets en FAT alleen-lezen ondersteuning zijn beschikbaar in de eerstvolgende 
nightly builds.

Ander Nieuws
-------------

Pavel Fedin heeft VESA v1.2 ondersteuning toegevoegd, wat meer (oude) video 
kaarten  compatibel maakt met AROS. Hij voegde ook VESA mode overschakeling 
toe aan de bootstrap, wat het minder hidd afhankelijk maakt van de custom GRUB.

Neil Cafferkey heeft 64-bit disk support toegevoegd. Boot partities kunnen nu 
groter zijn dan 4GB.

Hogne Titlestad heeft versie 0.3.2 vrijgegeven van zijn teken 
programma `Lunapaint`__, met nu ook een werkende tekst tool. U kunt nieuwe 
`screenshots hier vinden`__.

Tomasz Pawlak's `KGB Archiver`__ is overgezet naar AROS door FOrest. 
U kunt deze `hier in de Archives`__  vinden.

__ http://www.sub-ether.org/lunapaint/
__ http://www.sub-ether.org/lunapaint/upload/032.jpg
__ http://www.aros.ppa.pl/images/photoalbum/48.jpg
__ http://archives.aros-exec.org
