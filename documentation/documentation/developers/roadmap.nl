================
AROS 1.0 roadmap
================

:Authors:   Adam Chodorowski, Sergey Mineychev
:Copyright: Copyright Š 2003, The AROS Development Team
:Version:   $Revision: 24965 $
:Date:      $Date: 2006-11-13 01:02:54 +0100 (Mon, 13 Nov 2006) $

:Abstract:  
    This is a roadmap intended to outline the requirements that need to be
    fullfilled before AROS 1.0 can be released. It is *not* meant to force
    developers to work on certain things (since it's not possible in an open
    source project anyway) but should rather be seen as a recommendation
    where we would like people to focus their efforts. Hopefully, this will
    help organize the development effort and make us all work more in the
    same direction. 


Versies
-------

Om een AROS 1.0 versie te rechtvaardigen moet aan onderstaande eisen 
worden voldaan voor de i386-pc en i386-linux soorten (deze worden 
hier "vereiste soorten" genoemd). Andere soorten (of ports) worden niet nodig
geacht voor 1.0, dit om de hoeveelheid werk klein te houden.

Eisen
-----

Gezien de huidige status niet objectief kan worden weergegeven in procenten,
proberen we van deze punten een kwalitatieve schatting van de vordering te maken.
Ook mogen de verdere aanbevelingen onderstreept worden.

1.  AmigaOS 3.1 API compatibiliteit, uitgezonderd delen die geacht worden
    niet-overzetbaar, overbodig of niet de moeite waard te zijn. 
    
    Om iets als overbodig te kenmerken terwijl implementatie wel mogelijk is, en dat 
    ook in een overbrengbare methode, moet goede redenen hebben. Bijvoorbeeld als zoiets 
    maar zelden gebruikt wordt door applicaties, terwijl de moeite om dit te implementeren substantieel is.
    
    Huidige status: Het grootste deel van AmigaOS 3.1 API is geïmplementeerd, op sommige
    plekken zelfs voorbijgestreefd. Desondanks zijn er nog veel fouten te verhelpen.


2.  Gedeeltelijke AmigaOS 3.5 en 3.9 API comptabiliteit. We kozen de delen waarvan wij vonden 
    dat ze handig en nuttig zijn om te hebben, de rest latend voor wat het is. 
    
    Ter voorbeeld, het is *zeer* onwaarschijnlijk dat we ReAction compatibiliteit willen hebben
    nu we toch al gekozen hebben voor Zune als de standaard GUI toolkit. (En het implementeren
    van de ReAction API is geen triviale taak). Uiteraard moet dit wel bediscussieerd worden voordat 
    er tot een gedetailleerde lijst wordt besloten. 
    
    Huidige status: <?>
    

3.  Volledige GUI toolkit. Dit betekend dat Zune compleet MUI API
     compatibel moet zijn mét een afgemaakt preferenties programma.
    
     Huidige status: <?>


4.  Standaard applicaties gelijk die met AmigaOS 3.1 geleverd worden.
    
    Dit betekend *niet* dat we exact dezelfde applicaties moeten hebben als in
    AmigaOS, laat staan dat deze hetzelfde moeten werken. Wel moet de functionaliteit
    ongeveer dezelfde zijn.

    Huidige status: De meeste basic applicaties zijn ontwikkeld voor AROS. 

    
5.  Geluids ondersteuning, ofwel API compatibiliteit en basis applicaties. Er 
    moet op zijn minst één driver zijn voor de vereiste AROS soorten. 
    
    Huidige status: Op moment is AHI overgezet, ook zijn er enkele drivers (een paar)
    voor de i386-port. Applicaties worden ontwikkeld.
    
    
6.  Netwerk ondersteuning. Hieronder vallen een TCP/IP-stack en enkele basis-applicaties
    zoals email en SSH clienten, samen met een *simpele* web browser. Er zou minstens 
    één NIC driver moeten zijn voor de vereiste AROS versies.
    
    De eisen voor de web browser moeten niet te hoog zijn, maar het moet op
    zijn minst wel mogelijk zijn dat men op één manier kan 'browsen' (ook al is dat
    in tekst mode).
    
    Huidige status: de AROSTCP die we nu hebben is een oeroude maar werkende implementatie 
    van de AmiTCP stack. Een aantal applicaties zijn ontwikkeld (o.a. een FTP, telnet en IRC client),
    maar zijn geen onderdeel van het systeem zelf. Meer applicaties zijn in ontwikkeling zoals een 
    web browser.

        
7.  Zelf-gehoste ontwikkeling omgeving en SDK voor ontwikkelaars. Specifiek
    omvat dit alle software die nodige is om AROS te compileren, zoals GCC,
    GNU binutils, GNU make, enz. Het moet mogelijk zijn AROS te compileren
    onder AROS.
    
    De ABI voor ondersteunde architecturen (alleen i386 op dit moment) moet 
    definitief zijn voor 1.0. Zo gauw 1.0 vrijgegeven is, moet de ABI stabiel zijn
    voor een redelijk langdurige periode.
    
    Huidige status: AROS heeft een complete gcc port, wat toestaat applicaties te
    compileren. Enkele GNU tools ontbreken nog, zodat zelf-compilatie niet
    mogelijk is.
    
    
8.  Uitgebreide documentatie voor ontwikkelaars. Dit omvat een complete
    referentie gids over alle libraries, devices, classes en ontwikkeling tools;
    compleet met handleidingen en uitleg om de subsystemen te begrijpen en te
    overzien. Ook moet een migratie/overzetting handleiding beschikbaar
    zijn.
    
    Huidige status: <?>

    
9.  Uitgebreide documentatie voor gebruikers. Dit omvat een complete
    commando referentie, uitleg en installatie-, configuratie- en andere handleidingen.
    
    Huidige status: Documentatie bestaat en wordt grootschalig vertaald naar
    meerdere talen. Toch moeten de gidsen, handleidingen en het help
    systeem nog compleet gemaakt worden.


10. Substantieel testen en complete fout controles. De 1.0 release moet praktisch
    fout vrij zijn en een *zeer* stabiele release worden. We moeten niet het soort
    fiasco's krijgen dat sommige open source projecten hadden met hun ".0" versies.
        
    Dit zal waarschijnlijk een langdurige feature bevriezing vragen, gevolgd door een 
    broncode bevriezing en verschillende test versies voor feedback en tests.  
    Feature verzoeken worden niet gezien als fouten, tenzij iets benodigd (maar vergeten)
    is in de (voorlaatste) test versies. Een vraag als "een film speler is nodig" is bijv. niet
    toepasselijk, terwijl "de tekst editor zou een 'save' menu optie moeten hebben" dat wel is.

    Huidige status: Op moment kan geen stop gemaakt worden omdat de eigenschappen nog
    niet compleet zijn. Ook moeten er nog vele foutencorrecties gemaakt worden, maar een groeiend
    aantal gebruikers brengt hoop met zich mee. Het zoeken van fouten, deze doorgeven en/of verhelpen 
    zijn nodig.