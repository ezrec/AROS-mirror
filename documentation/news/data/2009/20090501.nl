=============
Statusrapport
=============

:Author:   Simone Bernacchia & Neil Cafferkey
:Date:     2009-05-01

Recentste aanpassingen
----------------------

Sinds het verslag van voor Kerstmis, heeft zijn er veel verbeteringen geweest
in AROS, zowel in systeemonderdelen als in toepassingen: AROS is bijna
bruikbaar voor dagelijks gebruik.

Oliver Brunner heeft onlangs versie 0.3 van Janus-UAE uitgebracht, de
AROS-specifieke versie van UAE die vereist wordt door fase 1 van premie voor
UAE-integratie. AROS kan nu AmigaOS-toepassingen op hun eigen AROS-scherm of
-venster openen. Oliver werkt nu aan het integreren van menu's.

Maar het belangrijkste nieuws is dat AROS eindelijk zijn eigen moderne
webbrowser heeft! Dankzij het harde werk van Stanislaw Sszymczyk, is OWB
eindelijk beschikbaar voor AROS, met volledige CSS- and
Javascript-overeenstemming, met tabbladen en een afhaalbeheerder. OWB is nu
versie 0.9.3 Beta, en heeft om correct te werken recente versies van
muimaster.library, arosc.library and codesets.library nodig. OWB kan
afgehaald worden worden van zijn eigen `pagina op sand-labs`__. Stanislaw heeft
het ook mogelijk gemaakt om meerdere versies van AROS te compileren vanaf
dezelfde kopie van de bronnenboom.

Pavel "Sonic" Fedin werkt nog steeds aan de gast-versie Windows van AROS en
probeert ook om, in de loop van dit karwei, verschillende problemen op te
lossen met de manier waarop AROS communiceert met de apparatuur, om het te
kunnen abstraheren; dit heeft er toe geleid dat hij onderdelen heeft aangepast
die een abstractielaag nodig hebben om in gastversies van AROS te kunnen
werken, zoals bestandssystemen en apparatuur; hieronder valt een
pakketverwerkingsmethode voor SFS. Verder probeert Pavel, samen met Jens
Langner, om de recentste versies te kruiscompileren van de mcc.BetterString en
mcc.TextEditor MUI-klassen opdat die direct van de bron genomen worden, in
plaats van uit de AROS bronnenboom.

Matthias "Mazze" Rustler heeft een `alfa-versie`__ uitgebracht van de Ignition
rekenblad-toepassing. Op het moment kunnen gegevens nog niet worden opgeslagen
maar dit is toch een goede gelegenheid om deze toepassing eens uit te proberen,
die op AmigaOS ondergewaardeerd wordt maar tamelijk krachtig en goed-uitgevoerd
is. Er zijn wel nog een paar haken en ogen: de toepassing is maar gedeeltelijk
vertaald van het Duits naar het Engels; om het programma in het Engels te
gebruiken moet het bestand ignition_english.prefs hernoemd worden naar
ignition.prefs.

Nik "Kalamatee" Andrews is bezig Wanderer te verbeteren : de laatste maanden
heeft zijn werk de stabiliteit verbeterd, hij heeft een statusbalk aan vensters
toegevoegd, en hij heeft geëxperimenteerd met een laden-boom aan de linkerkant
van vensters. Hij is van plan in de toekomst, naast de introductie van de 
langverwachte lijstweergave, om Wanderer modulairder te maken, zodat
verschillende onderdelen naar keuze kunnen worden toegevoegd of weggelaten, en
ook door modules van derden kunnen worden uitgevoerd.

De bekende VmwAROS distributie is van naam veranderd naar `Icaros
Desktop`__; versie 1.1 (`recent bijgewerkt to 1.1.1`__) brengt aangepaste
systeembestanden en betere stabiliteit. Bijna alle toepassingen werden
hercompileerd om de recentste muimaster.library compilatie -
waardoor men eindelijk de oude switchmuimaster toepassing kwijt is. Zoals
gewoonlijk, is Icaros zowel beschikbaar als een actieve DVD en als een virtuele
omgeving (VE). De VE-version is op Windows XP geïnstalleerd op een netboek,
draaiend op een redelijke snelheid met volledige audio- en
draadloos-netwerkondersteuning. Meer informatie is `hier`__.

Michal Schulz werkt verder aan zijn Efika-versie en, daarnaast, aan de premie
voor USB-massaopslag. In december was AROS in staat om USB-sticks aan te
koppelen en van een extern CDROM-station op te starten. Voortgang van de
Efika-versie bracht een robuustere kern, een gedetailleerd lekstopverslag en
een functie die veel lijkt op de Grim Reaper van AmigaOS 4, die voorkomt dat
een machine vastloopt in het geval van een uitval.

Neil Cafferkey heeft gewerkt aan de overeenkomst van de ATA-stuurder, waardoor
die nu werkt voor een grotere variatie aan PC-s en schijven. Deze
veranderingen waren niet op tijd klaar om meegenomen te worden in Icaros 1.1.1,
maar ze zullen waarschijnlijk in de volgende uitgave.

Ook de manier waarop AROS opstart onderging veranderingen: het is nu mogelijk
32-bit- en 64-bit-versies van AROS naast elkaar op dezelfde computer te
installeren. Het gevolg daarvan is dat alle opstartbare AROS-partities een 
bestand AROS.boot moeten hebben dat het architectuur-type bevat van de AROS
systeembestanden op die partitie. Om de kern van een systeem dat voor
20 maart geïnstalleerd is bij te werken, moet zo'n bestand handmatig naar
de root-lade van de systeem-partitie, om moet het daar aangemaakt worden (b.v.:
voer het commando "Echo pc-i386 TO DH0:AROS.boot" uit voor een 32-bit systeem).

En tenslotte de komst van Steve "ClusterUK" Jones: in het verleden een 
Siamese System-ontwikkelaar, die nu terugkomt naar Amiga-land en een
AROS-systeem te koop aanbiedt. De `iMica`__ is een kleine bureaucomputer,
gebaseerd op een Intel Atom, with Icaros Desktop geïnstalleerd. De iMica biedt
momenteel een van de beste AROS-ervaringen op echte apparatuur, door het
opnemen van een Creative Labs SoundBlaster-kaart met EMU10k-chipset die
ondersteund wordt door AROS, en een RTL8168 netwerkkaart, ondersteund door
Kalamatee's nieuwe stuurder.

__ http://www.sand-labs.org/aros
__ http://www.mazze-online.de/files/ignition.i386-aros.zip
__ http://www.icarosdesktop.org
__ http://vmwaros.blogspot.com/2009/04/icaros-desktop-111-update-ready-for.html
__ http://vmwaros.blogspot.com/2009/04/running-icaros-desktop-on-netbook.html
__ http://www.clusteruk.com

