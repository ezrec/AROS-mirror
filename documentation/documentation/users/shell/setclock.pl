.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <set>`_ `Next <setdefaultfont>`_ 

---------------


========
SetClock
========

Format
~~~~~~
::

	SetClock {LOAD|SAVE|RESET}


Sk³adnia
~~~~~~~~
::

	LOAD/S,SAVE/S,RESET/S


¦cie¿ka
~~~~~~~
::

	C:SetClock


Funkcja
~~~~~~~
::

	SetClock mo¿e byæ wykorzystywany do:
		o	Wczytywania czasu z zegara sprzêtowego, podtrzymywanego
			bateri±
		o	Zapisywania czasu z zegara sprzêtowego, podtrzymywanego
			bateri±
		o	Resetowania czasu z zegara sprzêtowego, podtrzymywanego
			bateri±


Przyk³ad
~~~~~~~~
::


	SetClock LOAD
	
	Ustawia czas ze sprzêtowego zegara. W wiêkszo¶ci przypadków jest to 
	robione automatycznie, wiêc nie ma potrzeby uruchamiaæ tego rêcznie,
	przydaje siê to tylko wtedy, gdy zegar, jest w rozszerzeniu, które
	nie za³atwia tego samodzielnie.
	

	SetClock SAVE

	Ustawia czas z programowego zegara. Zapisuje czas do sprzêtowego zegara.


	SetClock RESET
	
	Resetuje datê do pierwszego stycznia tysi±c dziewiêæset 
	siedemdziesi±tego ósmego roku, a czas do godziny zero. Najczê¶ciej
	wykorzystuje siê t± opcjê do naprawy zegara sprzêtowego, gdy zapis i 
	odczyt nie dzia³a prawid³owo.


Zobacz tak¿e
~~~~~~~~~~~~
::

	C:Date, Prefs/Time


