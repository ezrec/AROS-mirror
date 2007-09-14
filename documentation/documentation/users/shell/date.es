====
Date
====

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <copy>`_ `Next <delete>`_ 

---------------

::

 Date [<day>] [<date>] [<time>] [TO | VER <filename>]

Synopsis
~~~~~~~~
::


     DAY,DATE,TIME,TO=VER/K


Ubicación
~~~~~~~~~
::


     Workbench:C


Función
~~~~~~~
::


     Muestra o pone la hora y/o fecha del sistema.


Entradas
~~~~~~~~
::


     DAY  -- pone la fecha por nombre (lunes, martes, ..., mañana, ayer)
     DATE -- pone la fecha con el formato DD--MMM--YY.
             Para MMM ya sea el número o las tres primeras letras del
             mes en inglés.
     TIME -- pone la hora con el formato HH:MM:SS
     TO   -- la salida es enviada a un archivo.


Ejemplo
~~~~~~~
::


     date 2-feb-06
     date 21:10


