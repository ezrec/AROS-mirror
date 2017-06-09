=====================
Werken met Subversion
=====================

:Authors:   Aaron Digulla, Adam Chodorowski 
:Copyright: Copyright (C) 1995-2002, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Done.
:Abstract: 
    
    Subversion (afgekort SVN) is versiebeheer software dat een bestanden database 
    beheerd voor een project. Met SVN is het 
    mogelijk om veranderingen aan archiefbestanden te inspecteren en te beheren: 
    te zien welke veranderingen er werden aangebracht en wanneer, door wie en wat het doel 
    daarvan was (zolang er een logboek vermelding) is. 
    Het geeft ook de mogelijkheid verkeerde wijzigingen ongedaan te maken, bewerkingen
    aan hetzelfde bestand door verschillende mensen samen te voegen en veel meer. 
  
    In wezen wordt het zo *veel* makkelijker voor een groep mensen om gemeenschappelijk 
    aan een project te werken. Immers, iedereen wordt nu in staat gesteld om te weten 
    wat er in het archief gebeurt, ook garanderend dat deelnemers niet per ongeluk elkaars 
    veranderingen ongedaan maken. Belangrijker misschien nog: het systeem staat toe dat 
    mensen samenwerken over internet. Uiteraard gebruiken ook wij dit systeem om samen aan 
    AROS te werken.
    

.. Contents::



Introductie
===========

De server beheert een centraal "archief", wat de hoofd database is die alle
algemene code bevat van het project. Individuele ontwikkelaars hebben hun eigen
"werkende kopieën". Dit zijn lokale kopieën van de database, gemaakt op een 
specifieke tijd, welke ook de aanpassingen bevatten van de ontwikkelaar die nog
niet geupload zijn naar de server. Wanneer een ontwikkelaar zijn veranderingen
wil delen met de rest van het team, "commit" hij deze naar de server via een client 
programma. Deze verzorgd het uploaden van de veranderingen
en voegt ze toe aan de veranderingen van andere gebruikers. 



De software
===========

UNIX
----

Draait u Linux, FreeBSD of een andere moderne UNIX kloon, dan hoeft u alleen
de officiële SVN software te installeren, versie 1.0 of hoger voor uw OS. De meeste
Linux distributies worden met SVN geleverd. 

Er is ook een cross-platform QT GUI frontend voor SVN getiteld `eSVN <http://esvn.umputun.com/>`__, 
beschikbaar voor alle Unixen(GNU/Linux distributies, FreeBSD, Sun Solaris en anderen), 
Mac OS X en Windows. 

.. Note:: De server draait Subversion 1.1 wat toegankelijk is voor een client
          met versie 1.0, 1.1 of 1.2.

SVN werkt niet met UTF-8 lokalisatie. U moet de locale veranderen naar ISO8859
voordat u SVN acties uitvoert.


AmigaOS
-------

Als u AmigaOS draait, dan heeft u een TCP/IP stack en een geïnstalleerde SVN port
nodig. Één optie is de Amiga port van Olaf Barthel die op AmiNET__ gevonden kan
worden (zoek naar "subversion").

__ http://main.aminet.net/


Windows
-------

Als u Microsoft Windows (TM) gebruikt, dan kunt u de TortoiseSVN__ SVN client gebruiken,
als u de Windows Explorer prettig vind werken. Het programma is Open Source en gratis, 
kent veel features en heeft goede ondersteuning. Verzekerd u er wel van dat u bestanden
met UNIX-stijl regeleindes verstuurd, anders kunt u de code generatie *onklaar* maken.
Als bestanden die u veranderd de eol-style:native SVN property ingesteld hebben
(sowieso niet bij nieuwe bestanden), dan kunt u het dit vergeten; de regel eindes (EOL) zullen
dan automatisch geconverteerd worden. 
Voorbeelden van editors die zulke EOL bestanden kunnen opslaan 
zijn o.a. `Notepad++ <http://notepad-plus.sourceforge.net>`__, TigerPad 
(uitgebreide notepad, gratis), `DOS Navigator OSP <http://dnosp.com/>`__, en vele anderen.
Ook de eSvn client is beschikbaar, zoals eerder vermeld.

__ http://www.tortoisesvn.net/


MacOS X
-------

Als u MacOS X draait kunt u één van de verschillende SVN ports gebruiken, waaronder
Martin Ott's `Subversion OSX overzetting`__. Het is aangeraden daarna ook een extra SVN GUI client te installeren
zoals `svnX`__. svnX is op moment de meest up-to-date SVN client voor OS X. Het ondersteund
onder andere integratie met de freeware `Textwrangler`__ texteditor (voormalig BBedit Lite).
Let wel dat u de karakter codering instelt van 'Macos Roman' op 'ISO latin 1' (ISO8859-1), wat
in preferences kan. Voor TW staan alle andere instellingen standaard correct. 
Genoemde applicaties zijn gratis en beschikbaar als Universal Binary voor PPC en Intel Macs.
Ook de eSvn client is beschikbaar, zoals eerder vermeld.

__ http://www.codingmonkeys.de/mbo/
__ http://www.lachoseinteractive.net/en/community/subversion/svnx/
__ http://www.barebones.com/products/textwrangler/



Inloggen op de server
=====================

Anders dan bij CVS, hoeft u niet steeds in te loggen op de server. In plaats daarvan
vraagt SVN wanneer uw login en paswoord nodig zijn.

.. Note:: 

    Het AROS archief (repository) draait op een paswoord beveiligde SVN server, wat
    betekend dat u eerst om `toegang moet vragen`__ om mee te mogen werken aan 
    de ontwikkeling. Op verzoek van Amiga Inc., is de anonieme alleen-lezen toegang
    tot het archief uitgeschakeld.
          
__ contribute#joining-the-team



De AROS broncode ophalen
========================

Om een kopie van de AROS broncode te verkrijgen moet u het "checkout" commando gebruiken,
op de volgende manier::

    > svn checkout https://svn.aros.org/svn/aros/trunk/AROS

Dit zal een map met de naam AROS creëren en deze vullen met alle sources. Dit kan
enige tijd duren als u een langzame netwerk verbinding heeft.
De aparte "contrib" module bevat programma's van derden die naar AROS overgezet zijn.
U moet ook hier een checkout toepassen mocht u alle AROS soorten willen bouwen::

    > cd AROS
    > svn checkout https://svn.aros.org/svn/aros/trunk/contrib

.. Tip:: 

    Na de checkout zal SVN onthouden waar de bron vandaan kwam.



De extra broncode ophalen
=========================

Behalve de AROS hoofd broncode, die we zojuist met "checkout" ophaalden, bevat
het AROS archief ook zaken die niet direct gerelateerd zijn aan het besturingssysteem. 
Bijvoorbeeld de "binaries" module, die o.a. afbeeldingen bevat zoals screenshots en
achtergronden, of de "documentation" module die de bronnen van de website bevat. 

U kunt een lijst met alle modules opvragen met::

    > svn ls https://svn.aros.org/svn/aros/trunk/



Uw lokale archief updaten
=========================

Nadat u de broncode hebt opgehaald, wilt u deze misschien periodiek updaten 
om de nieuwste wijzigingen te ontvangen van andere ontwikkelaars. Hiervoor moet 
u het "update" commando gebruiken::

    > cd AROS
    > svn update
    
Dit zal de veranderingen van andere ontwikkelaars toevoegen aan uw lokale
kopie, samen met eventuele nieuwe bestanden en mappen die toegevoegd zijn. 
Als iemand veranderingen "commit" die u ook lokaal hebt gemaakt, dan zal SVN proberen 
deze veranderingen automatisch samen te voegen. Hebt u beiden echter dezelfde regels 
aangepast dan kan dit samenvoegingsproces falen. Mocht dit gebeuren dan zal SVN 
dit opmerken en **beide** versies in het bestand plaatsen, gescheiden door ``<<<<``. 
U zult daarna handmatig het bestand moeten aanpassen om het conflict op te lossen.

.. Warning:: 

    Ook al voegt SVN veranderingen van andere auteurs met de uwe samen, dit betekend
    nog niet dat alles direct in orde is. SVN let alleen om de *tekstuele* inhoud;
    er kan zo best nog een *logisch* conflict zijn ontstaan na de samenvoeging (bijv.: de andere
    ontwikkelaar heeft de semantiek veranderd van een functie die u gebruikt heeft in
    uw veranderingen). U zult daarom altijd de bestanden moeten controleren die samengevoegd zijn
    om te zien of de inhoud nog klopt.



Veranderingen doorsturen
========================

Als u veranderingen hebt gemaakt waarvan u oordeelt dat u deze wil delen met
andere ontwikkelaars, gebruik dan het "commit" commando om deze aan het archief toe
te voegen::

    > svn commit

U kunt ook een lijst specificeren met bestanden voor de commit; anders zal SVN recursief
afdalen vanaf het begin van de huidige map, alle bestanden zoekend die veranderd zijn
en deze bij de commit versturen. Voordat SVN de veranderingen doorstuurt, zal het u vragen
om een "log boodschap" in te voeren. Deze log boodschap bevat typisch een korte samenvatting van
wat u veranderd heeft, met in sommige gevallen ook de redenatie daarachter. Goed geschreven
log boodschappen zijn erg belangrijk, omdat deze het makkelijker maken voor andere ontwikkelaars
om snel te zien wat u hebt gedaan - en misschien ook waarom. Alle log boodschappen worden
per dag verzameld en gemaild naar de ontwikkelaars mailing lijst, zodat iedereen
op de hoogte kan blijven van de ontwikkelingen in het code archief. 

Voordat u de veranderingen in een map commit, is het beter om eerst een update uit
te voeren. U kunt dan zien of iemand tussentijds nog aan de betreffende bestanden
heeft gewerkt. Mocht dit gebeurd zijn, dan zult u mogelijke conflicten in het bestand op moeten
lossen voordat u deze aanpassingen commit. Test a.u.b. ook eerst uw veranderingen voor een commit; 
om te voorkomen dat deze het build proces onderbreken.



Toevoegen van nieuwe bestanden en mappen
========================================

Om nieuwe bestanden en mappen aan het archief toe te voegen, gebruik het "add" commando::

    > svn add file.c
    > svn add dir

SVN zoekt niet automatisch naar nieuw toegevoegde bestanden en mappen; u moet dat zelf doen.
Nadat u het bestand heeft toegevoegd, moet het "commit" commando gebruikt worden
om deze ook daadwerkelijk aan het archief toe te voegen. 

.. Note::

    Voeg geen gegenereerde bestanden (gebruikelijk *mmakefile*, *strings.h*) toe aan 
    het archief. Anders zullen deze bestanden niet geupdate worden als de source is veranderd.



Eigenschappen instellen
=======================

Besturingssystemen verschillen met welke code ze gebruiken voor regeleindes. Om er zeker
van te zijn dat text-bestanden die niet zijn gegenereerd voor Linux, dezelfde regeleindes hebben
is het nodig het volgende commando uit te voeren::

    svn propset svn:eol-style native <source.c>

Subversion kan ook speciale sleutelwoorden invoegen in source bestanden. Om dit in te stellen
moet u het volgende commando uitvoeren op het bestand::

    svn propset svn:keywords Author Date Id Revision <main.c>

.. Note::

    U kunt Subversion zodanig configureren dat deze automatisch de eigenschappen instelt
    van bepaalde bestandstypes. Consuleert u hiervoor uw Subversion documentatie.



Importeren
==========

Als u een grote collectie bestanden toevoegt, bijv. de broncode van een bestaande
applicatie, wordt het gebruik van "svn add" snel vervelend. Voor dit soort acties
kunt u daarom beter "svn import" gebruiken. Helaas is het SVN handleiding onderdeel 
over het import commando nogal slecht geschreven; een aanvulling is daarom op zijn
plaats:

1. Zet de bestanden en mappen die u wilt importeren waar u ook wilt, zolang
   dit **niet** binnen uw lokale werkende kopie is. Het draaien van het "import"
   commando op een map die zich bevind binnen een bestaande lokale SVN kopie
   kan tot enkele heel vreemde resultaten leiden; beter dus om zulke problemen
   te ontwijken.

2. Ga naar de map die de bestanden bevat die u wilt importeren:: 

       > cd map-1.2.3

3. Importeer de bestanden met het "svn import" commando::

       > svn import -m <logMessage> <destinationPath>
       > svn import -m <logboodschap> <doelpad>

   Dit zal recursief alle bestanden naar het archief importeren, vanuit de 
   huidige map en lagere mappen, naar het doelpad en met de log boodschap
   die u opgegeven heeft. In werkelijkheid worden niet *alle* bestanden 
   toegevoegd: SVN zal sommige bestandsnamen negeren, zoals de gebruikelijke
   namen van back-ups en verborgen bestanden, zoals ``#?.bak``,
   ``.#?`` en ``#?~``.

   Desondanks kunt u beter eerst alle bestanden verwijderen die u niet in het
   archief zou willen hebben. Onderbreek SVN overigens niet tijdens een import als
   u ziet dat een verkeerd bestand toegevoegd wordt. Vermeld het en verwijder het
   bestand later zelf.
   
   Als voorbeeld: stel u wilt de SVN 1.1.3 broncode in 
   de "contrib/development/versioning/svn" map importeren::
   
      > cd subversion-1.1.3
      > svn import -m "Begin import van SVN 1.11.12" 
      \ https://svn.aros.org/svn/aros/trunk/contrib/development/versioning/svn



Aanvullende informatie
======================

Aanvullende en gedetailleerdere informatie over SVN kan natuurlijk gevonden worden
in de handleiding en informatie bestanden die meegeleverd worden met SVN zelf.
Ook zijn er veel sites die handige uitleg en gidsen bevatten die misschien nog
wel makkelijker te lezen zijn. De volgende (Engelstalige) pagina's worden aangeraden:

+ `Versie Controle met Subversion (Engels)`_
+ `Subversion Home (Engels)`_

.. _`Versie Controle met Subversion (Engels)`: http://svnbook.red-bean.com/
.. _`Subversion Home (Engels)`:               http://subversion.apache.org/
