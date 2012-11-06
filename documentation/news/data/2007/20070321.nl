=============
Status Update
=============

:Auteur:   Paolo Besser
:Datum:     2007-03-21

Eerste USB ondersteuning
------------------------

Dr Michal Schulz heeft de eerste commit gemaakt van de AROS USB stack, 
die al toestaat USB muizen te gebruiken en het eerste begin is van ondersteuning 
voor andere USB apparaten. Let a.u.b. dat dit pre-alpha software is, 
wat betekend dat alleen USB 1.1 UHCI 
controllers ondersteund worden; misschien werkt dit ook geeneens correct 
met uw moederbord. De volgende stap is het ontwikkelen van extensies voor
de USBHID class, die toestaat grafische tabletten en USB toetsenborden te
gebruiken met AROS, samen met ondersteuning voor OHCI controllers. We 
moeten daarbij wel opmerken dat ondersteuning voor USB 2.0 EHCI controllers,
niet was opgenomen in de bounty waar Michal nu aan werkt.
Om de functionaliteit en compatibiliteit te verbeteren is zeer uitgebreide 
beta testing nodig. We vragen iedereen die het aandurft ons te helpen, dit
door de nieuwste nightly build te downloaden en alle gevonden problemen te
meldden. Om de hele USB stack te activeren op UHCI machines, moet u het volgende
commando uitvoeren (via CLI)::

  C:Loadresource DRIVERS:uhci.hidd


Anders nieuws
-------------

Nic Andrews heeft een aantal verbeteringen gemaakt aan AROSTCP, wat ons een
betere (meer compatibele) netwerk stack geeft. De driver voor de VIA Rhine is
gerepareerd, een test versie voor RTL8139 is toegevoegd.

Pavel Fedin werkt ondertussen nog altijd aan de verbeterde VESA modi: "De VESA
mode switch code is opgeschoond. Statisch palet verwijderd, dit was toch 
overbodig. Muis cursor kleuren op 256- en 16-kleuren schermen zijn ook
gerepareerd". Het initialiseren van een aantal oude kaarten met 
buggy BIOS'en is ook gerepareerd. Tevens heeft Pavel het MorphOS Trackdisk device 
'teruggeport'. Een gloednieuw Trackdisk preference programma staat nu een
'no-click' mode toe voor de floppydrive, zodat er geen argumenten meer toegevoegd
hoeven worden aan de kernel command line in GRUB.