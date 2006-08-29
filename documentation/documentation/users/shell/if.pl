.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <getenv>`_ `Next <info>`_ 

---------------


==
If
==

Sk³adnia
~~~~~~~~
::


	NOT/S,WARN/S,ERROR/S,FAIL/S,,EQ/K,GT/K,GE/K,VAL/S,EXISTS/K


¦cie¿ka
~~~~~~~
::


	Workbench:C


Funkcja
~~~~~~~
::

	Uruchamia sekwencjê komend wydanych po sobie, je¶li warto¶æ jest true.
	(sekwencja nastêpuj±ca po If to linie komend zakoñczone Else lub EndIf).
	Dla ka¿dej komendy If musi byæ EndIf. Je¶li jednak stan jest false to
	wtedy wykonywanie przeskoczy do Else lub EndIf.


Parametry
~~~~~~~~~
::


	NOT		--	neguje warto¶æ stanu

	WARN		--	True je¶li warto¶æ jest wiêksza b±d¼ równa 5.
	ERROR		--	True je¶li warto¶æ jest wiêksza b±d¼ równa 10.
	FAIL		--	True je¶li warto¶æ jest wiêksza b±d¼ równa 20.

	EQ, GE, GT	--	True je¶li pierwsza warto¶æ jest równa, wiêksza
				lub równa kolejno wiêksza od drugiej.

	VAL		--	Wskazuje, ¿e porównanie powinno zmieniaæ ci±gi
				w warto¶ci numeryczne.

	EXISTS <ci±g>	--	True je¶li obiekt <ci±g> wystêpuje.



Przyk³ad
~~~~~~~~
::


     If 500 GT 200 VAL
         echo "500 to wiêcej ni¿ 200"
     Else
         If EXISTS S:User-Startup
             echo "Skrypt User-Startup znaleziony w S:"
             Execute S:User-Startup
         EndIf
     EndIf


Opis
~~~~
::

	ERROR oraz FAIL bêdzie odpowiednie dopiero wtedy je¶li poziom b³êdu
	jest ustawiony przez FailAt (normalnie stopieñ ten wynosi 10, i je¶li
	jaki¶ b³±d przekroczy lub bêdzie równy tej warto¶ci, to skrypt zostanie
	przerwany).


Zobacz tak¿e
~~~~~~~~~~~~
::


     Else, EndIf, FailAt


