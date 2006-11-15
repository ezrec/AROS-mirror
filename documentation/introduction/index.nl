==========================
Korte introductie van AROS
==========================

:Authors:   Aaron Digulla, Stefan Rieken, Matt Parsons, Adam Chodorowski 
:Copyright: Copyright © 1995-2002, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Almost finished, I think...


.. Include:: index-abstract.nl


Doelen
======

De doelen van het AROS project zijn om een besturingssysteem te cree‘ren dat:

1. Zo compitabel als mogelijk is met AmigaOS 3.1;

2. Overgebracht kan worden naar verschillende soorten (hardware) architecturen en processoren, zoals x86, PowerPC, Alpha, Sparc, HPPA en anderen;

3. Zowel binair compitabel is met de Amiga en broncode compitabel is met andere hardware;

4. Zowel in een 'stand-alone' versie gedraaid kan worden, die direct start van een hard-disk, alsook in een ge‘muleerde (hosted) vorm. Laatstgenoemde variant draait in een venster op een bestaand besturingssysteem (bijv. Linux), is bedoeld om software te ontwikkelen en staat tevens ook om gelijktijdig Amiga en standaard applicaties te draaien;

5. Verbeteringen brengt in de functionaliteit van het AmigaOS.

Om dit doel te bereiken gebruiken we verschillende methodes. Belangrijkste is dat we veel gebruik maken van Internet. Je kan deelnemen aan ons project zelfs als je maar ŽŽn enkele OS functie kan schrijven. De nieuwste versie van de broncode is 24 uur per dag beschikbaar, waarbij aanpassingen elk moment van de dag ingevoegd kunnen worden. Verder zorgt een kleine database met open taken  ervoor dat er geen dubbelop werk verricht wordt. 


Geschiedenis
============

In 1993 zag de situatie er voor de Amiga slechter uit dan normaal. Enkele Amiga fans kwamen daarom samen en begonnen een discusie over wat ondernomen moest worden om de acceptatie van hun geliefde platform te verbeteren. Al vrij snel werd de hoofdreden gevonden voor het gebrek aan succes voor de Amiga: het was de verspreiding, ofwel het gebrek daaraan. De Amiga als platform zou een veel breder gespreide basis moeten krijgen om deze aantrekkelijker te maken voor zowel gebruikers als ontwikkelaars. Plannen werden bedacht om dit doel te bereiken. ƒŽn van deze plannen was om de fouten uit het AmigaOS te halen, een ander om van het oude AmigaOS een modern besturingssysteem te maken. Zo onstond het AOS project.  

Maar wat is nu precies een fout? En hoe zouden deze fouten gerepareerd moeten worden? Wat zouden de mogelijkheden zijn van een zogenaamd *modern* besturings systeem? En hoe zouden deze in het AmigaOS verwerkt moeten worden? 

Twee jaar later werd er nog gediscussierd over deze punten, terwijl er nog geen regel programmacode was geschreven (tenminste, niemand had nog iets gezien). Discussies verliepen nog altijd in een patroon waarin de ŽŽn naar voren bracht "we hebben ... nodig", waarop iemand anders antwoorde "lees de oude mails" of "dit is onmogelijk om te doen vanwege..." wat dan weer beantwoord werd met "je hebt het fout omdat...", enzovoorts.

In de winter van 1995 had Aaron Digulla genoeg van deze situatie - hij stuurde een RFC (request for comments/aanvraag voor commentaar) naar de AOS mailing lijst, waarin hij vroeg over een minimale basis voor het systeem, iets waarover iedereen het eens was. Verschillende mogelijkheden werden geopperd, waaruit de conclusie getrokken kon worden dat vrijwel iedereen een open (source) besturingssysteem wenste, wat compitabel was met AmigaOS 3.1 (Kickstart 40.68). Op dit idee werden voortaan alle discussies gebaseerd, om te overwegen wat nu wel en niet mogelijk zou zijn.

Zo begon het werk en werd het AROS project geboren.
