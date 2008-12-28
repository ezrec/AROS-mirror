===============================================
Spou¹tìní klasických Amiga aplikací z Wandereru
===============================================

:Authors:   Matthias Rustler
:Copyright: Copyright © 2007, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Done.

.. Contents::

Úvod
----

E-UAE umí emulovat klasický Amiga hardware a umo¾òuje spou¹tìt aplikace
pod moderním hardwarem a moderními operaèními systémy. Dokonce je mo¾né
spustit E-UAE pomocí ikony z Wandereru takovým zpùsobem, ¾e nahraje pøímo
i tebou zvolenou aplikaci. Trik je v tom, ¾e pou¾ijeme skript, který spustí
E-UAE s urèitými parametry. Skript dostane ikonu "iconx" s obecným
(default tool) nástrojem.


Pøíprava E-UAE
--------------

Program E-UAE pro AROS najde¹ v contrib archivu v noèních buildech a cesta je
*System:Extras/Emu/E-UAE*.

Pro spu¹tìní E-UAE potøebuje¹ soubor ROM. Legální zpùsob jak jej získat je
koupit si Cloanto Amiga Forever nebo Amiga Classix CD-Roms. Nebo jej mù¾e¹
vykopírovat ze skuteèné Amigy pomocí nástroje zvaného "TransRom". Zkopíruj
image soubory nìkam na pevný disk, kde má¹ ulo¾ený AROS. Následující pøíklad
pøedpokládá, ¾e jsi vytvoøil adresáø s názvem "uae" na disk "work:". (Tip:
obrazy (images) z Cloanto CD jsou kryptovány. Bude¹ potøebovat
je¹tì soubor rom.key.)

V editoru uprav stávající konfiguraèní soubor *System:Extras/Emu/E-UAE/.uaerc*
Minimálnì by jsi mìl urèit cesty k ROM obrazùm. Pøíklad::

    amiga.rom_path     = work:uae
    amiga.use_dither   = false
    cpu_type           = 68020
    chipset            = ecs
    chipmem_size       = 4
    cachesize          = 4096
    fastmem_size       = 8
    gfx_linemode       = double
    kickstart_rom_file = $(FILE_PATH)/kick13.rom
    #kickstart_key_file = $(FILE_PATH)/rom.key
    sound_output       = none

A teï si udìláme test. Otevøi shell pro cestu *System:Extras/Emu/E-UAE*
a napi¹ *i386-aros-uae*. Pokud se po chvíli objeví Workbench disketa,
(napø. ruka u Kickstartu 1.3 nebo zají¾dìjící disketa u Kickstartu 3.1)
tak jsi zdolal první pøeká¾ku. I kdy¾ u¾ to takto bì¾í, tak pøesto vìnuj
pozornost chybovým hlá¹kám a sna¾ se je opravit.


Nastavení
---------

E-UAE má hodnì konfigurovatelných mo¾ností, které se ukládají do konfiguraèních
souborù a také má nìkolik mo¾ností v pøíkazové øádce. Podívej se na dokumentaci
pro E-UAE. Kdy¾ spustí¹ E-UAE takto: ``i386-aros-uae -f config1 -option1
-option2``, nejprve se nahraje soubor *.uaerc*. Pak se nahraje soubor
s parametrem -f a pøepí¹e se pøede¹lé nastavení. Poté se pou¾ijí mo¾nosti
dané pøíkazovým øádkem, a znovu se pøepí¹ou mo¾nosti nastavené pøedtím.

Doporuèujeme, aby jsi vytvoøil konfiguraèní soubory, které emulují
skuteèné poèítaèe:

* a500-13.uaerc: 68000 processor, ecs, kick1.3, no acceleration
* a1200-31.uaerc: 68020, aga, kick 3.1, additional memory
* a4000-31.uaerc: no limits

Tady je pøíklad pro *a500-13.uaerc*::

    cpu_type=68000
    cpu_speed=real
    kickstart_rom_file=$(FILE_PATH)/kick13.rom

Psaní konfiguraèních souborù je nejslo¾itìj¹í èást této pøíruèky.
Pokud má¹ Amiga Classix CDRom mù¾e¹ na nìm vyhledat pár rad ohlednì psaní
konfiguraèních souborù. Nebo mù¾e¹ napsat ``i386-aros-uae -h >uaecommands``
a dostane¹ se ke startovnímu bodu se v¹emi mo¾nými volbami.


Instalování aplikací
--------------------

Potøebuje¹ soubory s obrazy (images) diskù aplikací, které bude¹ chtít spustit.
Tyto obrazy mají pøíponu *.adf*. Mù¾e¹ si je ulo¾it na libovolné místo.
Napøíklad my pou¾íváme *work:uae*.


Vytváøení a spou¹tìní skriptù z ikony
-------------------------------------

V dal¹ím kroku si vytvoøí¹ pomocí textového editoru skript, kterým bude¹
spou¹tìt E-UAE. Tento pøíklad u hry Zarathrusta má 2 diskety:


    cd system:emu/e-uae
    i386-aros-uae -f work:uae/a500-13.uaerc -0 work:uae/Zarathrusta1.adf -1 work:uae/Zarathrusta2.adf

První øádek urèuje aktuální adresáø pro E-UAE. Potom spustí¹ E-UAE pomocí
konfiguraèního souboru *a500-13.uaerc* a vlo¾í¹ obraz disku do mechaniky 0 a 1.

Ulo¾í¹ soubor jako *Zarathrusta* ve *work:uae*.

A nakonec pøidá¹ ikonu ke skriptu. Poté co otevøe¹ adresáø *work:uae*
ve Wandereru, klikni na ikonu skriptu a zvol si *Icon/Information*
v menu. Napi¹ *c:iconx* jako default tool. (IconX tool spou¹tí textové soubory
jako DOSové skripty). Na stránce Tooltypes information o ikonì napi¹
*WINDOW=con:0/20//600/Zarathrusta/AUTO*. Toto má za následek vytvoøení
vìt¹ího výstupního okna, tak¾e nebude problém se ètením chybových hlá¹ek.

Dvojklikem na ikonu spustí¹ E-UAE s danou aplikací.


Pevné disky
-----------

E-UAE umo¾òuje pou¾ívat adresáøe hostovaného systému buï jako hardfiles nebo
jako pevné disky. Podrobnìj¹í informace nalezne¹ v souboru
*Extras/Emu/E-UAE/docs/configuration.txt*. Následující pøíklad ti uká¾e,
*jakým zpùsobem mù¾e¹ pou¾ívat adresáøe *work:uae/workbench*
*a *work:uae/programs* jako pevný disk::

    filesystem2=rw,:Workbench:work:uae/workbench,0
    filesystem2=rw,:Programs:work:uae/programs,-1

Na takový disk dokonce mù¾e¹ i nainstalovat AmigaOS a nabootovat z nìj.
Disk, ze kterého by mìl systém bootnout musí mít nejvy¹¹í bootovací prioritu
(je to ten poslední parametr v mo¾nostech filesystem2).


Grafika
-------

Bohu¾el, AROS E-UAE nemá emulaci Picasso, tzn. ¾e jsi omezený na 256
barev na obrazovce.

Nìkolik rad, jak získat lep¹í rozli¹ení a výkon:

+ V konfiguraci pou¾ívej volbu *chipmem_size = 16*. To poskytne 16*512 = 8 MB Chip Ram.
+ Dále pou¾ívej volbu *z3mem_size=x* kde *x* musí být nìco z øady 1,2,4,6,8,16,32.
+ Zvol si High Res Laced v nastavení Screenmodu v Prefs.
+ Pou¾ívej nejvy¹¹í mo¾né nastavení v Overscanu v Prefs.
+ Nainstaluj si nástroj *FBlit*. který vyu¾ívá urèité patche k tomu,
  aby se pou¾ívala Fast Ram namísto Chip RAM.
