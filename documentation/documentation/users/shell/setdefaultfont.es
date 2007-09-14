==============
SetDefaultFont
==============

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <setclock>`_ `Next <setenv>`_ 

---------------

::

 SetDefaultFont 

Synopsis
~~~~~~~~
::


     FONTNAME/A,FONTSIZE/N/A,SCREEN/S


Ubicación
~~~~~~~~~
::


     Workbench:C


Función
~~~~~~~
::


     Establece la Fuente del sistema/pantalla por omisión.
     

Entradas
~~~~~~~~
::


     FONTNAME  --  el nombre de la fuente
     FONTSIZE  --  el tamaño de la fuente
     SCREEN    --  si se especifica establece lafuente de pantalla
                   por omisión, de otro modo, establece la misma
                   del sistema.


Ejemplo
~~~~~~~
::


     SetDefaultFont ttcourier 12


Notas
~~~~~
::

     La fuente del sistema por omisión debe ser monoespaciada
     (no proporcional).

