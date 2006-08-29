.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <stack>`_ `Next <type>`_ 

---------------


======
Status
======

Sk³adnia
~~~~~~~~
::


	PROCESS/N,FULL/S,TCB/S,CLI=ALL/S,COM=COMMAND/K


¦cie¿ka
~~~~~~~
::


	Workbench:c


Funkcja
~~~~~~~
::


	Wy¶wietla informacje o procesach CLI.


Parametry
~~~~~~~~~
::


	PROCESS		--	Numer procesu.

	FULL		--	Wy¶wietla wszystkie dostêpne informacje o 
				procesach.

	TCB		--	Jak dla FULL, ale bez nazw.

	CLI=ALL		--	Domy¶lny. Wy¶wietla wszystkie procesy.

	COM=COMMAND	--	Pokazuje numer procesu, ¿±danej komendy. Okre¶l
				nazwê komendy.


Wynik
~~~~~
::


	Standardowe kody b³êdów.


Przyk³ad
~~~~~~~~~~
::


	Status

	Process  2: Loaded as command: c:status
	Process  3: Loaded as command: c:NewIcons
	Process  4: Loaded as command: GG:Sys/L/fifo-handler
	Process  5: Loaded as command: Workbench
	Process  6: Loaded as command: ToolsDaemon

	Status full

	Process  2: stk 300000, pri   0 Loaded as command: c:status
	Process  3: stk  4096, pri   0 Loaded as command: c:NewIcons
	Process  4: stk  4096, pri   0 Loaded as command: GG:Sys/L/fifo-handler
	Process  5: stk  6000, pri   1 Loaded as command: Workbench
	Process  6: stk  4000, pri   2 Loaded as command: ToolsDaemon


Zobacz tak¿e
~~~~~~~~~~~~
::


	<dos/dosextens.h>


