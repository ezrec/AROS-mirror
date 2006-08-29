.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <setdefaultfont>`_ `Next <setkeyboard>`_ 

---------------


======
Setenv
======

Sk³adnia
~~~~~~~~
::


	NAME,SAVE/S,STRING/F


¦cie¿ka
~~~~~~~
::


	Workbench:c


Funkcja
~~~~~~~
::


	Ustawia globaln± warto¶æ z aktualnego Shell. Te warto¶ci mog± byæ 
	osi±galne z ka¿dego programu.
	
	Te warto¶ci nie s± zapisywane w ENVARC:, w zwi±zku z tym zostaj± one
	zachowywane na konkretn± sesjê systemu operacyjnego, po ponownym 
	uruchomieniu s± wymazywane. Gdy u¿yta zostaje opcja SAVE, wtedy warto¶æ
	zostaje zapisywana tak¿e w ENVARC:
	
	Je¶li brak parametrów, aktualna lista parametrów globalnych zostaje 
	wy¶wietlona.


Parametry
~~~~~~~~~
::


	NAME	-	Nazwa globalnej zmiennej.

	SAVE	-	Zapisz warto¶æ w ENVARC:

	STRING	-	Warto¶æ dla globalnej zmiennej NAME.


Wynik
~~~~~
::

	Standardowe kody b³êdów.


Przyk³ad
~~~~~~~~
::


	Setenv EDITOR Ed
	
	Ka¿dy program korzystaj±cy z warto¶ci EDITOR, bêdzie móg³ pobraæ nazwê
	edytora tekstowego, którego u¿ytkownik bêdzie chcia³ u¿ywaæ.


Zobacz tak¿e
~~~~~~~~~~~~
::


	Getenv, Unsetenv


