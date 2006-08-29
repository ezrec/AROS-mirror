.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <beep>`_ `Next <cd>`_ 

---------------


=====
Break
=====

Format
~~~~~~
::

	Break <process> [ALL|C|D|E|F]


Sk³adnia
~~~~~~~~
::

	PROCESS/N,PORT,ALL/S,C/S,D/S,E/S,F/S


¦cie¿ka
~~~~~~~
::

	Workbench:c


Funkcja
~~~~~~~
::

	BREAK wysy³a jeden lub wiêcej sygna³ów do procesu CLI.
	Argument PROCESS okre¶la numeryczn± postaæ programu (ID) CLI,
	do którego chcesz wys³aæ sygna³.
	Komenda STATUS wy¶wietli wszystkie aktualnie uruchomione procesy CLI
	wraz z identyfikatorem ID. Mo¿esz tak¿e podaæ publiczn± nazwê portu
	i wys³aæ sygna³y do tego portu.

	Mo¿esz tak¿e wys³aæ wszystkie sygna³y w tym samym momencie, dziêki
	opcji ALL lub kombinacji znaczników CTRL-C, CTRL-D, CTRL-E i CTRL-F
	przez ich okre¶lone opcje. Tylko gdy proces CLI ma okre¶lone ID wtedy 
	zostanie wys³any sygna³ CTRL-C.

	Efekt u¿ywania komendy BREAK jest ten sam jak wybranie okna
	konsoli i wci¶niêciu odpowiedniej kombinacji.

	Znaczenie klawiszy jest nastêpuj±ce:
	CTRL-C	-	Zatrzymuje proces
	CTRL-D	-	Zatrzymuje skrypt CLI
	CTRL-E	-	Zamyka okno procesu
	CTRL-F	-	Aktywuj okno procesu

	Nie wszystkie programy reaguj± na te sygna³y, ale wiêkszo¶æ
	powinna odpowiadaæ na CTRL-C.



Przyk³ad
~~~~~~~~
::

     
	1.SYS:> BREAK 1

	Wy¶lij sygna³ CTRL-C do procesu oznaczonego jako 1.

	1.SYS:> BREAK 4 E

	Wy¶lij sygna³ CTRL-E do procesu oznaczonego jako 4.


