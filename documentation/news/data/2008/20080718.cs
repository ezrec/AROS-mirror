================
Stav aktualizace
================

:Autor:   Paolo Besser a Saimon69
:Datum:   18.07.2008

Poslední aktualizace
--------------------

Omlouváme se, ¾e "Stav aktualizace" pøi¹el tak pozdì, ale AROS tým
velmi tvrdì pracuje "under the hood", aby pøinesl lep¹í
operaèní systém. Ní¾e je spousta zajímavých novinek, o kterých se mluví.

Stanislaw Szymczyk dokonèil port linuxem hostované verze
na platformu x86-64, a ú¾asnì pomáhá samokompilování AROSu
portováním potøebných nástrojù. Ten poslední byl abc-shell a
dosahuje neuvìøitelných výsledkù.

Krzysztof Smiechowicz kontroluje úplnost AROS API, aby bylo
mo¾né sledovat kompatibilitu s AmigaOS a stav celého projektu.
Výsledky si mù¾e¹ kdykoli prohlédnout na na¹í aktualizované `stavové stránce`__.

Pavel Fedin skvìle vylep¹il funkènost HDToolBox: oddíl nyní mù¾e být
pøemístìn, èi mu mù¾e být zmìnìna velikost. Tyto nové funkce v¹ak stále
potøebují testování. Pavel také naportoval BHFormat a zprovoznil
ve Wandereru formátování diskù.

Pavel Fedin, Krzysztof Smiechowicz a Tomasz Wiszkowski také tvrdì
pracují na na¹ich ata.device a s ATAPI souvisejících souborech, aby se
zbavili problémù s kompatibilitou, které znemo¾òují AROSu na nìkterých
konfiguracích správnì bootovat.

Michal Schulz stále pracuje na portování AROSu na SAM440EP.
Nedávno vydal první pracovní `beta verzi`__, vlastníci SAMu si ji mohou
vyzkou¹et sami. Nezkou¹ejte ji na jiných PPC architekturách,
nebude fungovat.

Gianfranco Gignina pomohl najít a opravit hodnì chyb v Zune,
a zaèal pracovat na pøenositelné a více nezávislé verzi
Wandereru. Cílem projektu je umo¾nit jednodu¹¹í portování Wandereru
i na ostatní AmigaOS platformy.

Máme tu novou, velmi cennou implementaci jazyka E na AROS:
je to `PortablE`__ a urèitì stojí za vyzkou¹ení!

Nick Andrews pí¹e ovladaè pro sí»ovou kartu Intel Gigabit E1000 na AROS, který
umo¾ní pøipojení k síti velkému mno¾ství základních desek, na nich¾ je tato
karta integrována.

Paolo Besser vydal novou verzi jeho distribuce VmwAROS 0.8b,
která pøiná¹í lep¹í vzhled a kompatibilitu.
Stejnì jako v minulosti, VmwAROS je k dispozici ve dvou verzích:
`live CD`__, které bì¾í na x86 hardwaru a mù¾e být
nainstalováno na pevný disk, a `virtuální prostøedí`__ pro
VMware.

__ http://aros.sourceforge.net/it/introduction/status/everything.php
__ http://msaros.blogspot.com/2008/05/try-it-yourself.html
__ http://cshandley.co.uk/portable
__ http://live.vmwaros.org
__ http://ve.vmwaros.org

