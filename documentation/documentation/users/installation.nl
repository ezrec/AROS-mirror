=====================
AROS Installatie Gids
=====================

:Authors:   Stefan Rieken, Matt Parsons, Adam Chodorowski, Neil Cafferkey, Sergey Mineychev
:Copyright: Copyright ² 1995-2006, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Needs to be updated for some AROS ports. Can be translated.
:Abstract:
    Deze instructies begeleiden u bij het installeren van de verschillende AROS soorten.

    .. Warning:: 
    
        AROS is alpha kwaliteit software. Dit betekend dat het momenteel vooral
        leuk is om mee te spelen en cool om voor te ontwikkelen. Mocht u hier gekomen
        zijn met het idee dat AROS af is en/of een compleet en bruikbaar besturingssysteem is,
        dan moeten we u teleurstellen. AROS is daar nog lang niet, al gaan we 
        wel langzaam in de juiste richting. 


.. Contents::


Downloaden
==========

AROS is op moment nog zwaar onder ontwikkeling. Het resultaat daarvan is dat u
moet kiezen tussen stabiliteit en mogelijkheden. Op moment zijn er zo twee typen
binaire pakketten beschikbaar om te downloaden: snapshots en nightly builds.

Snapshots worden handmatig en op wisselende momenten aangemaakt, meestal als
er zojuist veel werk is verricht aan AROS sinds de laatste snapshot, en/of indien iemand
zich gemotiveerd voelt om een nieuwe snapshot te maken. In andere woorden: er is geen vaste
release planning. Ook al maken we de snapshots vaak op tijden dat AROS stabiel
lijkt, dan nog is er geen garantie dat ze fout-vrij draaien, laat staan werken,
op uw machine. Dat gezegd, we proberen de snapshots wel te testen op een groot
aantal machines, zodat ze in de praktijk vaak goed werken.

Nightly builds (Nachtelijke Compilaties) worden, zoals de naam aangeeft, automatisch
elke avond gemaakt van het (ontwikkelaars) Subversion archief en bevatten daarmee
de nieuwste code. Echter, ze zijn ook niet getest en kunnen compleet kapot of vol fouten
zitten, die zelfs uw systeem (installaties) kunnen vernietigen mocht u ongeluk hebben. 
Meestal werken ze echter goed.

Ziet u de `download pagina`_ voor meer informatie over welke snapshots en nightly builds
beschikbaar zijn en hoe u ze kunt downloaden.


Installatie
===========

AROS/i386-linux and AROS/i386-freebsd
-------------------------------------

Systeemeisen
""""""""""""

Om AROS/i386-linux of AROS/i386-freebsd te draaien heeft u het volgende nodig:

+ Een werkende FreeBSD 5.x of Linux installatie (maakt niet uit welke distributie
  u draait, zolang deze relatief recent is).
+ Een geconfigureerde en werkende X server (bijvoorbeeld X.Org of XFree86).

Dit is alles. 


Uitpakken
"""""""""

Nu AROS/i386-linux en AROS/i386-freebsd beiden gehoste versies zijn van AROS,
is de installatie simpel. Zorg dat u de betreffende archieven voor uw platform 
verkrijgt via de `download pagina`_ en pak ze uit waar u wilt::

    > tar -vxjf AROS-<version>-i386-<platform>-system.tar.bz2

Als u ook het contrib archief heeft gedownload, dan wilt u deze misschien ook uitpakken (overigens,
de inhoud is al bijgesloten in het systeem archief en de LiveCD)::

    > tar -vxjf AROS-<version>-i386-all-contrib.tar.bz2


Starten
"""""""

Nadat u alle bestanden heeft uitgepakt kunt u AROS als volgt starten:: 

    > cd AROS
    > ./aros


.. Note:: 
    
    Tenzij u XFree86 3.x of ouder draait, zult u merken dat het AROS scherm niet
    goed ververst (bijv. als een ander venster erover heen wordt gesleept). Dit word veroorzaakt
    door het feit dat AROS de "backingstore" functionaliteit gebruikt van X, wat echter
    standaard uit staat in XFree86 4.0 en later. Om dit aan te zetten zult u de volgende
    regel moeten toevoegen aan de "device section" van uw grafische kaart, dit in het X configuratie
    bestand (typisch genaamd
    ``/etc/X11/xorg.conf``, ``/etc/X11/XF86Config-4`` or
    ``/etc/X11/XF86Config``)::

        Option "backingstore"

    Een complete 'device section' kan er daarna zo uitzien::

        Section "Device"
            Identifier      "Matrox G450"
            Driver          "mga"
            BusID           "PCI:1:0:0"
            Option          "backingstore"
        EndSection


AROS/i386-pc
------------

Systeemeisen
""""""""""""

U heeft een standaard PC nodig (i486 of Pentum-gebaseerd) met *PS/2 muis* en PS/2 of AT toetsenbord,
IDE hard disk en CD-ROM, (S)VGA video kaart en monitor. Optioneel kan ook een PC-compatibele
VM (virtual machine) gebruikt worden. 
Aangeraden wordt ten minste 16 MB RAM en een VGA kaart die aan de VESA standaard voldoet.
Er zijn tevens algemene (versnelde) drivers (HIDD's) voor ATI en nVidia kaarten. Tot slot 
kunt ook een netwerkkaart toevoegen (sommigen worden ondersteund) om te proberen te netwerken in AROS.
Mochten er problemen zijn, raadpleeg dan de FAQ voor informatie over uw soort hardware. 


.. Note:: 
    
    We raden op moment af om AROS/i386-pc te installeren op een hard disk [#]_.
    Echter, u zult AROS wel moeten installeren om sommige van de features te zien en te testen;
    een oplossing moeten we hier daarom wel vermelden. We raden u in dat geval aan om ieder geval **niet**
    te installeren op uw 'werk' machine of één waarvan de HD belangrijke informatie
    bevat! We nemen geen verantwoordelijkheid voor enig verlies van data tijdens de installatie.
    Tot slot zijn foutrapporten over de installatie altijd welkom.
    
   
Installatie media
"""""""""""""""""

De optimale installatie media voor AROS/i386-pc is de CDROM, gezien het hele
systeem op een enkele schijf past (met alle optionele software). 
Het maakt de installatie eenvoudiger, sinds u geen installatie-marathon 
hoeft te doorlopen door het systeem op verschillende floppies te zetten.

Gezien nog niemand AROS op CDROM verkoopt (of op enige andere media), heeft u wel een
CD brander nodig om zelf de installatie schijf te creëren. 

CDROM
^^^^^

Schrijven
'''''''''

Download simpelweg het ISO bestand van de `download pagina`_ en brand deze naar een CD
met uw favoriete CD brand programma. Er zijn meerdere freeware CD brand programma's 
beschikbaar voor vele platformen. Windows gebruikers kunnen we zo aanraden 
`InfraRecorder <http://infrarecorder.sourceforge.net>`__ - het is gratis, 
klein, snel en simpel. 


Opstarten
'''''''''

De makkelijkste manier om AROS te draaien is via de AROS installatie CD, mits u
computer het starten (of booten) vanaf CDROM ondersteunt. Het kan misschien wat aanpassingen in
de BIOS opzet met zich meebrengen om dit mogelijk te maken; in veel gevallen staat dit
standaard uit. Voer daarna simpelweg de CD in de eerste CDROM drive
en herstart de computer. Het opstarten van de AROS Live CD is volautomatisch, waarna u -als
alles goed werkt - na enige tijd een mooi scherm te zien krijgt.

Overigens, mocht uw computer het starten van CDROM niet direct ondersteunen dan kunt u ook een
boot floppy_ maken en deze in combinatie gebruiken met de CDROM. Voer simpelweg
zowel de boot floppy als de CD in hun respectievelijke drives en herstart. AROS
zal dan eerst van de floppy starten, maar nadat de meest belangrijke zaken zijn geladen
(waaronder de CDROM bestandssysteem handler) zal verder geladen worden van de CDROM.


Floppy
^^^^^^

De floppies kunnen handig zijn als opstartschijf indien de BIOS van uw PC
het starten vanaf CD niet ondersteund, of voor sommige heel oude PC's. Deze versie wordt nog
altijd onderhouden.


Schrijven
'''''''''

Om een boot of opstart floppy te maken, moet u de downloadimage ophalen van de `download pagina`_,
het archief uitpakken en de image naar een floppy wegschrijven. Gebruikt u een
UNIX-type besturingssysteem (zoals Linux of FreeBSD), dan kunt u dit doen met het volgende
commando::

    > cd AROS-<version>-i386-pc-boot-floppy
    > dd if=aros.bin of=/dev/fd0

Gebruikt u Windows, dan heeft u rawrite_ nodig om het bestand naar een floppy
te schrijven. Zie a.u.b. de documentatie van rawrite_ voor verdere instructies.
Er bestaat ook een GUI versie genaamd rawwritewin.


Opstarten
'''''''''

Stop de floppy in de drive en herstart de computer. Het starten/booten
werkt volledig automatisch; als alles goed werkt moet u na enige tijd het AROS scherm zien.

Installeren op een harde schijf
"""""""""""""""""""""""""""""""

Om te beginnen, weet dat u hierbij **GEWAARSCHUWD** bent dat de HD installatie incompleet 
is en daarmee **gevaarlijk** voor uw gegevens. Wees er dus zeker van dat de PC die u gebruikt 
geen belangrijke informatie bevat. Het gebruik van een virtuele machine wordt aangeraden, 
gezien dit het risico minimaliseert en toestaat AROS ook te gebruiken en te testen op een 
'werk' machine. Er zijn inmiddels veel gratis VM's beschikbaar, waaronder QEMU en VMWare.


Voorbereiden van de HD
^^^^^^^^^^^^^^^^^^^^^^

Bereid eerst uw HD voor - ofwel een echte of een virtuele drive image - op
de installatie. Voor een echte drive kan dit betekenen dat u deze in moet pluggen
(altijd een goed begin) om deze vervolgens op te zetten via uw BIOS. Voor de virtualiser's of
emulator's virtuele schijf, zult u waarschijnlijk een optie moeten kiezen om een
nieuw schijfbestand te creëren. Deze kunt u daarna als één van de opstartschijven instellen
van uw virtuele PC (let wel, de CD drive moet het eerste opstart apparaat zijn tijdens de 
installatie).

Een volgende stap is het verwijderen van bestaande partities op de HD, om fouten
tijdens te voorkomen bij het maken van de aankomende partities. Installeren van
AROS naast andere besturingssystemen is mogelijk, maar vraagt aanvullende vaardigheden
en wordt hier niet verder behandeld. Op dit moment leren we u alleen om AROS te installeren als
enigste systeem op een HD.


Partitionering
^^^^^^^^^^^^^^

Enkele partitie installatie

Hier zullen we leren hoe we AROS kunnen installeren als het enigste systeem op een PC, geplaatst
op een enkele partitie. Dit is het eenvoudigste type installatie.

Dit hoofdstuk kan een beetje moeilijk zijn gezien de installatie feature nog incompleet is. 
Om te beginnen: ga ervan uit dat een *standaard regel* voor dit proces is om te *herstarten*
na elke significante verandering aan het bestandssysteem (we zullen opmerken wanneer nodig).
Herstarten betekend het sluiten van het HDToolbox venster als dit open is, om daarna de computer
of VM te herstarten - een harde reset. U kunt ook een zachte reset proberen door <reboot> ENTER
te typen in het CLI venster.

Om te beginnen, zoek op de AROS CD naar de tool genaamd *HDToolBox*. Het is te vinden
in de Tools map. Dit zal voor een tijdje uw gereedschap zijn. Als u het programma start, 
krijgt u een venster te zien met een device-type kiezer. In dit voorbeeld (hier en later), gebruiken we
een echte of virtuele IDE hard drive (ook bekend als een ATA hard drive). Dus, klikken op de 
*ata.device* optie zal in het linker venster Devices:1 tonen. Dus, dit is onze HD. Door op
deze keuze te klikken openen we de beschikbare HD lijst. 

In deze lijst moet nu onze HD staan. Als het een virtuele HD is, zullen we iets zien in de zin van
*QEMU Harddisk* of de tegenhanger van VMWare. Als uw HD een echte is, ziet u hier de naam ervan.
Gebeurd dat niet, dan moet u er zeker van zijn dat u de HD goed heeft voorbereid. Het klikken op
de HD naam geeft ons om te beginnen meer informatie:

    Size: <Grootte van de HD>
    Partition Table: <type van de huidige PT; moet 'unknown' zijn na cleanup>
    Partitions: <nummer van partities op de HD; moet 0 zijn nu we net zijn gestart>

Nu moeten we een nieuwe partitie tabel creëren. Voor een PC moeten we een *PC-MBR* type
tabel aanmaken. Hiervoor moet u de *Create Table* knop aanklikken en uit de lijst *PC-MBR*
kiezen. Kies OK.

Nu moeten we de wijzigingen terugschrijven naar de schijf. Klik hiervoor op de HD naam
en klik *Save Changes*. Antwoord *Yes* in de confirmatie dialoog. Sluit nu het HDToolbox
venster en herstart het systeem vanaf de Live CD.

Start na het herstarten HDToolbox opnieuw. Nu, na het binnengaan van het *ata.device* item,
moeten we de volgende informatie zien: "Partition table: PC-MBR, Partitions:0". Dit is OK,
gezien we nog geen partities ingesteld hebben. Dat gaan we nu doen. Klik op de HD naam en ga
naar de partitie lijst. De lijst is nu leeg. Klik op de *Create Entry* knop, selecteer alle ruimte
door op de lege ruimte te klikken en kies *OK*. Nu moet u in de lijst de "Partitie 0" item
zien. Kies deze door te klikken, wat u de volgende informatie moet geven::

    Size: <Partitie grootte. Bijna gelijk aan de HD grootte>
    Partition table: Unknown <Nog niet aangemaakt>
    Partition type: AROS RDB Partition table <Dit is OK>
    Active: No <Niet actief>
    Bootable: No <Niet opstartbaar>
    Automount: No <Zal niet mounten na opstarten systeem>

Hierna is zijn er twee verschillende keuzes - maak een partitie aan in een RDB tabel of
de gebruikelijke PC-MBR partitie. RDB (Rigid Disk Block) heeft de voorkeur vanwege
compatibiliteit en is ook gebruikt in AmigaOS HDD partitionering ; hier kunnen we
ook gebruiken. Tenminste; AROS ondersteund FFS partities gemaakt met een gewone
PC-MBR tabel, zoals gebruikelijke PC partities zoals FAT/NTSF/etc. De tweede optie kan
gezien worden als moderner en meer compatibel met enkele nieuwe AROS programma's. 
We behandelen nu beiden opties.

*FFS in RDB*
Klik op de *Create Table* knop, selecteer *RDB tabel* en klik OK. Om de veranderingen
op te slaan, ga *één niveau* omhoog door de *Parent* knop te kiezen. Selecteer nu de
HD naam opnieuw en klik de *Save Changes* knop. Antwoord tweemaal *Yes* in het confirmatie
dialoogvenster. Sluit nu HDToolbox en herstart de machine.

*FFS in MBR*
...moet nog toegevoegd worden

Na het opstarten, open HDToolbox (u kon het raden). De informatie voor onze
Partitie 0 moet dezelfde zijn, behalve dat de partitietabel nu als RDB ingesteld staat (of niet).
Nu moet deze partitie nog actief gemaakt worden. Klik hiervoor op de *Switches* knop, vink de
*Active* checkbox en klik *OK*. En dan? Jawel, bewaar de veranderingen, door één niveau omhoog
te gaan en te klikken op de betreffende knop. Sluit en herstart.

Waarom trouwens al dit herstarten? Wel, HDToolbox en de systeem libraries zijn verre van compleet en
bevatten nog aardig wat bugs; het herstarten nu helpt om deze naar hun initiale stap terug te zetten.
(In de toekomst zal dit overigens in één keer kunnen én ook via een nette installer.)

Na het opstarten, moet HDToolbox ons tonen dat Partitie 0 actief is geworden. Nu moeten we onze
disk creëren en daar AROS op installeren. Ga één niveau omlaag door te klikken op de "Partition 0"
optie. Klik nu op de Add Entry knop en kies alle lege ruimte. Nu zult u hier een "DH0" optie zien,
welke onze schijf is. Erop klikken geeft de volgende informatie::

    Size: <...>
    Partition Table: Unknown (dat is OK)
    Partition Type: Fast Filesystem Intl <OK>
    Active: No <OK>
    Bootable: No <deze moet omgeschakeld worden naar Yes>
    Automount: No <deze moet omgeschakeld worden naar Yes>

Ga nu *2 niveau's omhoog* naar de HD naam, klik Save Changes, bevestig, sluit
en herstart. Na de herstart (wordt een beetje saai inmiddels, niet?) moeten
we de switches instellen voor de DH0 drive in HDToolbox. We gaan naar de DH0 optie 
en zetten de relevante knoppen en checkboxes naar: *Bootable: Yes* en
*Automount: Yes*. Bewaar de veranderingen nadat u 2 niveau's omhoog bent gegaan, 
conformeer en herstart.

*Hoe lang nog???* We zijn bijna halverwege succes. Na de herstart en het nakijken van alle
instellingen voor DH0, moeten we goed controleren of alles goed staat. Daarna kunnen we HDToolbox
sluiten zonder enige twijfels. Nu wordt het tijd voor enige CLI magie.

Formatteren
^^^^^^^^^^^

We moeten nu de DH0 drive formatteren om deze bruikbaar te maken. Op moment biedt
AROS ondersteuning voor twee bestandssystemen - Fast FileSystem(FFS) en Smart FileSystem(SFS).
FFS is bekend om de betere stabiliteit en compatibiliteit tegenover de meeste programma's,
SFS is daarentegen meer fout-bestendig en moderner, maar kent nog menige problemen met programma's.
Op moment moeten we trouwens wel FFS kiezen, omdat de GRUB bootloader nog geen ondersteuning bied voor SFS
(GRUB2 wel). Hou overigens rekening dat u met SFS ook problemen kunt krijgen met sommige geporte
software (zoals gcc). Open nu het CLI venster (rechterklik op de top menubalk en
kies Shell van het eerste Wanderer menu). Bij de prompt, toets in het Info commando 
(type ``info`` en druk Enter). U moet nu DH0 zien in de lijst als 
``DH0: Not a valid DOS disk``. Nu zullen we deze gaan formatteren met het volgende commando:

    >format DRIVE=DH0: NAME=AROS FFS INTL
    About to format drive DH0:. This will destroy all data on the drive. Are 
    you sure ? (y/N)

Voer y in, druk Enter en wacht een seconde. De string ``Formatting...done`` zal nu verschijnen. 
Mocht u een error krijgen, check dan alle partitie parameters in HDToolbox - u kan misschien 
wat vergeten zijn, waarna u deze stap herhaalt.

Mocht u problemen hebben met formatteren (denk ERROR boodschappen, vooral als u RDB
partities gebruikt), hoe onwaarschijnlijk ook, dan kunt u de goede oude Amiga
FORMAT64 utility gebruiken::

    >extras/aminet/format64 DRIVE DH0: Naam AROS FFS INTL


Nu moet het Info commando tonen::

    >DH0: <grootte>  <gebruikt> <vrij> <vol 0%> <fouten> <r/w status> <FFS> <AROS>

Dat is het. Tijd voor de pre-installatie herstart.

.. Note:: Mocht dit alles zo saai lijken dat u het niet kunt uitstaan? Er is  
          enige opluchting mocht u AROS alleen willen gebruiken in een virtual machine. 
          Om te beginnen, kunt u een pre-installatie pakket downloaden, zoals *WinAROS/WinAROS
          Lite* - dit systeem is al geïnstalleerd, maar kan oud zijn. Ten tweede 
          kunt u in de `AROS Archives`_ zoeken voor de *Installation Kit* die
          kant-en-klare virtuele HD's bevat, waarop direct geïnstalleerd kan worden.
          U kunt dan dus de vorige procedure overslaan en direct een nieuwe AROS versie installeren. 


Het systeem kopiëren
^^^^^^^^^^^^^^^^^^^^

Na de herstart, zal het opvallen dat u de AROS HD op uw desktop ziet, en dat deze
leeg is. We gaan deze nu vullen met bestanden.

Nu Drag`n`Drop ondersteuning is geïmplementeerd in AROS, kan het hele systeem
simpelweg gekopieerd worden door deze te slepen van de LiveCD naar de DH0: map.
Het is dan alleen nog nodig het bestand dh0:boot/grub/menu.lst te vervangen met
dh0:boot/grub/menu_dh.lst.DH0.

Er is ook een installer voor AROS, maar anders dan het incomplete HDToolbox, kan 
deze 'normaal' gebruikt worden. Tenminste, we gaan het proberen. Dus hierbij de eerste
manier om te installeren.

1. Start *InstallAROS* in de Tools map. U zult het welkomst scherm zien
dat u dezelfde dingen verteld als ik u net deed - we gebruiken een alpha versie. 
Tijd om het ook eens echt te gebruiken ;) Er is een *Proceed* knop voor u om op te klikken. 
Daarna zal u de AROS Publieke Licentie zien, die u moet accepteren om verder te gaan. 
Daarna volgt het installatie opties venster (als het nee zegt, vink dan de relevante box *uit*)::

    Show Partitioning Options...    []
        <Nee. Hebben we net gedaan>
    Format Partitions               []
        <Nee. Hebben we net gedaan>
    Choose Language Options         []
        <Nee. Beter om later te doen>
    Install AROS Core System        [V]
        <Ja, nodig. We zijn hier om dat te doen>
    Install Extra Software [V] 
        <Ja. Vink uit als u alleen de lite installatie wilt>
    Install Development Software    []
        <Nee. Trouwens nu alleen nog een loze optie>
    Show Bootloader Options         [V]
        <Ja, de bootloader zal anders niet geïnstalleerd worden>

Laat me opmerken dat *Show Partitioning Options* onkiesbaar en grijs kan staan
in geval dat de installer geen geschikte partitie kon vinden. Nadat u alle punten die 
we nodig hebben aan- of uitgevinkt heeft, klik *Proceed*. Het volgende scherm toont ons
daarna de mogelijke installatie doelen::

    Destination Drive
    [default:DH0]
    
    DH0  <dat is correct>
    
    Use 'Work' Partition                        [] 
        <vink uit, we installeren nu alles-in-één>
    Copy Extras and Developer Files to Work?    [] 
        <zelfde als boven>
    Work drive ... <overgeslagen>
    
Nu we alles uitgevinkt hebben, klik *Proceed*. Het scherm met de bootloader
opties verschijnt nu. Hier kunnen we alleen selecteren of GRUB, de *GRand Unified Bootloader*,
geïnstalleerd moet worden naar DH0 en op welk apparaat. Klik nogmaals *Proceed*. 

Nu zal het venster zeggen dat we klaar zijn om te installeren. Klik nog een keer op *Proceed*. 

Hierna verschijnt de voortgangsmeter als de bestanden worden gekopieerd. Wacht een
moment tot dit proces eindigt. Daarna zal het eind scherm verschijnen met een *Reboot*
vinkje. Vink deze aan en kies Proceed. Dit is dan nog niet alles - er is nog één laatste stap te gaan.
Onze machine zal nu herstarten met dezelfde instellingen als eerder, vanaf de Live CD.

De bootloader installeren
^^^^^^^^^^^^^^^^^^^^^^^^^

Nu zien we nog steeds onze AROS schijf, met alle bestanden erop. 
De bootloader was al geïnstalleerd, dus wat missen we nog? 
Wel: heeft u een verse nightly build gebruikt dan moet 'GRUB <http://en.wikipedia.org/wiki/GRUB>'__ 
al geïnstalleerd staan en werkend zijn. U kunt dan deze stap overslaan. Zo niet, lees a.u.b. verder.

Voor oudere versies (voor nov. 2006):
Er was een bug in GRUB, welke voorkwam dat de installatie de eerste keer correct plaatsvond.
Mocht u nu niet kunnen opstarten en berichten krijgen als GRUB GRUB FRUB etc lees dan het volgende.
Een tweede herinstallatie helpt meestal het probleem op te lossen. Dus moeten we InstallAROS nog
een keer draaien. Herhaal alle eerdere stappen vanaf punt 1, maar vink alle checkboxen uit. Na
de laatste klik wordt GRUB ge-herinstalleerd en zal er een scherm verschijnen dat u vraagt het 
schrijven te bevestigen. Antwoord ja zo vaak als nodig is, sluit het Install programma en zet de 
machine uit.

Alternatief kan GRUB ook geïnstalleerd worden via shell met dit commando::

    c:install-i386-pc device ata.device unit 0 PN <pn> grub dh0:boot/grub kernel dh0:boot/aros-i386.gz

waar PN <pn> (of PARTITIONNUMBER <pn>) het nummer is van de partitie waar de GRUB 
bootloader wordt geïnstalleerd.


Klaarmaken om te starten
^^^^^^^^^^^^^^^^^^^^^^^^

We hebben zojuist onze eerste cursus installatie alchemie gevolgd; uw AROS
installatie moet nu klaar zijn voor gebruikt. Verwijder de Live CD van de CD drive (of deactiveer
het starten van CD in de VM) en probeer uw verse installatie!

Mocht iets fout gaan, dan kunnen er antwoorden zijn...

Problemen oplossen
^^^^^^^^^^^^^^^^^^

Het installatie proces is één van de meest gevraagde onderwerpen op onze forums,
meestal door nieuwelingen. U kunt ook de FAQ bekijken voor antwoorden op u vragen.

AROS installeren naast andere systemen
""""""""""""""""""""""""""""""""""""""

In de beschreven stappen was AROS het *enige* systeem op de de HD. Men kan AROS
echter ook installeren in een multiopstart omgeving samen met andere besturingssystemen.
Echter, dit is en blijft een complexe onderneming. Wordt vervolgt ...

De HD voorbereiden
^^^^^^^^^^^^^^^^^^
Wordt vervolgt ...

AROS/i386-PPC-hosted
--------------------

Systeemeisen
""""""""""""

Moet nog geschreven worden door iemand.

AROS/m68k-backport of AfA
-------------------------

Dit is geen gewone native/hosted smaak van AROS, maar iets dat een *backport*
wordt genoemd. In essentie is het een set van libraries en binaries die de capaciteiten
van het originele AmigaOS verbeteren. AfA staat voor AROS voor Amigas.

Systeemeisen
""""""""""""

Moet nog geschreven worden door iemand.

Installatie
"""""""""""

Installatie:

+ kopieer de map AfA_OS_Libs naar uw sys: Amigaschijf Bootpartitie.
  Mocht dit u niet aanstaan dan kunt u deze ook elders kopiëren en vervolgens
  de AfA_OS: map assignen naar de map waar de AfA_OS_Libs zich bevinden. 
+ kopieer Libs:freetype2.library naar uw sys:libs map
+ kopieer C:AfA_OS_Loader naar uw sys:fonts map
+ kopieer Fonts: naar uw sys:fonts map. Mocht u meer fonts willen, gebruik 
  de fonts van AROS of MOS
+ kopieer prefs: naar uw sys:prefs map

Om tijdens opstarten te laden, zet u de AfA_OS_Loader in uw S:startup-sequence,
even voor IPrefs. De laadregel moet wel na patching tools zoals MCP 
of picasso96/cgx komen, omdat zij AfA_OS Functies weer 'terug' patchen. 

Als u de regel uitbreid met de parameter MOUSESTART (in hoofdletters), dan moet u
de linkermuisknop ingedrukt houden tijdens het opstarten om de modules te 
laden. Zo niet dan worden deze overgeslagen.

Om te zien of het allemaal goed werkt, start u het "TextBench" programma, onderdeel
van het archief. De snelheid van de TTF anti-aliasing zal momenteel geen snelheidsrecords
vestigen gezien deze niet is geoptimaliseerd (zie aatext source), maar we hopen dat ze
bruikbaar genoeg is op zelfs een 060/50Mhz.


Voetnoten
=========

.. [#] Het *is* wel mogelijk om AROS/i386-pc op een harddrive te installeren, maar
       de procedure is verre van automatisch en gebruikersvriendelijk. Ook zijn de
       benodigde tools nog onder (zware) ontwikkeling waardoor deze foutgevoelig
       kunnen zijn. We raden daarom -officieel- de harddisk installatie niet aan 
       voor onervaren gebruikers op het tijdstip dat deze gids werd geschreven.


.. _`download pagina`: ../../download

.. _rawrite: http://uranus.it.swin.edu.au/~jn/linux/rawwrite.htm

.. _`AROS Archives`: http://archives.aros-exec.org
