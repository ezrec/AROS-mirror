==========
Contribute
==========

:Authors:   Adam Chodorowski 
:Copyright: Copyright © 1995-2007, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Done. 

.. Contents::

.. Include:: contribute-abstract.cs


Aktuální úkoly
==============

Zde je seznam nìkterých úkolù, se kterými potøebujeme pomoci, a na kterých
momentálnì nikdo nedìlá. Nejedná se o kompletní seznam v¹ech úkolù, ale
obsahuje ty úkoly, které pova¾ujeme za dùle¾ité.


Programování
------------

+ Implementace chybìjících knihoven, zdrojù, zaøízení nebo jejich souèástí.
  Podívej se na detailní zprávu o vývoji, aby jsi zjistil více informací
  o èástech, které chybí.


+ Implementace nebo zdokonalení ovladaèù hardware zaøízení:
  
  - AROS/m68k-pp:
    
    + Grafika
    + Vstup (dotyková obrazovka, tlaèítka)
    + Zvuk
 
  - AROS/i386-pc:

    + Konkrétní ovladaèe grafických karet (máme pouze obecné, ne zrovna
      pøíli¹ akcelerované). Krátký seznam pøání:
      
      - nVidia TNT/TNT2/GeForce (zaèlo se, ale nedokonèilo) 
      - S3 Virge
      - Matrox Millenium
    
    + chybí USB classes
    + SCSI
    + Konkrétní IDE èipsety
    + Zvuk
    + ... cokoliv, na co bys mohl pøijít.

  - V¹eobecná podpora tiskáren.
 
+ Portování na dal¹í architektury. Nìkolik pøíkladù hardwaru, pro
  který AROS je¹tì nemá port nebo zaèal být portován:

  - Amiga, m68k i PPC.
  - Atari.
  - HP 300 series.
  - SUN Sparc.
  - iPaq.
  - Macintosh, m68k i PPC.

+ Implementace chybìjících editorù nastavení:

  - Overscan
  - Paleta
  - Pointer
  - Tiskárna
 
+ Zdokonalování knihovny C link

  Tedy implementace chybìjících ANSI (a nìkterých POSIX) funkcí v clib,
  aby se usnadnilo portování UNIX softwaru (napø. GCC, make a binutils).
  Nejdùle¾itìj¹í vìc, která chybí, je podpora POSIX style signaling,
  ale i jiné funkce.

+ Implementace více datatypù a vylep¹ení tìch stávajících

  Poèet datatypù obsa¾ených v AROSu je pomìrnì malý. Nìkteré datatypy,
  které potøebují vylep¹ení, aby byly pou¾itelné nebo potøebují
  implementaci úplnì od základu:

  - amigaguide.datatype
  - sound.datatype
    
    + 8svx.datatype

  - animation.datatype
    
    + anim.datatype
    + cdxl.datatype
    
  
+ Portování programù tøetích stran:

  - Textové editory jako jsou ViM a Emacs.
  - Balík vývojových nástrojù, které zahrnují GCC, make, binutils a dal¹í
    GNU vývojáøské nástroje.
  - AmigaOS Open Source software jako je SimpleMail, YAM, Jabbwerwocky


Dokumentace
-----------

+ Psaní u¾ivatelské dokumentace. Týká se psaní hlavních U¾ivatelských
  pøíruèek pro zaèáteèníky a experty, a také dokumentace na v¹echny
  standardní AROS programy.

+ Psaní vývojáøské dokumentace. Tøeba¾e je tato práce v pokroèilej¹ím
  stádiu ne¾ u¾ivatelská dokumentace, stále je toho je¹tì hodnì zpracovávat.
  Pro pøíklad, zatím nemáme ¾ádnou dobrou pøíruèku pro zaèínající programátory.
  Bylo by taky hezké mít ekvivalent k ROM Kernel manuálùm pro AROS.


Pøeklad
-------

+ Pøekládání samotného AROSu do více jazykù. Nyní jsou více èi ménì kompletnì
  podporovány pouze tyto jazyky:

  - Angliètina
  - Nìmèina
  - ©véd¹tina
  - Nor¹tina
  - Ital¹tina
  - Francouz¹tina
  - Ru¹tina

+ Pøeklad dokumentace a internetových stránek do více jazykù. Aktuálnì je
  web kompletní pouze v angliètinì. Pøekládá se pozvolna i do dal¹ích jazykù,
  ale zbývá je¹tì hodnì práce.


Ostatní
-------

+ Koordinace GUI designu pro AROS programy, jako jsou prefs programy,
  nástroje a utility.


Pøipoj se k týmu
================

Chce¹ se pøipojit k vývojáøskému týmu? Skvìle! V tom pøípadì se pøihla¹ k
`vývojáøskému mailing listu`__, o který se zajímá¹ (*vøele* doporuèujeme pøipojit
se alespoò k hlavnímu vývojáøskému listu) a po¾ádej o pøístup k Subversion
repozitáøi. A je to. :)

Napi¹ krátký mejlík na vývojáøský list, nìco o sobì, co dìlá¹ a jakým zpùsobem
bys rád pomohl. Pokud bude¹ mít nìjaký problém, neváhej poslat email do listu
nebo se zeptej na nìkterém z `IRC kanálù`__. Také, ne¾ zaène¹ dìlat na nìèem
konkrétním, napi¹ email do listu s tím, co bude¹ dìlat, nebo aktualizuj
databázi úkolù. Takto se dá pøedejít tomu, aby nìkolik lidí dìlalo
nedopatøením na jednom a tom samém úkolu...


__ ../../contact#mailing-lists
__ ../../contact#irc-channels


Subversion repozitáø
--------------------

AROS repozitáø bì¾í na Subversion serveru, který je chránìný heslem,
co¾ znamená, ¾e musí¹ po¾ádat o pøístup, aby jsi se mohl podílet na vývoji.
Hesla jsou kryptována, kryptované heslo si mù¾e¹ vytvoøit pomocí na¹eho
`online kryptovacího nástroje hesla`__.

Po¹li prosím kryptované heslo spoleènì s tvým vlastním u¾ivatelským jménem
a skuteèným jménem na adresu `Aarona Digully`__ a poèkej na odezvu. Aby jsi
urychlil proces pøístupu, napi¹ do pøedmìtu zprávy toto "Access to the
AROS SVN server" a do tìla napi¹ "Please add <username> <password>", napø.::

    Please add digulla xx1LtbDbOY4/E

Mù¾e to trvat nìkolik dní, jeliko¾ Aaron je hodnì vytí¾ený, tak¾e mìj trpìlivost. 

Abys vìdìl, jak pou¾ívat AROS SVN server, pøeèti si prosím "`Práce s
SVN`__". I pokud u¾ ví¹ jak pou¾ívat SVN, je stále u¾iteèné jej prostudovat,
proto¾e obsahuje informace a rady urèené pro AROS repozitáø
(napø. jak se do nìj zalogovat).

__ http://aros.sourceforge.net/tools/password.html 
__ mailto:digulla@aros.org?subject=[Access%20to%20the%20AROS%20SVN%20server]
__ svn
 
