==========
Portningar
==========

:Authors:   Adam Chodorowski, Matthias Rustler 
:Copyright: Copyright © 1995-2006, The AROS Development Team
:Version:   $Revision: 24547 $
:Date:      $Date: 2006-06-15 17:41:03 +0200 (Thu, 15 Jun 2006) $
:Status:    Done.

.. Contents::


Introduktion
============

Eftersom AROS är ett portabelt operativsystem, så är det tillgängligt för flera
olika plattformar. En "portning" av AROS är precis som det låter, en version av
AROS portad till en specifik plattform.


Smaker
------

Portningar är uppdelade i två större grupper, eller "smaker" inom AROS termioligi,
dessa är "native" och "hosted".

Native-portningar körs direkt på hårdvaran och har total kontroll över datorn.
Dom kommer att vara det rekommenderade sättet att köra AROS i framtiden eftersom
det ger överlägsen prestanda och effektivitet, men dom är för närvarande för
inkompletta för att vara användbara (åtminstone för utveckling). 

Hosted-portningar körs ovanpå ett annat operativsystem och har inte tillgång till
hårdvaran direkt, de använder faciliteterna som operativsystemet ger dom. Fördelarna
med hosted-portningar är att dom är enklare att skriva, eftersom det inte är 
nödvändigt att skriva låg-nivå drivrutiner. Eftersom AROS uveckling även inte är 
själv-hosted ännu (Vi kan inte kompilera AROS inifrån AROS) så snabbar det upp
programmeringen, eftersom vi kan köra utvecklingsmiljön och AROS sida-vid-sida utan
att ödsla tid på konstanta omstarter för att testa ny kod. 

Namngivning
-----------

De olika AROS-portningarna är namngivna i formen <cpu>-<platform>, där <cpu> är
CPU-arkitekturen och <platform> är ett symboliskt namn av plattformen. Plattformen
av en portning kan antingen vara en hårdvara för native-portningar, som "pc" eller
"amiga", eller ett operativsystem för hosted-portningar, som "linux" eller
"freebsd". I fall där det inte är uppenbart att ämnet i fråga handlar om
AROS så är det vanligt med att använda ett prefix "AROS/" till portnings-namnet,
vilket t.ex. kan ge namnet "AROS/i386-pc".


Portnings-kompabilitet
----------------------

AROS exekverbara filer för en specifik CPU är kompatibla över alla portningar
som använder den CPU:n, vilket betyder att exekverbara filer för "i386-pc" kommer
att fungera på "i386-linux" och "i386-freebsd".

Existerande portningar
======================

Nedan finns en lista på alla AROS portningar som fungerar och/eller aktivt
utvecklas. Alla av dessa är inte tillgängliga för nerladdning, eftersom dom
eventuellt inte är tillräckligt kompletta eller har kompileringskrav som vi
inte alltid kan uppnå pga. ej tillräckliga resurser.


AROS/i386-pc
------------

:Flavour:    Native
:Status:     Fungerande, ej komplett drivrutinsupport
:Maintained: Ja

AROS/i386-pc är en native-portning av AROS till vanliga IBM PC AT datorer och
kompatibla som använder x86-processorfamiljen. Namnet är faktiskt en smula
vilseledande eftersom AROS/i386-pc faktiskt kräver åtminstone en 486 CPU,
eftersom användandet av en del instruktioner inte finns tillgängliga på en 386 CPU.

Denna portning fungerar ganska bra, men vi har endast väldigt grundläggande
drivrutinfunktioner. En av de största begränsningarna är att vi för
tillfället endast har stöd för "accelerated graphics" på nVidia och
ATI-grafikkort. Andra grafikkort måste användas med "generic (non accelerated)
VGA och VBE grafikdrivrutiner. Det utvecklas drivrutiner för fler grafikkort,
men utvecklingen går relativt trögt eftersom vi endast har omkring 2,5 
hårdvaruutvecklare.
Denna portning är tillgänglig för nerladdning.


AROS/m68k-pp
------------

:Flavour:    Native 
:Status:     Delvis fungerande (i en emulator), ej komplett drivrutinsstöd.
:Maintained: Ja

AROS/m68k-pp är native-portningen av AROS för Palm-serien av handdatorer och
kompatibla ("pp" står för "palm pilot", vilket var namnet på de första handdatorerna
i denna serie). Detta betyder att du eventuellt kan ta med dig AROS i din ficka
när du reser i framtiden...

Denna portning är för tillfälligt väldigt "rå". Oftast fungerar den (Om man kör den
i en emulator, eftersom ingen vågar ta risken att förstöra deras dyra hårdvara
för tillfället) men det är fortfarande mycket arbete kvar. Det finns en 
grafikrutin, men inga för input.
Denna portning är inte tillgänglig för nerladdning för tillfället.


AROS/i386-linux
---------------

:Flavour:    Hosted
:Status:     Fungerande
:Maintained: Ja

AROS/i386-linux är den hostade portningen av AROS till operativsystemet Linux
[#]_ som körs på x86-processorer.

Detta är den mest kompletta portningen av AROS funktionsmässigt, eftersom
de flesta utvecklarna använder Linux när de utvecklar AROS och det är
mycket färre drivrutiner att skriva.
Denna portning finns tillgänglig för nerladdning.

AROS/i386-freebsd
-----------------

:Flavour:    Hosted
:Status:     Fungerande
:Maintained: Ja (5.x)

AROS/i386-freebsd är den hostade portningen av AROS till operativsystemet FreeBSD
som körs på x86-processorer.

Denna portning är relativt komplett eftersom den delar den mesta av koden
med AROS/i386-linux, men eftersom det inte finns så många utvecklare som använder
FreeBSD så ligger utvecklingen lite efter. Vi försöker att kompilera AROS/i386-freebsd
när vi gör snapshots, men det är inte alltid möjligt, då det kanske inte alltid finns
tillgängligt för nerladdning.

AROS/ppc-linux
---------------

:Flavour:    Hosted
:Status:     Fungerande
:Maintained: Ja

AROS/ppc-linux är den hostade portningen av AROS till operativsystemet Linux
som körs på PPC-processorer.

En förkompilerad version kan laddas ner från `Sourceforge`__.
Omkompilering kräver en patchad gcc3.4.3. Diff-filen finns i contrib/gnu/gcc.

__ http://sourceforge.net/project/showfiles.php?group_id=43586&package_id=194077

Fotnot
======

.. [#] Ja, vi vet att Linux bara är en kärna och inte ett helt OS, men det är mycket
       kortare att skriva än "operativsystem baserat på Linux-kärnan, en del av
       de vanligare GNU-verktygen och X fönsterhanterarsystemen". Denna optimering
       av utrymme går naturligtvis att bestrida av de mest pedantiska läsarna, men men...
