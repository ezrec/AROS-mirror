=======
Version
=======

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <unsetenv>`_ `Next <wait>`_ 

---------------

::

 Version [<library|device|file>] [<version #>] [<revision #>] [FILE] [FULL] [RES]

Synopsis
~~~~~~~~
::


     NAME/M,MD5SUM/S,VERSION/N,REVISION/N,FILE/S,FULL/S,RES/S


Ubicación
~~~~~~~~~
::


     Workbench:C


Función
~~~~~~~
::


     Imprime o revisa la versión y la información de revisión de un
     archivo, biblioteca o dispositivo.
       

Entradas
~~~~~~~~
::


     NAME      -- el nombre de un archivo, biblioteca o dispositivo a 
                  revisar. Si no se especifica, se imprime la versión y
                  revisión de Kickstart.
     MD5SUM    -- FIXME what is that?
     VERSION   -- revisa la versión y devuelve el código de error 5 
                  (advertencia) si la versión del archivo es menor.
     REVISION  -- revisa la revisión y devuelve el código de error 5
                  (advertencia) si la revisión del archivo es menor.
     FILE      -- lee de un archivo e ignora las bibliotecas y dispositivos cargados.
     FULL      -- imprime información adicional
     RES       -- consigue la versión de los comandos residentes
     

