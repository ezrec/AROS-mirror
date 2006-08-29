.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <fault>`_ `Next <get>`_ 

---------------


========
Filenote
========

Sk³adnia
~~~~~~~~
::


	FILE/A,COMMENT,ALL/S,QUIET/S


¦cie¿ka
~~~~~~~
::


	Workbench:c


Funkcja
~~~~~~~
::

	Dodaje komentarz do obiektu.

	Filenote obs³uguje rekurencyjne przeszukiwanie katalogów i dodaje 
	komentarze do ka¿dego pliku/katalogu, a dziêki wzorcom mo¿na okre¶liæ
	pliki.



Parametry
~~~~~~~~~
::


	FILE	-	Zawsze musi byæ podane. Mo¿e to byæ plik z pe³n± ¶cie¿k± 
			albo mo¿e byæ wzorzec.

	COMMENT	-	Tekst ASCII, który mo¿e byæ dodany jako komentarz do 
			obiektu.
			
			Aby stworzyæ komentarz z zamkniêtymi cudzys³owami
			nale¿y poprzedziæ cudzys³ów gwiazdk±.

			Np.
			Filenote FILE=RAM:test.txt COMMENT=*"witaj*"

	ALL     -	Przeszukiwanie rekurencyjne.

	QUIET   -	Cisza, brak wyj¶cia.


Wynik
~~~~~
::


	Standardowe kody b³êdów.


Przyk³ad
~~~~~~~~
::


	Filenote ram: witaj all

	
	Przeszukuje ca³y katalog w RAM: i dodaje "witaj" jako komentarz do
	ka¿dego obiektu.


Opis
~~~~
::

	Wyj¶cie z AROS'owego Filenote jest staranne i strukturalne, ni¿ ze
	standardowej komendy Filenote.
	
	Nie obs³uguje jeszcze wieoprzypisowo¶ci.


Zobacz tak¿e
~~~~~~~~~~~~
::


	dos.library/SetComment()


