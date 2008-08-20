=====
Porty
=====

:Authors:   Adam Chodorowski, Matthias Rustler 
:Copyright: Copyright Š 1995-2006, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Done.

.. Contents::


Úvod
====

Vzhledem k tomu, ¾e AROS je pøenosný operaèní systém, je k dispozici pro nìkolik
rùzných platforem. "Port" AROSu je pøesnì to, co znamená, to jest verze
AROSu upravená pro urèitou platformu.


Pøíchutì - flavors
------------------

Porty jsou rozdìleny na dvì hlavní varianty, v terminologii AROSu také "flavors",
jmenovitì "nativní" a "hostované".

Nativní porty bì¾í pøímo na hardwaru a mají absolutní kontrolu nad
poèítaèem. V budoucnu se stanou doporuèovaným zpùsobem pro bìh AROSu,
proto¾e poskytují lep¹í úèinnost i výkon, ale v souèasné dobì jsou pøíli¹
neúplné, aby mohly být pou¾ívány (pøinejmen¹ím pro vývoj).

Hostované porty bì¾í na jiném operaèním systému a k hardwaru nepøistupují
pøímo, ale pou¾ívají prostøedky poskytované hostitelským OS.
Výhodou hostovaných portù je jejich jednodu¹¹í programování, proto¾e není
nutné psát low-level ovladaèe. A vzhledem k tomu, ¾e vývoj AROSu je¹tì
není self-hosted (sebehostitelský - nelze kompilovat AROS z AROSu), výraznì urychlují
programování, proto¾e vedle sebe mù¾e bì¾et vývojové prostøedí i AROS
a pro vyzkou¹ení nového kódu není tøeba ztrácet èas neustálým restartováním.


Pojmenování
-----------

Rùzné porty AROSu jsou pojmenovány ve tvaru <procesor>-<platforma>, kde <procesor> je
procesor architektury a <platforma> je symbolický název platformy.
Platforma portu mù¾e být buï hardwarová, jako napø. "pc" nebo "amiga",
u nativních portù, nebo operaèní systém, jako napø. "linux" nebo "freebsd",
u hostovaných portù. V pøípadech, kdy není zøejmé, ¾e se jedná o AROS, pøidá
se na zaèátek názvu portu "AROS/", napøíklad "AROS/i386-pc".


Pøenositelnost
--------------

AROS spustitelné soubory pro konkrétní procesor jsou pøenosné na v¹echny porty,
které tento procesor pou¾ívají, co¾ znamená, ¾e spustitelné soubory kompilované pro
"i386-pc" budou v pohodì pracovat na "i386-linux" i "i386-freebsd".


Souèasné porty
==============

Ní¾e je uveden seznam v¹ech portù AROSu, které jsou v provozuschopném stavu
a/nebo aktivnì vyvíjeny. Ne v¹echny z nich jsou dostupné ke sta¾ení, proto¾e nemusí
být buï dostateènì kompletní nebo mají kompilaèní po¾adavky, které nemù¾eme
v¾dy splnit kvùli omezeným zdrojùm.


AROS/i386-pc a x86-64
---------------------

:Varianta:  Nativní
:Stav:      Funkèní, neúplná podpora ovladaèù
:Udr¾ována: Ano

AROS/i386-pc je nativní port AROSu na bì¾né IBM PC AT poèítaèe a
kompatibilní pou¾ívající x86 (nebo x86-64) rodinu procesorù. Název je tak trochu
zavádìjící, proto¾e AROS/i386-pc ve skuteènosti vy¾aduje tøídu procesoru alespoò 486
kvùli pou¾ití nìjakých instrukcí, které na 386 nejsou dostupné. Tento stroj musí být
té¾ zalo¾en na PCI sbìrnici.

Tento port funguje pomìrnì dobøe, ale máme pouze nejzákladnìj¹í podporu
ovladaèù. Jedním z nejvìt¹ích omezení je, ¾e v souèasné dobì mù¾eme
nabídnout grafickou akceleraci pouze kartám s èipem nVidia a ATI. Ostatní
grafické adaptéry musí být pou¾ívány s generickými VGA a VBE grafickými
ovladaèi (bez akcelerace). Pracuje se na více ovladaèích, ale jejich
vývoj je ponìkud pomalej¹í, proto¾e máme jen asi 2.5 hardwarových specialistù.
Tento port je k dispozici ke sta¾ení.


AROS/m68k-pp
------------

:Variatna:  Nativní
:Stav:      Èásteènì funkèní (v emulátoru), neúplná podpora ovladaèù
:Udr¾ována: Ano

AROS/m68k-pp je nativní port AROSu pro kapesní poèítaèe øady Palm
a kompatibliní ("pp" znamená "palm pilot", co¾ bylo jméno prvního
handheldu této øady). To znamená, ¾e v budoucnu bude mo¾né
nosit AROS s sebou v kapse...

Tento port je v souèasné dobì velmi nedodìlaný. Vìt¹inou funguje (bì¾í v emulátoru,
proto¾e nikdo zatím je¹tì nechce riskovat po¹kození drahého hardwaru), ale stále
je na nìm spousta práce. Má grafický ovladaè, ale ¾ádný pro vstup.
Tento port není v souèasné dobì k dispozici ke sta¾ení.


AROS/i386-linux
---------------

:Varianta:  Hostovaná
:Stav:      Funkèní
:Udr¾ována: Ano

AROS/i386-linux je hostovaný port AROSu pro operaèní systém Linux [#]_
bì¾ící na procesorech rodiny x86.

Je to rozhodnì nejkompletnìj¹í port, proto¾e vìt¹ina vývojáøù
v souèasné dobì k vývoji AROSu pou¾ívá Linux a je daleko ménì
ovladaèù, které se pro nìj musí psát. Tento port je k dispozici ke sta¾ení.


AROS/i386-freebsd
-----------------

:Varianta:  Hostovaná
:Stav:      Funkèní
:Udr¾ována: Ano (5.x)

AROS/i386-freebsd je hostovaný port AROSu pro operaèní systém FreeBSD
bì¾ící na procesorech rodiny x86.

Tento port je pomìrnì kompletní, proto¾e sdílí vìt¹inu kódù s portem AROS/i386-linux,
ale vzhledem k tomu, ¾e není mnoho vývojáøù, kteøí pou¾ívají FreeBSD,
zùstává trochu pozadu. Sna¾íme se sestavovat AROS/i386-freebsd, kdy¾ dìláme
snapshoty, ale ne v¾dy je to mo¾né, tak¾e nemusí být poka¾dé k dispozici
ke sta¾ení.


AROS/ppc-linux
--------------

:Varianta:  Hostovaná
:Stav:      Funkèní
:Udr¾ována: Ano

AROS/ppc-linux je hostovaný port AROSu pro operaèní systém Linux
bì¾ící na procesorech rodiny PPC.

Pøedkompilovaná verze mù¾e být sta¾ena ze `Sourceforge`__.
Pøestavení vy¾aduje opravenou verzi gcc3.4.3. Diff soubor se nachází v contrib/gnu/gcc.

__ http://sourceforge.net/project/showfiles.php?group_id=43586&package_id=194077


Dodatek
=======

.. [#] Ano, víme, ¾e Linux je vlastnì jen jádro a ne celý 0S, ale
       je mnohem krat¹í napsat Linux, ne¾ "operaèní systém zalo¾ený na
       linuxovém jádøe, nìkteré bì¾né GNU nástroje a X window
       system". Tato optimalizace rozsahu je ov¹em negována tím, ¾e se musí psát
       toto vysvìtlení pro pedantské ètenáøe, ale stejnì...

