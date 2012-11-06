================
Stav aktualizace
================

:Autor:   Paolo Besser
:Datum:   15.11.2007

Poslední zprávy
---------------

AROS se v posledních týdnech doèkal mnoha vylep¹ení a také opravení
spousty chyb. Pro pøedstavu, Neil Cafferkey opravil nìkolik záva¾ných
chyb v jeho milovaném `AROS Instalátoru`__; Nic Andrews pracoval na svém
ovladaèi sí»ové karty RTL8139; Robert Norris opravil chybu v hlá¹ení
o souboru, které bylo pøedtím ¹patnì nastaveno. Ale to byly jmenovány
pouze tøi. 

Robert Norris pøidal SDL ovladaè pro linux hostící AROS. Díky tomu
si mù¾e¹ vytvoøit hostovaný AROS, který nevy¾aduje X server
(teï u¾ Xka nemusí¹ instalovat). Teoreticky by to mohlo pomoci
s hostováním na ostatních platformách (kdekoli, kde je SDL), i kdy¾ je to trochu
pomalej¹í ne¾ s X serverem.

Matthias Rustler naportoval ptplay.library pro AROS. Tato knihovna
pøevádí Protracker moduly do zvukových vzorkù. Dále také naportoval
jednoduchý pøehrávaè - ShellPlayer. Tyto budou v noèních
sestaveních, v ¹uplíku Extras/MultiMedia/Audio.

Matthias Rustler také vytvoøil poèáteèní port `Wazp3D`__ od Alaina Thelliera 
pro AROS. Wazp3D je knihovna, která by mìla být kompatibilní
se slavnou knihovnou Warp3D.library pro AmigaOS 68040. Portování
3D Amiga her na AROS by mìlo být jednodu¹¹í. Wazp3D mù¾e také pracovat jako
softwarový renderer, který klame ostatní aplikace tím, ¾e se tváøí jako
3D hardwarový ovladaè.

Michal Schulz uèinil nìkolik velkých krokù vpøed s jeho
`x86-64`__ portem AROSu. Den 64 bitového AROSu je stále blí¾.
Michal také pøidal podporu SSE instrukcí.

Petr Novák pøelo¾il `aros.org do èe¹tiny`__.


__ http://aros-exec.org/modules/newbb/viewtopic.php?topic_id=2319
__ http://ftp.ticklers.org/pub/aminet/driver/video/Wazp3D.readme
__ http://msaros.blogspot.com/2007/10/very-close.html
__ http://www.aros.org/cs


