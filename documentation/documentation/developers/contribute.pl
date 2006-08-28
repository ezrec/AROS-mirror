==========
Contribute
==========

:Authors:   Adam Chodorowski 
:Copyright: Copyright (C) 1995-2002, The AROS Development Team
:Version:   $Revision: 24047 $
:Date:      $Date: 2006-02-02 22:20:47 +0100 (Cz, 02 lut 2006) $
:Status:    Done. 

.. Contents::

.. Include:: contribute-abstract.en


Dostêpne zdania
===============

To jest lista zadañ, przy których potrzebujemy pomocy i nad którymi nikt 
aktualnie nie pracuje. To nie jest pe³na lista, zawiera jednak najwa¿niejsze
rzeczy, przy których potrzebna jest pomoc.


Programowanie
-------------

+ Implementacja brakuj±cych bibliotek, zasobów, urz±dzeñ lub ich czê¶ci. 
  Obejrzyj szczególowy raport o statusie, by uzyskaæ wiêcej informacji czego brakuje.

+ Implementacja lub poprawienie sterowników sprzêtu:
  
  - AROS/m68k-pp:
    
    + Grafika
    + Urz±dzenia wej¶cia (touchscreen, buttons)
    + D¼wiêk
 
  - AROS/i386-pc:
    
    + specyficzne sterowniki kart graficznych (mamy tylko ogólne, niezbyt
      dobrze akcelerowane). Krótka lista ¿yczeñ:
      
      - nVidia TNT/TNT2/GeForce (rozpoczêta, lecz niekompletna) 
      - S3 Virge
      - Matrox Millenium
    
    + USB
    + SCSI
    + specyficzne chipsety IDE
    + ...Co¶ jeszcze, co Ci przychodzi na my¶l.

  - Ogólna obs³uga drukarki.
 

+ Portowanie na inne architektury. Kilka przyk³adów sprzêtu, na który nie 
  ma jeszcze portu AROSa lub prace dopiero siê rozpoczê³y:

  - Amiga m68k i PPC.
  - Atari.
  - HP 300 series.
  - SUN Sparc.
  - iPaq.
  - Macintosh m68k i PPC.

+ Implementacja brakuj±cych edytorów preferencji:

  - IControl
  - Overscan
  - Palette
  - Pointer
  - Printer
  - ScreenMode
  - Sound
  - WBPattern
  - Workbench 
 
+ Poprawienie biblioteki C link 

  Implementacja brakuj±cych funkcji ANSI (i kilku POSIX) w clib, by u³atwiæ
  portowanie programów UNIXowych (np. GCC, make i binutils). Najwiêksz± 
  brakuj±c± rzecz± jest wsparcie dla POSIX style signaling, lecz jest tak¿e
  kilka innych funkcji.

+ Implementacja wiêkszej ilo¶ci datatypów i poprawienie istniej±cych

  Liczba dostêpnych w systemie AROS datatypów jest niewielka. Poni¿ej kilka 
  przyk³adów datatypów, które wymagaj± poprawienia by by³y u¿ywalne lub 
  musz± byæ napisane:

  - amigaguide.datatype
  - sound.datatype
    
    + 8svx.datatype

  - animation.datatype
    
    + anim.datatype
    + cdxl.datatype
    
  
+ Portowanie programów:

  - Wdytory tekstu jak ViM i Emacs.
  - Lañcuch narzêdzi developerskich, zawieraj±cy GCC, make, binutils i inne
    narzêdzia programistyczne GNU.
  

Dokumentacja
-------------

+ Pisanie dokumentacji u¿ytkownika. Polega to na tworzeniu ogólnej Instrukcji 
  U¿ytkownika dla nowicjuszy i ekspertów, a tak¿e dokumentacji referencyjnej 
  dla wszystkich standardowych programów AROSa.

+ Pisanie dokumentacji programisty. Chocia¿ jest to w nieco lepszym stanie
  ni¿ dokumentacja u¿ytkownika, nadal jest du¿o do zrobienia. Na przyk³ad,
  nie ma jeszcze dobrego tutoriala dla pocz±tkuj±cych programistów. 
  Odpowiednik 'ROM Kernel Manuals for AROS' by³by naprawdê potrzebny.


T³umaczenie
-----------

+ T³umaczenie AROSa na inne jêzyki. Obecnie w mniejszym lub wiêkszym stopniu
  obs³ugiwane s± nastêpuj±ce jêzyki:

  - angielski
  - niemiecki
  - szwedzki
  - norweski
  - w³oski


+ T³umaczenie dokumentacji i strony internetowej na inne jêzyki. Obecnie 
  kompletna strona jest dostêpna jedynie po angielsku. Czê¶æ zosta³a 
  przet³umaczona na norweski, niemiecki, rosyjski i w³oski, lecz nadal jest 
  du¿o do zrobienia.


Inne
-----

+ Kierowanie projektami GUI dla programów AROSa, takich jak prefs,
  tools i utilities.

Do³±czanie do Zespo³u
=====================

Chcesz siê przy³±czyæ do developerów? Wspaniale! Do³±cz zatem do `listy 
dyskusyjnej`__, która Ciê interesuje (przynajmniej zapisanie siê na 
g³ówn± listê aros-dev jest *wysoce* wskazane) i popro¶ o dostêp do
repozytorium SVN.
To wszystko. :)

Napisanie krótkiego maila na listê dyskusyjn± zawieraj±c± informacje o sobie, 
o tym w czym chce siê pomóc jest mile widziane. Je¶li masz jakie¶ problemy, 
nie krêpuj siê, wy¶lij maila na listê lub spytaj na `kanale IRC`__.
Zatem zanim rozpoczniesz pracowaæ nad czym¶ konkretnym, napisz proszê maila na 
listê, informuj±c co chcesz robiæ lub popraw bazê zadañ. Tym sposobem 
uchronisz innych przed prac± nad tym samym przez pomy³kê...

__ ../../contact#mailing-lists
__ ../../contact#irc-channels


Repozytorium SVN
----------------

Repozytorium AROSa pracuje na chronioym has³em serwerze subwersji, co oznacza,
¿e musisz poprosiæ o dostêp do niego, by wspó³pracowaæ w rozwoju. Has³a s± 
zaszyfrowane, mo¿esz je wygenerowaæ naszym `narzêdziem szyfruj±cym`__.

Napisz maila ze swoim zaszyfrowanym has³em razem z wybran± nazw± u¿ytkownika
i nazwiskiem do `Aaron Digulla`__ i czekaj na odpowied¼. By przy¶pieszyæ odpowied¼,
wpisz w temacie maila "Access to the AROS SVN server" a w tre¶ci "Please add 
<u¿ytkownik> <has³o>", np.::

    Please add digulla xx1LtbDbOY4/E

To mo¿e potrwaæ kilka dni, bo Aaron jest bardzo zajêty, proszê b±d¼ cierpliwy. 

Aby uzyskaæ informacje jak u¿ywaæ serwera SVN, przeczytaj proszê "`Praca z SVN`__".
Nawet je¶li ju¿ wiesz jak u¿ywaæ SVN to warto tam zajrzeæ, poniewa¿ znajdziesz
tam informacje i porady specyficzne dla repozytorium AROSa (takie jak siê do 
niego zalogowaæ).

__ http://aros.sourceforge.net/tools/password.html 
__ mailto:digulla@aros.org?subject=[Access%20to%20the%20AROS%20SVN%20server]
__ svn
 
