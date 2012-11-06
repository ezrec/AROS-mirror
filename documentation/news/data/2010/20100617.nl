===============
Stand van zaken
===============

:Author:   Paolo Besser
:Date:     2010-06-17

Het laatste nieuws
------------------

Er zijn veel dingen gebeurd sinds het nieuws voor het laatst is bijgewerkt.

Zo ontwikkelde Michal Schulz de GMA950 hidd, waardoor AROS nu direct gebruik
kan maken van 2D-functionaliteit van de nieuwste GPU's van Intel. Dit zal
goed van pas komen voor mensen die een iMica systeem van Stephen Jones
of een netbook, zoals de Acer Aspire One A150, gebruiken.

Krzysztof "Deadwood" Smiechowicz is behoorlijk opgeschoten met zijn conversie
van Gallium3D naar AROS: Hij heeft nu 2D- en 3D-versnellende functies
geïntegreerd in één enkel stuurprogramma genaamd Nouveau, die 2D-versnelling
ondersteund voor bijna alle GeForce GPU's beginnende bij de antieke GeForce 2
kaarten tot en met de recente GTX 200 series. 3D-functionaliteit is echter
alleen beschikbaar voor GeForce FX (5x00) en nieuwere kaarten. Sommige
modellen en GPU's worden misschien nog niet ondersteund. Gebruikers van
Nvidia-kaarten kunnen dit stuurprogramma nu regelmatig gebruiken in plaats
van het oude Nvidia stuurprogramma. Dit zal Krzystof ook helpen met het 
verbeteren, omdat het nog een werk in uitvoering is. Raadpleeg dit 
`AROS-EXEC onderwerp`__ om te helpen met het vinden en oplossen van fouten 
(en onthoudt daarbij a.u.b. dat hij niet de direct verantwoordelijke is voor
de kwaliteit van het stuurprogramma: hij past alleen aan en converteert
datgene naar AROS wat al is gedaan door het Nouveau/Gallium team).

Pavel Fedin is ons grafisch subsysteem aan het reorganiseren, door het op te
schonen en er voor te zorgen dat het zich gedraagt als die van het originele
Amiga-subsysteem: De dag dat we geen namen van stuurprogramma's in het GRUB
opstart venster dienen in te geven is nabij. Sommige stuurprogramma's kunnen
nu worden geactiveerd door ze in de monitors directory te plaatsen (zoals op 
het Amiga OS), terwijl dit voor anderen nog niet mogelijk is. Maar hij is
hier nog steeds mee bezig. Pavel is ook bezig het stuurprogramma voor de
linux framebuffer te verbeteren alsmede zijn Win32-hosted versie van AROS,
welke nu schermen kan verplaatsen net zoals op klassieke Amiga's.

Neil Cafferkey heeft onze ondersteuning van FAT-partities behoorlijk
verbeterd, en heeft er ook voor gezorgd dat AROS te installeren is op USB
pendrives (en er vanaf kan worden opgestart). Dit betekent dat de reguliere
nachtelijke compilaties, maar ook `Icaros Desktop versie 1.2.2`__ nu kunnen
worden geïnstalleerd op netbooks en andere USB-opstartbare computers, door
gebruik te maken van een geheugenstick in plaats van een DVD. Paolo Besser
heeft een volledige `how-to`__ geschreven in PDF-formaat en heeft deze op
de Icaros website geplaatst.

Pascal Papara heeft ons een spiksplinternieuwe distributie gegeven met de 
naam `Broadway`__, die is gericht op mensen die zich nog nooit hebben 
beziggehouden met Amiga computers en bevat een uitgeklede versie van de 
aankomende AMC, de eerste media-center omgeving gebaseerd op AmigaOS-lijkende
systemen. Broadway is nog niet compleet, maar een aardige voorbeeld-versie
kan worden gedownload van zijn website__.

Steve "ClusterUK" Jones heeft de komst bekendgemaakt van een nieuwe stille
versie van zijn computer-lijn iMica. Dit op Atom gebaseerd systeem zal
dezelfde performance hebben als de oude, maar zal daarentegen compleet
zonder fan draaien. We moeten Steve tevens bedanken voor zijn 
financieringsinspanningen: na het betalen van Davy Wentzler voor zijn
nuttige AHI-HD-Audio-stuurprogramma's, heeft hij ook de conversie
gefinancierd van de Catweasel MKIV-stuurprogramma's en de ontwikkeling 
van Michal's GMA-stuurprogramma.

Ook goed nieuws voor gebruikers van ACube's SAM440EP: Sinds 14 april zijn de
nachtelijke compilaties van AROS ook beschikbaar voor dit alleraardigst PPC
platform, en die nog steeds actief worden onderhouden. Michal Schulz heeft
tevens recentelijk voor een EFIKA-versie van AROS gezorgd.

Nick "Kalamatee" Andrews heeft Wanderer enigzins verbeterd en enkele lang
openstaande fouten verholpen. Wanneer per ongeluk een lade op zichzelf
geplaatst wordt, zal AROS deze niet meer oneindig blijven proberen te
kopiëren, en de desktop van AROS kan nu worden bevolkt door gebruik te maken
van de "leave out"/"put away"-opties, die nu eindelijk zijn geïmplementeerd.
Hij werkt tevens aan een ikoon- en lijstweergave zodat bestanden nu kunnen
worden weergegeven op basis van details en kunnen worden gesorteerd naar
wens.

Er zijn ook vele kleine en grote verbeteringen "onder de motorkap" gaande die
van andere software ontwikkelaars komen, maar het zijn er simpelweg teveel om
ze hier allemaal te noemen. We bieden ze dan ook onze verontschuldigingen 
aan, en zouden tegelijkertijd een erg groot "Bedankt!" willen zeggen tegen
iedereen die ons op welke wijze dan ook helpt zoals diegenen die software
converteren, nieuwe applicaties maken, documentatie schrijven en/of enkel
AROS onder de aandacht brengen zowel binnen als buiten de Amiga-gemeenschap.

__ http://aros-exec.org/modules/newbb/viewtopic.php?viewmode=flat&topic_id=4724&forum=2
__ http://vmwaros.blogspot.com/2010/05/icaros-desktop-122-update-ready-for.html
__ http://vmwaros.blogspot.com/2010/06/create-your-own-icaros-stick.html
__ http://www.aros-broadway.de/
__ http://www.aros-broadway.de

