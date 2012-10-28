===============
Stand van zaken
===============

:Author:   Paolo Besser
:Date:     2010-01-11

Het laatste nieuws
------------------

Het is een lange tijd geleden sinds het laatste nieuws is bijgewerkt, echter
zijn er in de tussentijd veel dingen gebeurd.

Allereerst heeft Chris "platon42" Hodges de broncode van zijn Poseidon
USB-stack vrijgegeven onder de APL-voorwaarden, en heeft het geconverteerd
naar AROS: na enige weken van tweaken, aanpassen en verwijderen van
fouten, kan AROS nu omgaan met USB 1.1 en 2.0 controllers. Elk USB-apparaat
zou nu correct moeten worden gedetecteerd, terwijl muizen, toetsenborden en
overige invoerapparaten tevens correct zouden moeten worden afgehandeld. De
meeste USB-pendrives en opslagapparatuur werken nu, terwijl anderen dit niet
doen vanwege enkele problemen, met de fat.handler, die momenteel worden
onderzocht. Overige apparaten zullen in de toekomst worden
ondersteund, wanneer iemand hiervoor de noodzakelijke stuurprogramma's
schrijft.

Dankzij Poseidon kan AROS nu opstarten vanaf een USB CD-ROM of
DVD-ROM speler. Dit maakt het mogelijk om AROS te installeren op een grote
diversiteit aan notebooks en netbooks zoals de `Acer Aspire One A150`__.
Deze netbook is nu een vrij goede hardware kandidaat voor AROS: Steve Jones
en Davy Wenzler schreven een `HD Audio stuurprogramma`__ voor de iMica__
computer, die dit apparaat ondersteund maar ook een groot aantal andere
Intel-compatible moederborden, terwijl de `Icaros Desktop`__ distributie een
oplossing heeft gevonden om de juiste 1024x600 video-resolutie van de GMA9x0
video-chip weer te geven.

Przemyslaw "Qus" Szczygielski's TCPPrefs utility is gerepareerd en toegevoegd
aan de AROS instellingen-programma's: het is niet meer nodig om met scripts
en tekstbestand configuraties in de weer te gaan voor verbinding met het
internet. Matthias Rustler heeft Scout geconverteerd naar AROS, het is nu
dus nog makkelijker om vastlopende vensters en gecrashte applicaties te
sluiten. De stabiliteit van de Kernel is enorm verbeterd dankzij de vele
foutverwijderingen van Neil Cafferkey en Pavel Fedin.

Neil Cafferkey heeft ons ata.device enorm verbeterd, deze ondersteunt nu
vele SATA-controllers. Elke computer met een SATA-controller die in de BIOS
kan worden ingesteld als 'IDE' zou echter moeten werken met AROS.

Stanislaw Szymczyk is verder gegaan met het bijwerken van OWB, het oplossen
van fouten en problemen en het toevoegen van nieuwe functionaliteit zoals
bookmarks, een applicatie-menu, een opstart-scherm, de mogelijkheid om
HTML-bestanden te openen etc., en hij is nu richting een release versie 1.0
aan het toewerken van deze moderne, CSS en javascript ondersteunende
internetbrowser. OWB is een heel flexibel stuk gereedschap, omdat het AROS
in staat stelt om de meeste web-ondersteunde applicaties te draaien.

Yannick "Yannickescu" Erb heeft een aantal interessante games naar AROS
geconverteerd zoals Open Tyrian, Super Methane Brothers en SDL Ball en 
schreef ZuneARC, een gestandaardiseerde en configureerbare Zune bediening
voor archiveerprogramma's op de commando-regel die al beschikbaar zijn voor
Aros. Deze en ander interessant spul kunt u vinden op `zijn website`__. Een
aantal leuke spellen zijn geconverteerd door AROS_EXEC.org-gebruiker
Fishy_fis. Vergeet zijn conversie van DOSBox naar AROS niet!

Simone "samo" Bevilacqua heeft zijn Amiga spel BOH naar AROS
geconverteerd, en het gedraagt zich voorbeeldig! `BOH`__ is het eerste
commerciële spel dat beschikbaar is voor de nieuwe Amiga-platformen.

Krzysztof "Deadwood" Smiechowicz heeft versie 7.5 van MESA naar AROS
geconverteerd en is 3D-versnelling voor AROS aan het toevoegen, met zijn
voortdurende conversie van Gallium3D. Momenteel, heeft hij net een
`alpha demo`__ van deze techniek vrijgegeven die draait op video-kaarten van
de GeForce FX, 6 en 7 series. Een leuke video die de GLExcess demo
toont, draaiend onder AROS, staat hier__.

Paolo Besser heeft versie 1.2 vrijgegeven van de Icaros Desktop distributie.
Deze nieuwe versie bevat de mogelijkheid om `oude Amiga spellen rechtstreeks
vanaf hun ADF bestand te draaien`__, dankzij een aantal nieuwe scripts voor
AmiBridge en een oude versie van E-UAE. In de tussentijd, heeft Oliver
Brunner de integratie van Janus-UAE verbeterd, waardoor AROS in staat wordt
gesteld om AmigaOS 3.x applicaties te draaien binnen AROS vensters. Om beide
zaken te doen moeten gebruikers echter een werkende installatie van
AmigaOS3.x en een originele kickstart aanleveren, daarbij eventueel
gebruikmakend van Cloanto's `Amiga Forever`__.

Achter de schermen is Staf Verhaegen bezig met een nieuwe gestandaardiseerde
ABI voor AROS. Hopelijk zal de stand van de sterren nog dit jaar op een dag
goed genoeg staan om de ABI zijn opwachting te laten maken.


__ http://vmwaros.blogspot.com/2009/12/icaros-desktop-got-sound-on-imica-and.html
__ http://www.clusteruk.com/SitePortalPage.aspx?siteid=1&cfid=0&did=109
__ http://www.clusteruk.com/SitePortalPage.aspx?siteid=1&cfid=0&did=108
__ http://www.icarosdesktop.org/
__ http://www.dusabledanslherbe.eu/AROSPage/INDEX.1.html
__ http://www.bohthegame.com/
__ http://download.aros3d.org/gallium/mesa-nv.i386-aros.zip
__ http://vmwaros.blogspot.com/2009/11/yet-another-glexcessgallium3d-video.html
__ http://vmwaros.blogspot.com/2009/11/can-icaros-play-my-amiga-games-and-why.html
__ http://www.amigaforever.com
