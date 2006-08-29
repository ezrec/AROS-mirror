=============================
Wprowadzenie do komend AROS'a
=============================

:Authors:   Matthias Rustler 
:Copyright: Copyright © 2006, The AROS Development Team
:Version:   $Revision: 23961 $
:Date:      $Date: 2006-01-08 10:15:43 -0500 (Sun, 08 Jan 2006) $
:Status:    Draft

-----------------

`Indeks <index>`_

-----------------

AROS posiada konsole 'Shell'. 
Mo¿esz j± uruchomiæ z menu Wanderer>Shell. Okno z aktualnym katalogiem zostanie
otworzone. Okno poleceñ bêdzie zawiera³o numer CLI oraz aktualn± ¶cie¿kê.

Shell posiada historiê. Mo¿esz mieæ do niej dostêp poprzez kursor w górê i w 
dó³.

Jest tutaj wiele udogodnieñ. Je¶li wpiszesz pierwsze litery komendy 
lub obiektu i wci¶niesz tabulator, Shell szuka pasuj±cego wyrazu. Je¶li jednak 
bêdzie wiêcej mo¿liwo¶ci to pojawi siê lista ASL z mo¿liwo¶ci± wyboru.

W AROS komendy i nazwy plików mog± byæ pisane mieszanymi [wielkimi/ma³ymi] 
literami.

AROS szuka komend w aktualnym katalogu i ¶cie¿kach wyszukiwania.
Mo¿esz podgl±daæ i zmieniaæ ¶cie¿ki dziêki komendzie `path <path>`_


Niektóre potrzebne komendy
--------------------------
+ `CD <cd>`_		-- zmienia katalog
+ `DIR <dir>`_		-- pokazuje zawarto¶æ katalogu
+ `COPY <copy>`_	-- kopiuje pliki i katalogi
+ `DELETE <delete>`_	-- usuwa pliki i katalogi
+ `INFO <info>`_	-- pokazuje dostepne dyski
+ `MAKEDIR <makedir>`_	-- tworzy katalogi
+ `RENAME <rename>`_	-- zmienia nazwe plikom i katalogom
+ `TYPE <type>`_	-- pokazuje zawarto¶æ plików tekstowych

¦cie¿ka
-------
G³ówna ¶cie¿ka rozpoczyna siê od nazwy i dwukropka (:),
katalogi s± oddzielone uko¶nikiem (/).
Nazwa dysku mo¿e byæ nazw± urz±dzenia (dh0:), woluminem (workbench:) lub
logicznym dyskiem (zobacz `assign <assign>`_ komenda)

::
  
	Przyk³ad: dh0:dir1/dir2/file.dat

Je¶li chcesz dodaæ aktualny katalog do ¶cie¿ek wyszukiwania to mo¿esz to zrobiæ 
po prostu pisz±c path "".

::

	Przyk³ad: copy from ram:x to ""


Sam dwukropek oznacza katalog aktualnej ¶cie¿ki.
Gdy ¶cie¿ka zaczyna siê od dwukropka wtedy wskazuje na g³ówny katalog podanej 
¶cie¿ki.

Jeden uko¶nik (/) oznacza przej¶cie do katalogu wy¿ej, dwa oznaczaj± dwa wy¿ej,
i tak analogicznie.

Gdy nazwa zawiera spacje, nazwy musz± byæ ujête w nawiasy.

::

	Przyk³ad: type "nazwa z odstêpami"

Szablon komend
--------------
Znak zapytania po komendzie pokazuje jej dostêpne opcje. Nastêpnie komenda jest w
trybie, w którym czeka na podanie parametrów.

::

	Przyk³ad: copy ?
	FROM/M,TO/A,ALL/S,QUIET/S,BUF=BUFFER/K/N,CLONE/S,DATES/S,NOPRO/S,COM/S,NOREQ/S

S³owa kluczowe mog± posiadaæ nastêpuj±ce opcje::

	/A -- argument musi byæ podany
	/K -- s³owo kluczowe musi byæ wpisane, gdy jest podany argument
	/S -- prze³±cznik; tylko s³owo kluczowe jest potrzebne
	/N -- argument numeryczny
	/M -- wiêcej ni¿ jeden argument musi byæ podany
	/F -- reszta linii komend
	=  -- skrót; opcjonalnie mo¿esz u¿yæ skrótu

Gdy wywo³ujesz komendê mo¿esz u¿yæ nastêpuj±cej formy::

	Przyk³ad: copy from=a.dat to=b.dat
  
Wzorce
------
Niektóre komendy zezwalaj± na u¿ycie wzorców::

	?  -- jeden znak
	#? -- zero lub wiêcej znaków
	#x -- zero lub wiêcej x
	~  -- zaprzeczenie
	|  -- lub
	() -- grupa
	[] -- zasiêg

	Przyk³ad::

	dir #?.info
	dir #?~(.info)
	dir a(b|c)d
	dir [a-c]e

Przekierowania
--------------

::

	>	przekierowuje do pliku
	>>	przekierowuje do pliku, do³±czaj±c
	<	przekierowuje z pliku, b±d¼ urz±dzenia
	
	Przyk³ad: dir >ram:a 

Potok
-----
Je¶li chcesz przekierowaæ wyj¶cie jednej komendy do drugiej, mo¿esz u¿yæ potoku.
Musisz po³±czyæ komendy takim znakiem \| . Musi byæ przynajmniej jedna spacja
przed i po \|::

	Przyk³ad: dir | innakomenda
	
	
Lecz co je¶li druga komenda chce odzczytaæ wej¶cie z pliku? Rozwi±zanie polega
na u¿yciu nieistniej±cego urz±dzenia 'in:'::
  
	Przyk³ad: dir | more in:  

Urz±dzenia specjalne
--------------------
+ ram:	mo¿esz u¿ywaæ RamDysku jak twardego dysku. Lecz po ponownym uruchomieniu
	zawarto¶æ jest wyczyszczona. 
+ nil:	je¶li nie chcesz aby wyj¶cie z komendy zostawa³o wy¶wietlane, mo¿esz
	u¿yæ urz±dzenia 'nil:'. Przyk³ad Dir >nil:

Uruchamianie w nowym procesie
-----------------------------
Normalnie komenda blokuje Shell, a¿ do jej zakoñczenia. Mo¿esz uruchomiæ komendy
w nowych procesach dziêki 'Run <run>'_ .

::

	Przyk³ad: run dir #?

Pliki .info
-----------
Pliki z rozszerzeniem '.info' odgrywaj± wa¿n± rolê w Wanderer. Zawieraj±
obrazek do ikony i kilka dodatkowych informacji. Gdy korzystasz z komend Shell
musisz pliki '.info' wzi±æ pod uwagê.
