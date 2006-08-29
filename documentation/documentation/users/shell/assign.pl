.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <ask>`_ `Next <avail>`_ 

---------------


======
Assign
======

Sk³adnia
~~~~~~~~
::


	NAME, TARGET/M, LIST/S, EXISTS/S, DISMOUNT/S, DEFER/S, PATH/S, ADD/S,
	REMOVE/S, VOLS/S, DIRS/S, DEVICES/S


¦cie¿ka
~~~~~~~
::


	Workbench:C


Funkcja
~~~~~~~
::
	ASSIGN tworzy odno¶nik do pliku lub katalogu. Odno¶nik jest
	logiczn± nazw± dysku, co sprawia, ¿e bardzo wygodnie mo¿na
	przypisaæ obiekty u¿ywaj±c przypisów zamiast ¶cie¿ek do tych
	katalogów.
	
	Je¶li argumenty NAME i TARGET s± podane, ASSIGN przypisze
	logiczn± nazwê do okre¶lonej ¶cie¿ki. Je¶li NAME jest ju¿ przypisane
	do pliku lub katalogu to nowy przypis zmieni poprzedni± ¶cie¿kê.
	Dwukropek musi byæ podany po argumencie NAME.

	Je¶li tylko argument NAME jest podany, to ka¿de przypisanie do
	tego NAME jest usuwane. Je¶li brak jest argumentów, to zostan±
	wy¶wietlone wszystkie przypisy.
	


Parametry
~~~~~~~~~
::


	NAME		--	nazwa, która powinna byæ przypisana do pliku lub katalogu
	TARGET		--	jeden lub wiêcej katalogów podanych jako ¶cie¿ka
	LIST		--	lista wszystkich stworzonych przypisów
	EXISTS		--	je¶li nazwa NAME ju¿ wystêpuje, to zostanie zwrócony kod WARN
	DISMOUNT	--	usuñ wolumin lub nazwê urz±dzenia NAME z dos listy
	DEFER		--	stwórz przypis do nieistniej±cej (w chwili tworzenia) ¶cie¿ki
	PATH		--	stwórz przypis do nieistniej±cej (w chwili tworzenia) ¶cie¿ki	
	ADD		--	dodaje kolejn± ¶cie¿kê do nazwy przypisu NAME
	REMOVE		--	usuwa przypis
	VOLS		--	wy¶wietla przypisane woluminy
	DIRS		--	wy¶wietla przypisane katalogi
	DEVICES		--	wy¶wietla przypisane urz±dzenia
     


