.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <execute>`_ `Next <fault>`_ 

---------------


======
FailAt
======

Format
~~~~~~
::

	FailAt <limit>


Sk³adnia
~~~~~~~~
::

	RCLIM/N


¦cie¿ka
~~~~~~~
::

	C:


Funkcja
~~~~~~~
::

	FailAt ustawia limit kodu b³êdu w skrypcie. Je¶li jaka¶ komenda zwróci
	kod b³êdu wiêkszy b±d¼ równy tej warto¶ci to skrypt zostaje przerwany.

	Wspólne kody b³êdów:
         0   - Brak b³êdu
         5   - Ostrze¿enie
         10  - B³±d
         20  - Powa¿ny b³±d

	Standardowo limit wynosi 10.


Przyk³ad
~~~~~~~~
::

	Je¶li mamy skrypt z komendami

         Copy RAM:PewienPlik DF0:
         Echo "Skoñczone!"

	i plik RAM:PewienPlik który nie istnieje

         Copy: object not found
         Copy: returned with error code 20

	skrypt siê zatrzyma. Ale je¶li wcze¶niej zadeklarujesz komende

	FailAt 21
	
	wtedy skrypt zostaje wykonywany dalej dopóki nadal kody b³êdów bêd±
	ni¿sze od limitu.



