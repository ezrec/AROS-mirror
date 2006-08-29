.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <endskip>`_ `Next <execute>`_ 

---------------


====
Eval
====

Sk³adnia
~~~~~~~~
::


	VALUE1/A,OP,VALUE2/M,TO/K,LFORMAT/K


¦cie¿ka
~~~~~~~
::


	Workbench:C


Funkcja
~~~~~~~
::
	
	Oblicza warto¶ci liczbowe i wy¶wietla wynik. Wynik jest wy¶wietlany na
	standardowe wyj¶cie, je¶li argument TO nie jest podany, który zapisuje
	wyj¶cie do pliku. Dziêki LFORMAT mo¿liwe jest okre¶lenie rodzaju 
	wyj¶cia. Liczby oznaczone 0x lub #x s± rozumiane jako hex, a te # lub 0,
	jako ósemkowe. Znaki alfabetu s± rozpoznawane przez znak apostrofy
	('), i s± obliczane jak ich oznaczenia w ASCII.


Parametry
~~~~~~~~~
::


	VALUE1,
	OP,
	VALUE2      --  Wyra¿enie do obliczenia, oto dozwolone operatory:

		Operator              Symbol
		----------------------------------
		dodawanie		+
		odejmowanie		-
		mno¿enie		*
		dzielenie		/
		modu³			mod, M, m, %
		logiczne i		&
		logiczne lub		|
		zaprzeczenie logiczne	~
		lewy shift		lsh, L, l
		prawy shift		rsh, R, r
		warto¶æ ujemna		-
		alternatywa roz³±czna	xor, X, x
		***warto¶æ bitu		eqv, E, e

	TO          --  Plik w którym zostanie zapisane wyj¶cie
	LFORMAT     --  Rodzaje parametrów takie jak w printf().
			Oto mo¿liwe opcje:
                      
			%x  --  wyj¶cie hex
			%o  --  wyj¶cie ósemkowe
			%n  --  wyj¶cie dziesiêtne
			%c  --  wyj¶cie znakowe (znak ANSI)
                             
			Ustawiaj±c *n w LFORMAT, nowa linia bêdzie utoworzona.


