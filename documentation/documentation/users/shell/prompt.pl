.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <path>`_ `Next <protect>`_ 

---------------


======
Prompt
======

Sk³adnia
~~~~~~~~
::


	PROMPT


¦cie¿ka
~~~~~~~
::


	Workbench:c


Funkcja
~~~~~~~
::


	Okre¶la liniê komend dla konkretnego CLI.


Parametry
~~~~~~~~~
::


		PROMPT	--	Ci±g którym okre¶la siê wygl±d CLI, oto
				przy³adowe argumenty:

		N	--	numer cli
		S	--	nazwa aktualnego katalogu
		R	--	kod b³êdu

	Je¶li PROMPT nie jest okre¶lone to "%N.%S> " jest u¿ywane za domy¶lne.


Wynik
~~~~~
::


	Standardowe kody b³êdów.


Przyk³ad
~~~~~~~~
::


	Prompt "Tu Camelek!.%N> "	daje:

	Tu Camelek!.10>			(je¶li numer CLI by³ 10)


Zobacz tak¿e
~~~~~~~~~~~~
::


	SetPrompt()


