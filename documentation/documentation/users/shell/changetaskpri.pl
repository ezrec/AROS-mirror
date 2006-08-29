.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <cd>`_ `Next <copy>`_ 

---------------


=============
ChangeTaskPri
=============

Format
~~~~~~
::

	ChangeTaskPri <priority> [ PROCESS <process number> ]


Sk³adnia
~~~~~~~~
::

	PRI=PRIORITY/A/N,PROCESS/K/N


¦cie¿ka
~~~~~~~
::

	Workbench:c


Funkcja
~~~~~~~
::

	Komenda ChangeTaskPri jest u¿ywana do zmiany aktualnego priorytetu
	zadania. Jako, ¿e AROS jest wielozadaniowym systemem operacyjnym,
	mo¿esz okre¶liæ które zadanie bêdzie pobiera³o wiêcej mocy procesora,
	poprzez zmianê jego priorytetu.

	Warto¶æ priorytetu mo¿e byæ w granicach od -128 do 127, jednak
	warto¶ci wiêksze od 4 nie s± zalecane, jako ¿e mog± one zak³ócaæ
	wa¿ne procesy systemowe. Wiêksza warto¶æ daje procesowi wiêksze
	zasoby procesora CPU.

	Mo¿esz u¿ywaæ komendy STATUS, aby sprawdziæ listê zadañ, które s±
	uruchomione i ich numery procesów.
	

Przyk³ad
~~~~~~~~
::

     
	1.SYS:> ChangeTaskPri 1 Process 1

	Ustawia proces 1 na priorytet 1.

	1.SYS:> ChangeTaskPri 1

        Aktualny proces ustawia na priorytet 1.


Zobacz tak¿e
~~~~~~~~~~~~
::

     Status


