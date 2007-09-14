====
Type
====

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <status>`_ `Next <unalias>`_ 

---------------

::

 Type {<file | pattern>} [TO <name>] [OPT H | N] [HEX | NUMBER]

Synopsis
~~~~~~~~
::


     FROM/A/M,TO/K,OPT/K,HEX/S,NUMBER/S


Ubicación
~~~~~~~~~
::


     Workbench:C


Función
~~~~~~~
::


     Muestra el contenido de un archivo.
     

Entradas
~~~~~~~~
::


     FROM   -- uno o más archivos a mostrar
     TO     -- imprime la salida en un archivo
     OPT    -- H o N (vea HEX o NUMBER)
     HEX    -- muestra la salida en formato hexadecimal
     NUMBER -- las líneas están numeradas
               HEX y NUMBER son mutuamente excluyentes


Ejemplo
~~~~~~~
::


     type abc.txt
     type xyz.dat hex
     

