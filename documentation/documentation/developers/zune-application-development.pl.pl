===============================
Zune - podrêcznik programowania
===============================

:Authors:   David Le Corfec, Camelek.AmigaRulez@wp.pl [ AROS Polska Team ]
:Copyright: Copyright © 2004, The AROS Development Team
:Version:   $Revision: 24429 $
:Date:      $Date: 2006-05-07 22:24:40 +0500 (sat, 02 sep 2006) $
:Status:    Unfinished;
:ToDo:      All


.. Contents::


------------
Wprowadzenie
------------

Czym jest Zune?
==============

Zune jest zorientowanym obiektowo zestawem narzêdzi GUI ( Graficznego Interfejsu U¿ytkownika ).
Jest klonem ( w API, i wygl±dzie ) MUI, bardzo dobrze znanym shereware'owym produktem Stenan'a Stuntz'a.
Wiêc programi¶ci MUI bêd± siê czuli jak w domu; pozostali odkryj± koncepcjê i cechy szczególne, które Zune
dzieli z MUI.

+ Programista ma ³atwiejsze zadanie w projektowaniu swojego GUI:
  nie ma potrzeby dla stosowania sta³ych warto¶ci, Zune automatycznie wybiera czcionkê,
  i dostosowuje j± do konkretnego rozmiaru okna, dziêki swojej budowie.
  Musi tylko podaæ schemat swojego GUI dla Zune, które
  dostosuje nisko-poziomowe szczegó³y za niego automatycznie.

+ Efektem ubocznym jest to, ¿e u¿ytkownik musi kontrolowaæ wygl±d i zachowanie ( Look&Feel ) GUI:
  to on decyduje poszczególnymi ustawieniami, które Zune bêdzie u¿ywaæ,
  aby pokazaæ GUI, które zaprojektowa³ programista.

Zune jest oparte o BOOPSI, budowa jest wziêta z AmigaOS,
dla zorientowanego-obiektowo programowania w C. Klasy Zune nie pochodz± od istniej±cych
gad¿etów klas BOOPSI; zamiast tego, klasa notyfikacji ( g³ówna klasa
hierarchi Zune ) pochodzi od g³ównej klasy BOOPSI.

Pomoce
======

Pewna wiedza o OO ( zorientowanym-obiektowo ) programowaniu, jest wiêcej, ni¿ mile widziana.
Je¶li nie, to Google mo¿e pomóc Ci znale¼æ informacje z tego zakresu.

Znajomo¶æ AROS'a ( lub AmigaOS ) API i koncepcji jak listy Tag oraz BOOPSI
jest niezbêdna. Posiadanie Amiga Reference Manual ( RKM - podrêcznik Amigi ) jest bardzo przydatna.

Jako, ¿e Zune jest klonem MUI, ca³a dokumentacja dotycz±ca MUI, dotyczy
tak¿e Zune. W szczególno¶ci ostatni dostêpny pakiet programistyczny, który
znajduje siê tutaj__. W tym archiwum, 2 dokumenty s± szczególnie zalecane:

+ 'MUIdev.guide', dokumentacja dla programistów dot. MUI.
+ `PSI.c`, ¼ród³a aplikacji demonstruj±cej wszystkie nowe 
  czê¶ci MUI, jak zorientowan±-obiektowo budowê, i dynamiczne tworzenie obiektów.

__ http://main.aminet.net/dev/mui/mui38dev.lha

Dodatkowo, to archiwum zawiera opisy funkcji ( autodocs ) MUI, które nawi±zuj±
do wszystkich klas Zune.


---------------
BOOPSI Podstawy
---------------

Koncepcja
=========

Klasa
-----

Klasa mówi sama za siebie, jest nadrzêdn± klas± i dyspozytorem ( dispatcher ).

+ nazwa: nazwa w postaci string dla klas publicznych, wiêc te mog± byæ u¿yte przez
  ka¿dy program w systemie lub ¿adna ( nazwa ) je¶li jest to prywatna klasa u¿yta tylko
  w pojedyñczej aplikacji.

+ klasa nadrzêdna: wszystkie klasy BOOPSI tworz± hierarchiê po³±czon± z
  g³ówn± klas±, trafnie nazwan± rootclass ( klasa g³ówna ). Pozwala to ka¿dej podklasie na
  implementacjê swoich w³asnych wersji specyficznych operacji klas nadrzêdnych lub cofn±æ
  jedn± implementowan± przez jej nadrzêdn±. Znan± jako klasê podstawow± lub super klasê.

+ dyspozytor: daje dostêp do wszystkich operacji ( zwanych metodami ) dostarczanych
  przez t± klasê, upewniaj±c siê, ¿e ka¿da operacja jest obs³ugiwana przez odpowiedni
  kod lub przesz³a do swojej super klasy.


Typ BOOPSI dla klasy to ``Class *`` znana jako ``IClass``.

Obiekt
------

Obiekt ma zawarto¶æ klasy: ka¿dy obiekt ma w³asne specyficzne dane, ale wszystkie
obiekty tej samej klasy dziel± to samo zachowanie.
Obiekt ma kilka klas, je¶li policzymy nadrzêdne jego prawdziwych klas,
( najbardziej pochodne ) a¿ do g³ównej klasy.

Typ BOOPSI dla obiektu to ``Object *``. Nie ma pola, do którego mia³by¶
bezpo¶redni dostêp.

Atrybut
-------

Atrybut jest powi±zany z danymi ka¿dego obiektu: nie mo¿esz
mieæ dostêpu do tych danych bezpo¶rednio, mo¿esz tylko ustawiæ, b±d¼ pobraæ atrybuty
dostarczane poprzez obiekt do zmodyfikowania jego wewnêtrznego stanu. Atrybut jest
za³±czony jako Tag (warto¶æ ``ULONG`` lub edytuj z ``TAG_USER``).

``GetAttr()`` i ``SetAttrs()`` s± u¿ywane do modyfikowania atrybutów obiektu.

Atrybuty mog± mieæ jedn± lub wiêcej flag:

+ Rozpoczynaj±cy-ustawialny (``I``) :
  ten atrybut mo¿e byæ podany jak parameter, podczas tworzenia obiektu.
+ Ustawialny (``S``) :
  Mo¿esz ustawiæ ten atrybut kiedy tylko chcesz ( albo ostatecznie, nie tylko tworzenia).
+ Pobieralny (``G``) :
  Mo¿esz pobraæ warto¶æ tego atrybutu.

Metoda
------

Metoda BOOPSI jest funkcj±, która odbiera obiekt, klasê i wiadomo¶æ ( message ) jako parametery:

+ obiekt: obiekt, którego potrzebujesz
+ klasa: rozwa¿ana klasa dla tego obiektu
+ wiadomo¶æ: zawiera ID metody, które okre¶la funkcjê do wywo³ania
  w obrêbie dyspozytora, i jest poprzedzona jego parametrami.

Aby wys³aæ wiadomo¶æ do obiektu, u¿ywaj ``DoMethod()``. U¿yje najpierw prawdziwej
klasy. Je¶li klasa zawiera t± metodê, wtedy jej u¿yje.
W przeciwnym razie bêdzie próbowaæ jej nadrzêdnej klasy, dopóki wiadomo¶æ jest obs³ugiwana lub
g³ówna klasa zosta³a osi±gniêta ( w tym przypadku, nieznana wiadomo¶æ jest niewidocznie
odrzucana ).

Przyk³ady
=========

Zobaczmy proste przyk³ady tej budowy OOP:

Pobieranie atrybutu
-------------------

Bêdziemy pobieraæ zawarto¶æ String MUI::

    void f(Object *string)
    {
        IPTR result;
        
        GetAttr(string, MUIA_String_Contents, &result);
        printf("Zawarto¶æ String to: %s\n", (STRPTR)result);
    }

+ ``Object *`` jest obiektem BOOPSI.
+ ``IPTR`` musi byæ u¿ywany dla wyniku, który mo¿e byæ liczb± ca³kowit± ( int )
  lub wska¼nikiem. `IPTR` jest zawsze zapisywany w pamiêci, wiêc u¿ywanie mniejszego
  typu mo¿e prowadziæ do uszkodzenia pamiêci ( programowej )!
+ Tutaj pytamy o obiekt MUI String dla jego zawarto¶ci: ``MUIA_String_Contents``,
  jak ka¿dy inny atrybut, jest to ``ULONG`` (to jest Tag)

Aplikacje Zune czê¶ciej u¿ywaj± makr ``get()`` i ``XGET()`` zamiast::

    get(string, MUIA_String_Contents, &result);
    
    result = XGET(string, MUIA_String_Contents);


Ustawianie atrybutu
-------------------

Zmieñmy zawarto¶æ naszego String::

    SetAttrs(string, MUIA_String_Contents, (IPTR)"witaj", TAG_DONE);

+ Parametry wska¼ników musz± byæ przypisane do `IPTR`, tak aby unikn±æ ostrzerzeñ.
+ Po parametrze obiektu, lista Tag jest przekierowana do `SetAttrs` i
  musi siê koñczyæ `TAG_DONE`.

Odkryjesz u¿yteczno¶æ makra ``set()``::

    set(string, MUIA_String_Contents, (IPTR)"hello");

Lecz tylko z ``SetAttrs()`` mo¿esz ustawiæ kilka atrybutów naraz::

    SetAttrs(string,
             MUIA_Disabled, TRUE,
             MUIA_String_Contents, (IPTR)"hmmm...",
             TAG_DONE);


Wywo³ywanie metody
------------------

Zobaczmy najczê¶ciej wywo³ywan± metodê w Zune, metodê procesu zdarzeñ
wywo³ywan± w Twojej pêtli g³ównej::

    result = DoMethod(obj, MUIM_Application_NewInput, (IPTR)&sigs);

+ Parametry nie s± list± Tag, i nie koñcz± siê ``TAG_DONE``.
+ Musisz przypisaæ wska¼niki do ``IPTR``, aby unikn±æ ostrzerzeñ.

-------------
Witaj ¦wiecie
-------------

.. Figure:: zune/images/hello.png

    Proste rzeczy najpierw! Wiem, ¿e to siê Wam spodoba.


¬ród³o
======

Przestudiujmy nasz pierwszy prawdziwy przyk³ad::

    // gcc hello.c -lmui
    #include <exec/types.h>
    #include <libraries/mui.h>
    
    #include <proto/exec.h>
    #include <proto/intuition.h>
    #include <proto/muimaster.h>
    #include <clib/alib_protos.h>
    
    int main(void)
    {
        Object *wnd, *app, *but;
    
        // Tworzenie GUI
    	app = ApplicationObject,
    	    SubWindow, wnd = WindowObject,
    		MUIA_Window_Title, "Witaj ¦wiecie!",
    		WindowContents, VGroup,
    		    Child, TextObject,
    			MUIA_Text_Contents, "\33cWitaj ¦wiecie!\nJak siê masz?",
    			End,
    		    Child, but = SimpleButton("_W porz±dku"),
    		    End,
    		End,
    	    End;
    
    	if (app != NULL)
    	{
    	    ULONG sigs = 0;
    
            // Wci¶nij gad¿et zamykania, b±d¼ ESC, aby wyj¶æ
    	    DoMethod(wnd, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
                     (IPTR)app, 2,
                     MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
    
            // Wci¶nij przycisk, aby wyj¶æ
    	    DoMethod(but, MUIM_Notify, MUIA_Pressed, FALSE,
                     (IPTR)app, 2,
                     MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
    
            // Otwórz okno
    	    set(wnd, MUIA_Window_Open, TRUE);

            // Sprawd¼, czy okno zosta³o otwarte
    	    if (XGET(wnd, MUIA_Window_Open))
    	    {
                // Pêtla g³ówna
    		while((LONG)DoMethod(app, MUIM_Application_NewInput, (IPTR)&sigs)
    		      != MUIV_Application_ReturnID_Quit)
    		{
    		    if (sigs)
    		    {
    			sigs = Wait(sigs | SIGBREAKF_CTRL_C);
    			if (sigs & SIGBREAKF_CTRL_C)
    			    break;
    		    }
    		}
    	    }
	    // Zamknij aplikacjê i wszystkie jej obiekty
    	    MUI_DisposeObject(app);
    	}
    	
    	return(0);
    }


Uwagi
=====

Ogólne
------

Nie bêdziemy rêcznie otwieraæ bibliotek, to jest automatycznie robione za nas.

Tworzenie GUI
-------------

U¿ywamy jêzyka opartego o makra, aby w prosty sposób zbudowaæ nasze GUI.
Aplikacje Zune posiadaj± jeden, i tylko jeden obiekt Application::

    :	app = ApplicationObject,

Aplikacja mo¿e mieæ 0, 1 lub wiêcej obiektów Window ( czyli okien ). Najczê¶ciej pojedyñcze::

    :	    SubWindow, wnd = WindowObject,

B±d¼ mi³y, podaj tytu³ okna::

    :		MUIA_Window_Title, "Witaj ¦wiecie!",

Okno musi mieæ jedno, i tylko jedno dziecko ( child ), zwykle grupê. Ta jest pionowa,
a to oznacza, ¿e jej dzieci bêd± ustawione pionowo::

    :		WindowContents, VGroup,

Grupa musi mieæ conajmniej 1 dziecko, a tutaj jest po prostu text::

    :		    Child, TextObject,

Zune akceptuje ró¿ne rodzaje kodów ESC ( tutaj, aby wyrównaæ tekst ) i nowe linie::

    :			MUIA_Text_Contents, "\33cWitaj ¦wiecie!\nJak siê masz?",

Makro ``End`` koñczy ka¿de rozpoczête makro ``xxxObject`` ( tutaj, TextObject)::

    :			End,

Dodajmy drugie dziecko do naszej grupy, przycisk! Wraz ze skrótem klawiaturowym ``w``
rozpoczête poprzez podkre¶lnik::

    :		    Child, but = SimpleButton("_Ok"),

Zakoñcz grupê::

    :		    End,

Zakoñcz okno::

    :		End,

Zakoñcz aplikacjê::

    :	    End;

Kto wiêc jeszcze potrzebuje programu do tworzenia GUI? :]


Kontrola b³êdów
---------------

Je¶li który¶ z obiektów w drzewie aplikacji nie mo¿e zostaæ utworzony, wtedy Zune zamyka
wszystkie obiekty dotychczas stworzone, i tworzenie aplikacji zostaje przerwane. Je¶li nie,
to masz w pe³ni dzia³aj±c± aplikacjê::

    :	if (app != NULL)
    :	{
    :	    ...

Gdy ju¿ skoñczy³e¶, po prostu wywo³aj ``MUI_DisposeObject()`` na swoim
obiekcie aplikacji, aby zamkn±æ wszystkie obiekty z Twojej aplikacji,
i uwolnij wszystkie zasoby::

    :       ...
    :	    MUI_DisposeObject(app);
    :	}


Powiadomienia
-------------

Powiadomienia s± najprostsz± drog± do odpowiedzi na zdarzenia. Zasada?
Chcemy byæ powiadamiani, gdy pewne atrybuty okre¶lonego obiektu
s± ustawione do wybranej warto¶ci::

    :        DoMethod(wnd, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,

Tutaj nas³uchujemy ``MUIA_Window_CloseRequest`` naszego obiektu Window
i bêdziemy powiadamiani, gdy tylko atrybut bêdzie mia³ warto¶æ ``TRUE``.
Wiêc co siê dzieje, gdy powiadomienie wyst±pi? Wtedy wiadomo¶æ jest wysy³ana
do obiektu, i to tutaj mówimy naszej aplikacji o tym, aby zwróci³a 
``MUIV_Application_ReturnID_Quit``, przy kolejnej iteracji pêtli::

    :                 (IPTR)app, 2,
    :                 MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

Jak ju¿ okre¶limy wszystko, czego nam potrzeba, tak teraz musimy podaæ liczbê
dodatkowych parameterów, które dostarczamy do MUIM_Notify: tutaj, 2 parametery.

Dla przycisku, nas³uchujemy jego ``MUIA_Pressed`` atrybutu: jest ustawiony na ``FALSE``
gdy tylko przycisk zostaje *uwolniony* ( odpowiadanie gdy przycisk zostaje wci¶niêty
jest z³ym nawykiem, bo przecie¿ bêdziesz chcia³ anulowaæ wybór, poprzez przesuniêcie
kursora poza przycisk - i chcemy przecie¿ zobaczyæ jak wyg±da, gdy jest wci¶niêty ).
Zadanie jest to samo co poprzednio, wy¶lij wiadomo¶æ do aplikacji::

    :        DoMethod(but, MUIM_Notify, MUIA_Pressed, FALSE,
    :                 (IPTR)app, 2,
    :                 MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);


Otwieranie okna
---------------

Okna nie s± otwarte, a¿ do momentu, w którym je o to nie poprosimy::

    :        set(wnd, MUIA_Window_Open, TRUE);

Je¶li wszystko idzie dobrze, Twoje okno powinno byæ wy¶wietlone w tym momencie. Ale mo¿e
co¶ pój¶æ nie tak! Wiêc nie zapomnij sprawdziæ, poprzez zapytanie atrybutu, czy zwraca `TRUE`::

    :        if (XGET(wnd, MUIA_Window_Open))


Pêtla g³ówna
------------

Pozwól, ¿e zapoznam Ciê - mój ma³y przyjacielu - z idealn± pêtl± zdarzeñ::

    :        ULONG sigs = 0;

Nie zapomnij zinicjowaæ `signals` do `0` ... test pêtli jest metod± `MUIM_Application_NewInput`::

    :        ...
    :        while((LONG) DoMethod(app, MUIM_Application_NewInput, (IPTR)&sigs)
    :              != MUIV_Application_ReturnID_Quit)

Jako wej¶cie pobiera `signals` ze zdarzeñ, które musi przetworzyæ ( wynik z ``Wait()`` lub `0` ),
zmodyfikuje t± warto¶æ do miejsca, gdzie sygna³y Zune czekaj± na ( do nastêpnego ``Wait()``)
i zwróc± warto¶æ. Ten mechanizm zwracania warto¶ci, by³ historycznie jedn± drog± do 
odpowiedzi na zdarzenia, ale by³ brzydki i zosta³ zarzucony w wizji wspania³ych 
zaawansowanych klasach i obiektowo-zorientowanej budowie.

Ca³o¶æ tej pêtli jest ca³kiem pusta, my tylko czekamy na sygna³y
Ctrl-C aby przerwaæ pêtlê::

    :        {
    :            if (sigs)
    :            {
    :                sigs = Wait(sigs | SIGBREAKF_CTRL_C);
    :                if (sigs & SIGBREAKF_CTRL_C)
    :                    break;
    :            }
    :        }


Wniosek
-------

Ten program pozwala Ci na rozpoczêcie zabawy z GUI Zune, lecz na nic wiêcej.

-------------------
Zadania powiadomieñ
-------------------

Jak widzisz w `hello.c`, u¿ywasz `MUIM_Notify` do wywo³ania metody je¶li zostanie
spe³niony okre¶lony warunek.
Je¶li chcesz aby Twoja aplikacja oddzia³ywa³a w okre¶lony sposób na zdarzenia, to
mo¿esz u¿yæ jednego z poni¿szych schematów:

+ `MUIM_Application_ReturnID`: mo¿esz zapytaæ swoj± aplikacjê, aby zwróci³a ID
  przy nastêpnej iteracji, i sprawdzi³a warto¶æ w pêtli. To jest paskudna stara
  metoda robienia takich rzeczy.
+ `MUIM_CallHook`, do wywo³ania standardowych Hook AmigaOS: to jest ¶rednie rozwi±zanie,
  nie jest zorientowane-obiektowo, ale nie tak paskudne.
+ klasy zaawansowane: ta metoda polega na jednej z Twoich klas. To jest najlepsze rozwi±zanie
  jako, ¿e obs³uguje zorientowan±-obiektowo budowê w aplikacjach.
  Zmusza Ciê do stworzenia w³asnej klasy, wiêc mo¿e nie byæ takie ³atwe dla
  pocz±tkuj±cych lub ludzi, którzy nie maj± na to czasu.
