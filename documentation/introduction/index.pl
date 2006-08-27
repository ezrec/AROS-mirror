=============================
Krótkie wprowadzenie do AROSa
=============================

:Autorzy:   Aaron Digulla, Stefan Rieken, Matt Parsons, Adam Chodorowski 
:Prawa autorskie: Copyright © 1995-2002, The AROS Development Team
:Wersja:   $Revision: 24430 $
:Data:      $Date: 2006-05-08 04:01:22 +0500 (ÐŸÐ½, 08 Ð¼Ð°Ð¹ 2006) $
:Status:    Prawie skoñczony, jak s¹dzê...


.. Include:: index-abstract.pl


Cele
====

Celem projektu AROS jest stworzenie systemu operacyjnego, który:

1. Jest najbardziej jak to mo¿liwe kompatybilny z AmigaOS 3.1.

2. Mo¿e byæ portowany na ró¿ne architektury sprzêtowe i procesory, 
   takie jak x86, PowerPC, Alpha, Sparc, HPPA i inne.

3. Powinien byæ kompatybilny na poziomie binariów z Amig¹ i na poziomie Ÿróde³
   kompatybilny z ka¿dym innym sprzêtem.
  
4. Mo¿e dzia³aæ jako samodzielny system, uruchamiaj¹cy siê bezpoœrednio z dysku twardego
   i jako emulacja, otwieraj¹ca okno w systemie gospodarza by umo¿liwiæ programowanie i
   uruchamianie natywnych amigowych programów w tym samym czasie.

5. Ma funkcjonalnoœæ ulepszon¹ w stosunku do AmigaOS.

By osi¹gn¹æ te cele, zastosowaliœmy szereg technik. Przede wszystkim intensywnie wykorzystujemy 
Internet. Mo¿esz wzi¹æ udzia³ w naszym projekcie nawet jeœli chcesz napisaæ jedn¹ 
funkcjê systemu operacyjnego. Najbardziej aktualna wersja Ÿróde³ jest dostêpna 24 godziny na dobê
a zmiany w nich mog¹ byæ wprowadzane w dowolnym czasie.  Niewielka baza danych z otwartymi zadaniami 
pozwala unikn¹æ dublowania pracy.


Historia
========

Jakiœ czas temu w roku 1993, sytuacja Amigi pogorszy³a siê i niektórzy z jej
fanów zaczêli siê zastanawiaæ nad tym, co nale¿a³oby zrobiæ, by podnieœæ presti¿ 
ich ukochanego komputera. Nagle g³ówny powód braku sukcesu Amigi sta³ siê jasny: 
to by³o rozpowszechnienie a w³aœciwie jego brak. Amiga powinna byæ bardziej powszechna 
by staæ siê bardziej atrakcyjn¹ dla u¿ytkowników i developerów. Zaplanowano osi¹gniêcie 
tego celu. Jednym z planów by³o naprawienie bugów AmigaOS, innym stworzenie z niego 
nowoczesnego systemu operacyjnego. Narodzi³ siê projekt AOS.

Lecz w³aœciwie co by³o bugiem? I jak nale¿y te bugi naprawiæ? Jakie cechy powinien 
posiadaæ tak zwany *nowoczesny* system operacyjny? I jak powinny byæ one 
zaimplementowane do AmigaOS?

Dwa lata póŸniej, ludzie nadal spierali siê w tym temacie i nie napisano ani  
jednej linii kodu (w ka¿dym razie nikt nie zobaczy³ tego kodu). Dyskusje 
zaczyna³y siê od "musimy mieæ ..." nastêpnie ktoœ odpowiada³ "przeczytaj stare maile" 
lub "to jest niemo¿liwe do zrobienia poniewa¿ ..." po czym nastêpowa³o "mylisz siê bo ..." 
itp. 

Zim¹ 1995 roku, Aaron Digulla mia³ ju¿ doœæ tej sytuacji i wys³a³ RFC (request for comments) 
na listê dyskusyjn¹ AOS, w którym zapyta³ jakie powinny byæ wymagania minimalne. 
Zaproponowano szereg wariantów i w rezultacie okaza³o siê, ¿ê praktycznie wszyscy
chcieliby zobaczyæ otwarty system operacyjny, kompatybilny z AmigaOS 3.1 (Kickstart 40.68). 
Na tej bazie mia³y siê opieraæ wszystkie dalsze dyskusje by ustaliæ co jest mo¿liwe a co nie.


Tak rozpoczê³y siê prace i narodzi³ siê AROS.

