.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <list>`_ `Next <makedir>`_ 

---------------

Hence the
     protection will be reset (to writable) on the next system reboot.

====
Lock
====

Format
~~~~~~
::

	Lock <drive> [ON|OFF] [<passkey>]


Sk³adnia
~~~~~~~~
::

	DRIVE/A,ON/S,OFF/S,PASSKEY


¦cie¿ka
~~~~~~~
::

	Workbench:c


Funkcja
~~~~~~~
::

	Lock sprawi, ¿e okre¶lone urz±dzenie lub partycja bêd± zabezpieczone
	przed zapisem lub odbezpieczone. To zabezpieczenie przed zapisem jest
	obs³ugiwane przez system plików woluminu. W zwi±zku z czym zapis bêdzie
	mo¿liwy po ponownym restarcie.
	Jest tak¿e mo¿liwe podanie has³a, które bêdzie chroni³o przed zmian±
	stanu. To samo has³o, które zosta³o u¿yte do zabezpieczenia jest u¿ywane
	do odbezpieczania. Has³o mo¿e mieæ dowoln± d³ugo¶æ.

	Wolumin MUSI byæ urz±dzeniem lub g³ównym woluminem, a nie przypisem.


Przyk³ad
~~~~~~~~
::

     
	1.SYS:> Lock Work:

	To zablokuje przed zapisem wolumin Work, ale nie zabezpieczy go has³em.
	

	1.SYS:> Lock Work:
	1.SYS:> MakeDir Work:SomeDir
	Can't create directory Work:Test
	MakeDir: Disk is write-protected

	Wolumin jest zablokowany, wiêc nie jest mo¿liwe stworzenie katalogu.
	

	1.SYS:> Lock Work: OFF

	A to go odblokuje.
	

	1.SYS:> Lock Work: MojeHaslo

	Tutaj u¿ywamy has³a "MojeHaslo"


