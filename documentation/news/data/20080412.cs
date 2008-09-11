================
Stav aktualizace
================

:Autor:   Paolo Besser
:Datum:   12.04.2008

Distribuce
----------

Nedávno byly vydány dvì AROS distribuce. VmwAROS LIVE! a
nová verze WinAROS. První je bootovatelné a instalovatelné
pøedkonfigufované prostøedí zalo¾ené na VmwAROS virtuálním stroji,
druhá je QEMU uzpùsobený kompletní virtuální stroj s IDE a
aktualizovaným vývojovým prostøedím. Podrobnìj¹í informace jsou dostupné na na¹í
`download stránce`__. Vyzýváme u¾ivatele a vývojáøe aplikací, aby si
tyto distribuce stáhli.

Poslední zprávy
---------------

Krysztof Smiechowicz a Alain Greppin poskytli veøejnosti binární
balíèek gcc/g++ 3.3.1 pro architekturu i386 - zalo¾ený na Fabiových
opravách, ke sta¾ení z Archivu. To je samozøejmì dobrá zpráva pro
v¹echny, kdo se zajímají o vývoj a portování softwaru na AROS,
ale není jedinou: v Archivu mù¾e¹ najít také novou verzi Murks!IDE
s podporou pro C++ - nejlep¹í integrované vývojové prostøedí pro AROS,
které nám pøinesli Krysztof Smiechowicz a Heinz-Raphael Reinke.

Nastal také èas na provedení velkých oprav. Krysztof Smiechowicz zaèal
pøezkoumávat úplnost API, zatímco Barry Nelson prozkoumal, protøídil a
zaèal spravovat ná¹ bug tracker. Vìt¹ina z u¾ opravených chyb byla
odstranìna ze seznamu.

Nic Andrews a Alain Greppin koneènì implementovali grub2 do AROSu.
Nic také na Aros-Exec ukázal `snímek obrazovky`__. Skvìlou zprávou je,
¾e u¾ivatelé se mohou koneènì zbavit pomalých FFS oddílù a spou¹tìt
systémové soubory z SFS oddílù. Tato mo¾nost je¹tì není doporuèována, proto¾e
stále existují urèité nedostatky v kompatibilitì s nìkterými AROS
aplikacemi.

Alain Greppin dokonèil AROS DHCP "bounty" s pøíkazem dhclient.
AROS teï mù¾e získat IP adresu automaticky. Také naportoval
`TeXlive`__.

Tomasz Wiszkowski a Michal Schulz pracují na zdokonalení ata.device.
Byla pøidána poèáteèní podpora pro nìkteré chipsety Serial ATA:
"SATA øadièe podporující re¾im dìdìní operací (legacy operation mode) by nyní mìly být
funkèní (ale to neznamená, ¾e bychom získali podporu AHCI)".

__ http://aros.sourceforge.net/download.php
__ http://i175.photobucket.com/albums/w131/Kalamatee/AROS/grub2gfx-1.jpg
__ http://www.chilibi.org/aros/texlive

