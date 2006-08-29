.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <rename>`_ `Next <requestfile>`_ 

---------------


=============
RequestChoice
=============

Sk³adnia
~~~~~~~~
::


	TITLE/A,BODY/A,GADGETS/A/M,PUBSCREEN/K


¦cie¿ka
~~~~~~~
::


	Workbench:c


Funkcja
~~~~~~~
::

	Pozwala na u¿ycie EasyRequest() w skryptach AmigaDOS.


Parametry
~~~~~~~~~
::


	TITLE		- Tytu³ okna zapytania.

	BODY		- Tekst wy¶wietlany w oknie zapytania.

	GADGETS		- Tekst dla przycisków.

	PUBSCREEN	- Nazwa ekranu publicznego na którym siê uruchomi.


Wynik
~~~~~
::


	Standardowe kody b³êdu.


Przyk³ad
~~~~~~~~
::

	RequestChoice "To jest tytu³" "To jest*Nopis" Dobra|Poniechaj
	
	Tutaj wszystko mówi za siebie, oprócz "*N". To jest odpowiednik
	'\n' w jêzyku C, ¿eby wstawiæ now± liniê. Okno zapytania zostanie
	otworzone w Workbench Screen.


	RequestChoice Title="Kolejny tytu³" Body="A to jest*Nkolejny opis"
	Gadgets=Dobra|Poniechaj PubScreen=DOPUS.1

	To robi dok³adnie to samo, ale na ekranie publicznym Directory Opus.


Opis
~~~~
::

	Aby umie¶ciæ now± liniê nale¿y w opisie wstawiæ *n lub *N .
	
	Aby umie¶cic cudzys³ów nale¿y u¿yæ *" .
	
	Szablon CLI daje GADGETS opcje jako ALWAYS; jest to odmienne od 
	orginalnego programu. Dziêki temu nie musimy sprawdzaæ, czy gad¿et 
	zosta³ podany.


Zobacz tak¿e
~~~~~~~~~~~~
::


	intuition.library/EasyRequest(), intuition.library/EasyRequestArgs()


