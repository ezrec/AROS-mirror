==========
AddBuffers
==========

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Next <alias>`_ 

---------------

::

 AddBuffers (drive) [(N)]
 AddBuffers (unidad) [(N)]

Synopsis
~~~~~~~~
::


     DRIVE/A, BUFFERS/N
     UNIDAD/A, BÚFERES/N


Ubicación
~~~~~~~~~
::


     Workbench:C


Función
~~~~~~~
::


     Agrega búferes a la lista de búferes disponibles de una unidad
     específica. Agregar búferes acelera el acceso al disco pero tiene
     el inconveniente de usar memoria del sistema (512 bytes por búfer).
     Especificar un número negativo resta búferes de la unidad.
         Si se especifica solamente el argumento UNIDAD, se muestra la
     cantidad de búferes para esa unidad sin cambiar la asignación de
     búferes. 


Entradas
~~~~~~~~

::


     UNIDAD   --  la unidad a alterar su asignación de búferes.
     BÚFERES  --  la cantidad de búferes a agregar (o quitar en caso de
                  que sea un número negativo) a la unidad.


