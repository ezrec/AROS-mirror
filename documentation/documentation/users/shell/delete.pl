.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <date>`_ `Next <dir>`_ 

---------------


======
Delete
======

Sk³adnia
~~~~~~~~
::

 
	Delete { (name | pattern) } [ALL] [QUIET] [FORCE]


¦cie¿ka
~~~~~~~
::


	Workbench/c


Funkcja
~~~~~~~
::

	Usuwa pliki i katalogi. Mo¿esz usuwaæ kilka plików i katalogów, 
	okre¶laj±c je osobno lub poprzez u¿ycie wzorców. Aby anulowaæ
	usuwanie, po prostu wci¶nij CTRL-C. Komenda poinformuje u¿ytkownika
	je¶li ten usuwa pliki z bitami ochronnymi.
	Delete nie mo¿e usuwaæ katalogów, które nie s± puste, chyba, ¿e opcja
	ALL jest za³±czona. Aby powstrzymaæ wy¶wietlanie usuwanych plików nale¿y
	u¿yæ opcji QUIET. Je¶li bit ochronny jest zniesiony dla pliku
	lub katalogu wtedy nie mo¿e zostaæ on usuniêty, ale mo¿na wymusiæ
	usuniêcie dziêki podaniu opcji FORCE.



Parametry
~~~~~~~~~
::


	FILE/M/A  --  pliki lub katalogi do usuniêcia (mog± zawieraæ wzorce)
	ALL/S     --  rekurencyjne usuwanie katalogów
	QUIET/S   --  nie wy¶wietlaj, które obiekty s± usuwane
	FORCE/S   --  usuñ obiekty, nawet wtedy, gdy s± chronione


Przyk³ad
~~~~~~~~
::


	Delete RAM:T/#? ALL FORCE

	Usuwa wszystkie obiekty rekurencyjnie (ALL) z RAM:T/ i nie zwraca uwagi 
	na bity ochrony (FORCE).
