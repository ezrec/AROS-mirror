=============================
Krótkie wprowadzenie do AROSa
=============================

:Autorzy:   Aaron Digulla, Stefan Rieken, Matt Parsons, Adam Chodorowski 
:Prawa autorskie: Copyright (C) 1995-2002, The AROS Development Team
:Wersja:   $Revision$
:Data:      $Date$
:Status:    Prawie skoñczony, jak s±dzê...


.. Include:: index-abstract.pl


Cele
====

Celem projektu AROS jest stworzenie systemu operacyjnego, który:

1. Jest najbardziej jak to mo¿liwe kompatybilny z AmigaOS 3.1.

2. Mo¿e byæ portowany na ró¿ne architektury sprzêtowe i procesory, 
   takie jak x86, PowerPC, Alpha, Sparc, HPPA i inne.

3. Powinien byæ kompatybilny na poziomie binariów z Amig± i na poziomie ¼róde³
   kompatybilny z ka¿dym innym sprzêtem.
  
4. Mo¿e dzia³aæ jako samodzielny system, uruchamiaj±cy siê bezpo¶rednio z dysku twardego
   i jako emulacja, otwieraj±ca okno w systemie gospodarza by umo¿liwiæ programowanie i
   uruchamianie natywnych amigowych programów w tym samym czasie.

5. Ma funkcjonalno¶æ ulepszon± w stosunku do AmigaOS.

By osi±gn±æ te cele, zastosowali¶my szereg technik. Przede wszystkim intensywnie wykorzystujemy 
Internet. Mo¿esz wzi±æ udzia³ w naszym projekcie nawet je¶li chcesz napisaæ jedn± 
funkcjê systemu operacyjnego. Najbardziej aktualna wersja ¼róde³ jest dostêpna 24 godziny na dobê
a zmiany w nich mog± byæ wprowadzane w dowolnym czasie.  Niewielka baza danych z otwartymi zadaniami 
pozwala unikn±æ dublowania pracy.


Historia
========

Jaki¶ czas temu w roku 1993, sytuacja Amigi pogorszy³a siê i niektórzy z jej
fanów zaczêli siê zastanawiaæ nad tym, co nale¿a³oby zrobiæ, by podnie¶æ presti¿ 
ich ukochanego komputera. Nagle g³ówny powód braku sukcesu Amigi sta³ siê jasny: 
to by³o rozpowszechnienie a w³a¶ciwie jego brak. Amiga powinna byæ bardziej powszechna 
by staæ siê bardziej atrakcyjn± dla u¿ytkowników i developerów. Zaplanowano osi±gniêcie 
tego celu. Jednym z planów by³o naprawienie bugów AmigaOS, innym stworzenie z niego 
nowoczesnego systemu operacyjnego. Narodzi³ siê projekt AOS.

Lecz w³a¶ciwie co by³o bugiem? I jak nale¿y te bugi naprawiæ? Jakie cechy powinien 
posiadaæ tak zwany *nowoczesny* system operacyjny? I jak powinny byæ one 
zaimplementowane do AmigaOS?

Dwa lata pó¼niej, ludzie nadal spierali siê w tym temacie i nie napisano ani  
jednej linii kodu (w ka¿dym razie nikt nie zobaczy³ tego kodu). Dyskusje 
zaczyna³y siê od "musimy mieæ ..." nastêpnie kto¶ odpowiada³ "przeczytaj stare maile" 
lub "to jest niemo¿liwe do zrobienia poniewa¿ ..." po czym nastêpowa³o "mylisz siê bo ..." 
itp. 

Zim± 1995 roku, Aaron Digulla mia³ ju¿ do¶æ tej sytuacji i wys³a³ RFC (request for comments) 
na listê dyskusyjn± AOS, w którym zapyta³ jakie powinny byæ wymagania minimalne. 
Zaproponowano szereg wariantów i w rezultacie okaza³o siê, ¿ê praktycznie wszyscy
chcieliby zobaczyæ otwarty system operacyjny, kompatybilny z AmigaOS 3.1 (Kickstart 40.68). 
Na tej bazie mia³y siê opieraæ wszystkie dalsze dyskusje by ustaliæ co jest mo¿liwe a co nie.


Tak rozpoczê³y siê prace i narodzi³ siê AROS.

