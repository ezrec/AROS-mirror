.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <run>`_ `Next <set>`_ 

---------------


======
Search
======

Sk³adnia
~~~~~~~~
::


	Search [FROM] {(name | pattern} [SEARCH] (string | pattern) [ALL] 
		[NONUM] [QUIET] [QUICK] [FILE] [PATTERN] [LINES=Number]


¦cie¿ka
~~~~~~~
::


	Workbench:C


Funkcja
~~~~~~~
::

	Search przeszukuje pliki zawarte w katalogu okre¶lonym w FROM, dla
	okre¶lonego ci±gu (SEARCH); w przypadku gdy opcja ALL jest okre¶lona,
	podkatalogi tak¿e zostaj± przeszukiwane. Nazwy wszystkich plików 
	zawieraj±cych ci±g SEARCH, s± wy¶wietlane, razem z numerami linii,
	gdzie ci±g zosta³ znaleziony.
	Je¶li CTRL-C zosta³ wci¶niêty, przeszukiwanie zostanie przerwane. CTRL-D
	przerwie przeszukiwanie aktualnego pliku.


Parametry
~~~~~~~~~
::


	NONUM	--	numery linii nie zostan± wy¶wietlane
	QUIET	--	nie wy¶wietlaj nazwy przeszukiwanego pliku
	QUICK	--	bardziej ograniczone wyj¶cie
	FILE	--	wyszukuje ci±gu w okre¶lonym pliku
	PATTERN	--	u¿yj wzorców przy wyszukiwaniu
	CASE	--	z uwzglêdnieniem ma³ych i wielkich liter
	LINES	--	dodatkowe linie po pasuj±cym ci±gu


Wynik
~~~~~
::

	Je¶li obiekt jest znaleziony, stan jest zwracany jako 0. W przeciwnym
	wypadku, jest WARN/Ostrze¿enie.

