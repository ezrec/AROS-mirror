====
Wait
====

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <version>`_ `Next <which>`_ 

---------------

::

 Wait [(n)] [SEC | SECS | MIN | MINS] [ UNTIL (time) ]

Synopsis
~~~~~~~~
::


     TIME/N,SEC=SECS/S,MIN=MINS/S,UNTIL/K


Ubicación
~~~~~~~~~
::


     Workbench:C


Función
~~~~~~~
::


     Espera una cantidad de tiempo o hasta un momento especificado.
     Usar Wait sin argumentos espera un segundo.


Entradas
~~~~~~~~
::


     TIME      --  el número de unidades de tiempo a esperar (por omisión en segundos)
     SEC=SECS  --  pone la unidad de tiempo a segundos
     MIN=MINS  --  pone la unidad de tiempo a minutos
     UNTIL     --  espera hasta que se alcanza la hora especificada. La hora
                   se da en el formato HH:MM.


