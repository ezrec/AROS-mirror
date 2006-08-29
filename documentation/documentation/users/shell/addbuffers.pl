.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Next <alias>`_ 

---------------


==========
AddBuffers
==========

Sk³adnia
~~~~~~~~
::


	DRIVE/A, BUFFERS/N


¦cie¿ka
~~~~~~~
::


	Workbench:C


Funkcja
~~~~~~~
::

	Dodaje bufory do listy dostêpnych buforów dla okre¶lonego
	dysku. Dodawanie buforów przy¶piesza dostêp do dysku, ale
	zwiêksza ilo¶æ zajêtej pamiêci systemowej (512 bajtów na
	bufor). Okre¶laj±c negatywn± liczbê buforów zmniejsza siê
	liczbê buforów dysku.
		Je¶li podany jest tylko argument DRIVE, to zostanie
	podana liczba buforów dysku, bez zmiany jego warto¶ci.



Parametry
~~~~~~~~~
::


	DRIVE	--	parametr okre¶laj±cy nazwê dysku
	BUFFERS	--	liczba dodawanych buforów (lub odejmowanych
			w przypadku ujemnej warto¶ci) dysku

