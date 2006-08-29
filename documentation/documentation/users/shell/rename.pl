.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <relabel>`_ `Next <requestchoice>`_ 

---------------


======
Rename
======

Sk³adnia
~~~~~~~~
::


	Rename [{FROM}] <nazwa> [TO|AS] <nazwa> [QUIET]

	FROM/A/M,TO=AS/A,QUIET/S


¦cie¿ka
~~~~~~~
::


	Workbench/c


Funkcja
~~~~~~~
::

	Zmienia nazwê obiektu. Rename mo¿e byæ u¿yty tak jak UNIX'owy mv,
	który przenosi plik/pliki do innej lokacji na dysku.


Parametry
~~~~~~~~~
::


	FROM	--	Nazwa(y) pliku(ów) do przeniesienia. Mo¿e byæ podanych 
			wiele plików.


	TO|AS	--	Nazwa jak± chcemy przypisaæ nowemu plikowi.

	QUIET	--	Brak wyj¶cia do konsoli.


Wynik
~~~~~
::


	Standardowe kody b³êdów.


Przyk³ad
~~~~~~~~
::


	Rename list1.doc list2.doc listy
	
	Przenosi list1.doc i list2.doc do katalogu listy.
     

	Rename ram:a ram:b quiet
	Rename from ram:a to ram:b quiet
	Rename from=ram:a to=ram:b quiet

	Wszystkie wersje, zmiana nazwy z "a" do "b", i brak jest wyj¶cia do
	konsoli(QUIET).

