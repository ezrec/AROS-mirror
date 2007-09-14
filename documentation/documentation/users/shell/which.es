=====
Which
=====

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <wait>`_ `Next <why>`_ 

---------------

::

 Which 

Synopsis
~~~~~~~~
::


     FILE/A, NORES/S, RES/S, ALL/S


Ubicación
~~~~~~~~~
::


     Workbench:C


Función
~~~~~~~
::


     Find and print the location of a specific program or directory.
     Resident programs are marked as RESIDENT if they are not
     interal resident in which case they are marked as INTERNAL.
     Busca e imprime la ubicación de un programa o directorio específico.
     Los programas residentes son marcados como RESIDENTES si no
     son residentes internos, en cuyo caso son marcados como INTERNOS.

     Which busca la lista de residentes, el directorio actual, 
     las rutas de comando y la assignación C:. Si no se encuentra
     el elemento la bandera de condición es puesta a WARN pero
     no se imprime ningún error.


Entradas
~~~~~~~~
::


     FILE   --  el comando o directorio a buscar
     NORES  --  no incluye los programas residentes en la búsqueda
     RES    --  solamente considera los programas residentes
     ALL    --  encuentra todas las ubicaciones de FILE. Esto
                puede causar que imprima la misma ubicación varias
                veces, por ejemplo si el directorio actual es C: y
                FILE se encuentra en C:


