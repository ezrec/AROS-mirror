.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <echo>`_ `Next <endcli>`_ 

---------------


====
Else
====

¦cie¿ka
~~~~~~~
::


	Workbench:C


Funkcja
~~~~~~~
::

	Oddziela 'true' i 'false' z komendy If. Wszystko po komendzie Else
	jest wykonywane tylko wtedy, gdy gdy niespe³niony zosta³ warunek.



Przyk³ad
~~~~~~~~
::


     If EXISTS Sys:Devs
         Copy random.device Sys:Devs/
     Else
         Echo "Brak katalogu Sys:Devs"
     EndIf


Zobacz tak¿e
~~~~~~~~~~~~
::


     If, EndIf


