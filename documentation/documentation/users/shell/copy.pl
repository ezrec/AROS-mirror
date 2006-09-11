.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <changetaskpri>`_ `Next <date>`_ 

---------------


====
Copy
====

Sk³adnia
~~~~~~~~
::


	FROM/M, TO, ALL/S, QUIET/S, BUF=BUFFER/K/N, CLONE/S, DATES/S, NOPRO/S,
	COM=COMMENT/S, NOREQ/S,

	PAT=PATTERN/K, DIRECT/S,SILENT/S, ERRWARN/S, MAKEDIR/S, MOVE/S,
	DELETE/S, HARD=HARDLINK/S, SOFT=SOFTLINK/S, FOLNK=FORCELINK/S,
	FODEL=FORCEDELETE/S, FOOVR=FORCEOVERWRITE/S, DONTOVR=DONTOVERWRITE/S,
	FORCE/S


¦cie¿ka
~~~~~~~
::


	Workbench:C


Funkcja
~~~~~~~
::


	Tworzy kopie jednego lub wielu plików w drugi lub wiele plików.


Parametry
~~~~~~~~~
::


	FROM	--	wej¶cie wieloplikowe
	TO	--	docelowy plik lub katalog
	ALL	--	oznacza, ¿e katalogi s± kopiowane rekurencyjnie
	QUIET	--	tryb cichy, brak wyj¶cia na konsole
	BUFFER	--	rozmiar bufora dla COPY (bufor = 512bajty, 
			standardowo 1024 (= 512K))
	CLONE	--	tryb klonowania, wszystkie pliki s± wiernymi kopiami
	DATES	--	kopiuj daty w tym czas
	NOPRO	--	pomiñ kopiowanie bitów ochronnych
	COMMENT	--	kopiuj komentarz
	NOREQ	--	blokuje zapytania
	PATTERN	--	wywiórcze kopiowanie plików, poprzez podanie wzorca
	DIRECT	--	kopiowanie bezpo¶rednie, bez testów
	VERBOSE	--	tryb gadatliwy, podaje wiêcej szczegó³ów
	ERRWARN	--	przerwij, gdy kopiowanie zawiedzie
	MAKEDIR	--	twórz katalogi
	MOVE	--	tryb przenoszenia, usuwa ¼ród³owe pliki
	DELETE	--	tryb usuwania, nie kopiuje, tylko usuwa
	HARDLINK--	stwórz twarde po³±czenie, zamiast kopiuj
	SOFTLINK--	stwórz miêkkie po³±czenie, zamiast kopiuj
	FOLNK	--	linki tak¿e dla katalogów
	FODEL	--	usuñ pliki chronione
	FOOVR	--	nadpisuj chronione pliki
	DONTOVR	--	nie nadpisuj
	FORCE	--	NIE U¯YWAJ. Tylko w celach kompatybilno¶ci


 Bardziej szczegó³owe opisy:

 FROM:
 Pliki ¼ród³owe. Dla katalogów, zawieraj±ce pliki to pliki ¼ród³owe. Mog± u¿ywaæ
 standardowych wzorców.

 TO:
 Plik docelowy lub wiele ¼róde³ dla docelowego katalogu. Docelowe katalogi s±
 tworzone (zawieraj±ce wszystkie potrzebne nadrzêdne katalogi).

 ALL:
 Kopiuje katalogi rekurencyjnie.

 QUIET:
 Tryb cichy, ¿adne informacje nie bêd± przekazywanem ani nawet zapytania.

 BUF=BUFFER:
 Okre¶la liczbê 512 bajtowych buforów dla kopiowania. Standardowo jest to 200
 [100KB pamiêci]. Jeden bufor to minimalny rozmiar, lecz nie powinien byæ 
 wykorzystywany.

 PAT=PATTERN:
 PATTERN okre¶la rodzaj plików, b±d¼ katalogów, dziêki standardowym dos wzorcom.
 Ta opcja jest u¿yteczna z ALL.

	Przyk³ad:
	Gdy potrzebujesz usun±æ wszystkie pliki z rozszerzeniem info to mo¿esz 
	u¿yæ tego przyk³adu : Copy DELETE #? ALL PAT #?.info

 CLONE:
 Pliki zostan± sklonowane, to znaczy, ¿e czas, data, bity ochronne i komentarz
 bêdzie taki sam w pliku ¼ród³owym, jak i w docelowym.

 DATES:
 Informacja o dacie zostanie skopiowana do obiektu docelowego.

 NOPRO:
 ¯adne bity ochrony nie zostan± skopiowane do docelowego obiektu, bêd± mia³y
 standardowe bity Odczytu[r], Zapisu[w], Wykonywania[e] i Usuniêcia[d] [rwed].

 COM=COMMENT:
 Komentarz pliku zostanie skopiowany.

 NOREQ:
 ¯adne dos standardowe zapytania nie zostan± wy¶wietlone, bêd± pomijane.


 DIRECT:
 Niektóre urz±dzenia nie obs³uguj± (typu DOS) pakietów zapytañ. Ta opcja to jest
 tak naprawdê prostym kopiowaniem, bez sprawdzania, tylko bezpo¶rednie 
 kopiowanie.
 Opcje ALL, PAT, CLONE, DATES, NOPRO, COM, MAKEDIR, MOVE, DELETE, HARD,
 SOFT, FOLNK, FODEL, FOOVR, DONTOVR i wielo¼ród³owe kopiowanie nie mo¿e byæ 
 u¿yte wraz z DIRECT, ta opcja wymaga jednego pliku ¼ród³owego i jednego 
 docelowego obiektu.
 Gdy chcesz usun±æ po³±czenie miêkkie, które ju¿ nie wskazuje na istniej±cy
 plik, bêdziesz potrzebowa³ tej opcji.

	Przyk³ad: 'Copy DIRECT text PRT:' aby wydrukowaæ plik nazwany "text".
	COPY automatycznie zajmuje siê takimi przypadkami, lecz mo¿e to Ci siê
	kiedy¶ przydaæ.

 VERBOSE:
 Tryb gadatliwy, dodatkowe informacje o kopiowaniu s± podawane.

 ERRWARN:
 COPY rozpoznaje trzy rodzaje kodów b³êdu:
 5   WARN    Ostrze¿enie, komenda pomija plik i kontynuuje kopiowanie.
 10  ERROR   B³±d, tworzenie obiektu siê nie powiod³o.
 20  FAIL    Powa¿ny b³±d, brak pamiêci, uszkodzenie systemu, komenda
            przerywa swoje dzia³anie.

 Gdy opcja ERRWARN jest u¿ywana, wtedy Ostrze¿enie (WARN) otrzymuje stopieñ B³êdu
 (ERROR). Wiêc dzia³anie w ka¿dym z tych przypadków jest zakañczane.
		
 MAKEDIR:
 Wszystkie ¼ród³a zostaj± wziête jako nazwy katalogów i stworzone w ¶cie¿ce 
 docelowej.

 MOVE:
 Przenoszenie, zamiast pliki kopiowaæ COPY je po prostu przenosi.

 DELETE:
 Ta opcja jest bardzo niebezpieczna, zamiast pliki kopiowaæ s± one usuwane!

 HARD=HARDLINK:
 Podczas kopiowania obiekty s± dowi±zywane jako po³±czenie twarde. Dzia³a tylko
 wtedy, gdy ¼ród³o i docelowa ¶cie¿ka s± na tym samym dysku.
 Gdy opcja ALL jest za³±czona, to katalogi s± tworzone rekurencyjnie, 
 w przeciwnym razie kopiowane s± tylko katalogi.

 SOFT=SOFTLINK:
 Podczas kopiowania katalogów, po³±czenie miêkkie jest tworzone. Te linki mog±
 byæ u¿ywane tak¿e pomiêdzy dwoma ró¿nymi dyskami. Miêkkie po³±czenia mog± byæ
 tworzone tylko dla katalogów, pliki s± pomijane. Opcja FORCELINK jest zawsze
 ustawiona jako prawda (true).
 OPIS:   Po³±czenia miêkkie nie s± wspierane przez system i mog± byæ
        niebezpieczne. Sugerujê ich nie u¿ywaæ!

 FOLNK=FORCELINK:
 Gdy po³±czenie powinno byæ mo¿liwe, to ta opcja jest wymagana. Zobacz sekcjê
 "About links" dla prawdopodobnych b³êdów.

 FODEL=FORCEDELETE:
 Usuwanie zamiast kopiowania, ale wraz z pomijaniem bitów ochronnych.

 FOOVR=FORCEOVERWRITE:
 Nadpisywanie, nawet gdy pliki s± chronione to zostaj± nadpisane.

 DONTOVR=DONTOVERWRITE:
 Ta opcja chroni przed nadpisywaniem.



Zobacz tak¿e
~~~~~~~~~~~~
::


     Delete, Rename, MakeDir, MakeLink


