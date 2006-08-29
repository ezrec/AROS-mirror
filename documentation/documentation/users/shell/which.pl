.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <wait>`_ `Next <why>`_ 

---------------


=====
Which
=====

Sk³adnia
~~~~~~~~
::


	FILE/A, NORES/S, RES/S, ALL/S


¦cie¿ka
~~~~~~~
::


	Workbench:C


Funkcja
~~~~~~~
::

	Znajduje i wypisuje lokalizacjê do okre¶lonego programu lub katalogu.
	Programy rezydentne s± oznaczone jako RESIDENT, je¶li nie to s± 
	oznaczone jak wewnêtrzne INTERNAL.
	
	Which przeszukuje rezydentn± listê, aktualny katalog, ¶cie¿ki 
	wyszukiwania i przypis C:. Je¶li obiekt nie zosta³ znaleziony, wtedy
	zostanie wys³any kod b³êdu 5, lecz ¿aden b³±d nie zostanie wy¶wietlony.


Parametry
~~~~~~~~~
::


	FILE	--	obiekt do wyszukania
	NORES	--	nie zawieraj pr. rezydentnych w wyniku
	RES	--	tylko rezydentne
	ALL	--	szukaj we wszystkich lokalizacjach dla FILE, to 
			spowoduje, ¿e zostanie wyszukiwane we wszystkich 
			dostêpnych lokalizacjach, mo¿e to spowodowaæ, ¿e po 
			kilka razy wy¶wietli po³o¿enie tego samego obiektu, np.
			obiekt wystêpuje w aktualnym katalogu i przypisie C:
			
