.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <makelink>`_ `Next <path>`_ 

---------------


========
NewShell
========

Sk³adnia
~~~~~~~~
::


	WINDOW,FROM


¦cie¿ka
~~~~~~~
::


	Workbench:C


Funkcja
~~~~~~~
::

	Tworzy nowe okno CLI w nowym oknie konsoli. To okno stanie siê aktywne.
	Nowe CLI dziedziczy wiêkszo¶æ atrybutów poprzedniej konsoli jak aktualny
	katalog, rozmiar stosu, liniê poleceñ i inne. Jednak kolejne okno jest
	niezale¿ne od poprzedniego.
	Okno nale¿±ce do nowej konsoli mo¿e byæ okre¶lone dziêki s³owu 
	kluczowemu WINDOW.
	


Parametr
~~~~~~~~
::


	WINDOW	--	Okre¶lenie okna konsoli

	X	--	liczba pikseli od lewej krawêdzi ekranu                                
	Y	--	liczba pikseli od górnej krawêdzi ekranu
	WIDTH	--	szeroko¶æ okna konsoli
	HEIGHT	--	wysoko¶æ okna konsoli
	TITLE	--	tekst, który pokazuje siê w pasku tytu³owym
	AUTO	--	okno automatycznie siê pojawia, gdy program potrzebuje
			wyj¶cia lub wej¶cia
	ALT	--	okno uruchamia siê o okre¶lonym rozmiarze i pozycji, gdy
			przycisk powiêkszania zostanie wci¶niêty
	BACKDROP--	okno bez ramek
	CLOSE	--	za³±cza przycisk wyj¶cia
	INACTIVE--	okno nie jest aktywowane przy uruchomieniu
	NOBORDER--	okno bez ramek, tylko p. rozmiaru, g³êbi i powiêkszania 
			s± dostêpne
	NOCLOSE	--	okno nie posiada p. zamykania
	NODEPTH	--	okno nie posiada p. g³êbi
	NODRAG	--	okno nie mo¿e byæ przemieszczane; za³±cza NOCLOSE
	NOSIZE	--	okno nie posiada przycisku rozmiaru
	SCREEN	--	nazwa ekranu publicznego, na którym ma byæ otworzone 
			okno
	SIMPLE	--	tekst nie zostaje usuniêty po zmianie rozmiaru
	SMART	--	tekst jest wymazywany przy zmianie rozmiaru
	WAIT	--	okno mo¿e zostaæ zamkniête po wci¶niêciu p. zamykania
			lub po kombinacji CTRL-\
	FROM	--	plik do wykonania przed uruchomieniem konsoli,je¶li nic
			nie jest podane to S:Shell-Startup zostaje wykonany


Przyk³ad
~~~~~~~~
::


	NewShell "CON:10/10/640/480/Moja w³asna konsola/CLOSE"


