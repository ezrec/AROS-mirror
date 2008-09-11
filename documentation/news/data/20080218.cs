================
Stav aktualizace
================

:Autor:   Paolo Besser
:Datum:   18.02.2008

Poslední zprávy
---------------

Michal Schulz usilovnì pracuje na portování AROSu na desku SAM440
od Acube Systems a dosahuje zajímavých `výsledkù`__. Zde je nìkolik
slov z jeho posledního pøíspìvku na `jeho blogu`__: "Rozhodl jsem se oddìlit
jádro (a knihovny nahrávané spoleènì s ním) od u¾ivatelského prostoru.
Jádro je nahráno nìkde v prvních 16MB pamìti RAM a pak pøemístìno
na virtuální adresu v horní èásti 32-bitového adresového prostoru.
Bootstrap loader pracuje stejným zpùsobem, jakým pracoval x86_64 bootstrap.
Celá èást pamìti nahoru od jádra je pouze pro ètení (read-only) a celá
èást dolù od jádra je zapisovatelná (writable). Proto¾e jsem od pøírody
zlý, moje jádro SAM440 AROSu si bude nenasytnì brát ve¹kerou pamì»
*pod* svým fyzickým umístìním pro sebe. Tato pamì» (nìkolik megabajtù)
bude pou¾ita jako místní úlo¾i¹tì pro kernel a bude zamezeno jakékoli
formì pøístupu ze strany u¾ivatele."

Nic Andrews pracuje na Wandereru, aby ho zdokonalil a opravil
nìkolik otravných chyb. V souèasné dobì "trochu pøepracovává
renderovací kód pro tøídu Wanderer iconlist. Prùbì¾ným cílem je
umo¾nit vykreslování ikon/pozadí z vyrovnávací pamìti tak,
¾e napøíklad s pou¾itím kachlového vykreslování (tiled rendering) pro pozadí
iconlistu nebude zpùsobováno znatelné blikání ikon, jak se to dìlo doteï".
Podrobnìj¹í informace o jeho práci jsou zveøejnìny na `jeho blogu`__.


Internet jednodu¹eji
--------------------

Michael Grunditz oficiálnì uvolnil první verzi `SimpleMail`__ 0.32 beta
pro AROS Research Operating System. SimpleMail má vìt¹inu funkcí,
které jsou v souèasných moderních email klientech zapotøebí a stále
se dál vyvíjí. Aktuální verze mù¾e být sta¾ena z `Archivu`__.

Robert Norris udìlal znaèný pokrok s Travellerem (jeho webový prohlí¾eè pro AROS
zalo¾ený na Webkitu). Aby v¹echno správnì fungovalo, je stále zapotøebí
dokódovat nìkolik chybìjících funkcí a knihoven, nicménì jeho port Cairo.library
je na dobré cestì a celkem dobøe uspìl pøi zobrazování nìkolika stránek.
Opravdu slibné `snímky obrazovky`__ byly publikovány na `jeho
blogu`__.


Dal¹í zprávy
------------

Joao "Hardwired" Ralha nedávno napsal nìkolik dobrých pøíruèek pro AROS.
Nicménì nejsou je¹tì dokonèené a on v souèasné dobì hledá nìkoho, kdo mu pomù¾e.
Dostupné dokumenty jsou `AROS u¾ivatelská pøíruèka`__ (50% kompletních),
`AROS pøíruèka pro shell`__ (70%) a `AROS instalaèní pøíruèka`__ (25%).
Autor je k dosa¾ení na `jeho webu`__.

Alain Greppin naportoval TeXlive na AROS, ("bounty" dokonèeno). Více informací
o tomto poèinu na `jeho webu`__.

Tomek 'Error' Wiszkowski pracuje na Frying Pan, aplikaci
pro vypalování CD/DVD. Na AROS-Exec.org zveøejnil `nìkolik snímkù obrazovky`__.
Verze 1.3 pro AROS (shareware) mù¾e být sta¾ena z `webu
této aplikace`__. Aby správnì fungovala, opravil také nìkolik chyb
v ATA rozhraní AROSu.

... a pro v¹echny, kdo si toho nev¹imli: souborový systém AROS FFS nedávno získal vlastnost
pro kontrolu a opravu integrity na vadných oddílech. U¾ ¾ádné nepou¾itelné read-only oddíly!


__ http://msaros.blogspot.com/2008/01/ive-promised-to-show-you-some.html
__ http://msaros.blogspot.com
__ http://kalamatee.blogspot.com/
__ http://simplemail.sourceforge.net/index.php?body=screenshots
__ http://archives.aros-exec.org/index.php?function=showfile&file=network/email/simplemail_beta_aros-i386.tgz
__ http://cataclysm.cx/2008/02/18/cow-launched
__ http://cataclysm.cx
__ http://archives.aros-exec.org/share/document/manual/aros_user_manual_version_0.56a.pdf
__ http://archives.aros-exec.org/share/document/manual/aros_shell_manual_version_0.7a.pdf
__ http://archives.aros-exec.org/share/document/manual/aros_install_manual_version_0.25a.pdf
__ http://aros-wandering.blogspot.com
__ http://www.chilibi.org/aros/texlive
__ http://aros-exec.org/modules/newbb/viewtopic.php?viewmode=flat&topic_id=2569&forum=2
__ http://www.tbs-software.com/fp/welcome.phtml
