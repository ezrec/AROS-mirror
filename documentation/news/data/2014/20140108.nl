=====================
Verbeteringen in 2013
=====================

:Author:   Matthias Rustler, Staf Verhaegen, Neil Cafferkey
:Date:     2014-01-12

2013 was vooral een jaar van interne verbeteringen en correcties voor AROS,
zodat er minder toeters en bellen toegevoegd werden dan in eerdere jaren.
Desalniettemin, zijn er een aantal belangrijke onderdelen toegevoegd die
het waard zijn genoemd te worden.


Nieuwe C-bibliotheken
---------------------

Na een lange weg werd een opgeschoonde en opgedeelde C-bibliotheek opgenomen in
de ABIv1-tak. Naast het eigenlijk opschonen en naast verbeteringen van de
documentatie, is het grootste gedeelte van de aanpassing het afsplitsen van het
ANSI-standaard-gedeelte van het POSIX-gedeelte. Dit maakt het mogelijk functies
uit standaard-C overal in AROS te gebruiken zonder de noodzaak de hele
POSIX-emulatie toe te voegen


Raspberry Pi
------------

In 2012 berichtten we dat AROS ook draaide op de Raspberry Pi, maar alleen als
gastsysteem bovenop Linux. In 2013 ontstond een hoofdsysteem-versie van AROS
voor dit compacte en alomtegenwoordige platform, met ondersteuning vor USB,
SD-kaart en beeld (althans, met behulp van dezelfde gesloten programmatuur waar
Linux ook op gebaseerd is).


Scalos overgebracht
-------------------

Het Wanderer/Workbench-alternatief `Scalos` is overgebracht naar AROS. Op het
moment is het alleen beschikbaar in de nachtcompilaties van `ABIv1` voor het
i386-platform. Scalos kan ofwel opgestart worden als een toepassing vanaf
*Extras:Scalos* of het kan Wanderer vervangen door in ``S:Startup-Sequence``
``Wanderer:Wanderer`` te vervangen door ``Scalos:Scalos EMU``.

__ http://scalos.noname.fr
__ http://aros.sourceforge.net/nightly1.php


Geluidsstuurders
----------------

Er zijn drie stuurders bijgekomen voor VIA geluidsbestuurders, en wel voor een
aantal van hun Envy24-, Envy24HT- en AC97-chipsets. De overeenstemming van onze
HD-geluidsstuurder is ook vergroot en die ondersteunt nu ook een groter bereik
aan bestuurders en omzetters.


Varia
-----

Er zijn ook massa's kleinere verbeteringen doorgevoerd, waaronder lekstoppen en
herstructureren naar ATA- en SATA-stuurders, bestandssystemen en partities,
EFI-ondersteuning, en systeemsluiting. Gebruikersinterfaces kregen ook
aandacht, met het Zune GUI-systeem dat geleidelijk completer wordt, net als
nieuwe voorkeursprogramma's voor GRUB en voor thema-opties.

