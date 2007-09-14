===
Dir
===

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <delete>`_ `Next <echo>`_ 

---------------

::

 Dir [(dir | pattern)] [OPT A | I | D | F] [ALL] [DIRS] [FILES] [INTER]

Synopsis
~~~~~~~~
::


 DIR,OPT/K,ALL/S,DIRS/S,FILES/S,INTER/S


Ubicación
~~~~~~~~~
::


 Workbench:C


Función
~~~~~~~
::


 DIR muestra cada archivo y directorio contenido en el directorio
 actual o en el especificado. Se listan primero los directorios,
 después en orden alfabético, los archivos en dos columnas. Con la 
 combinación CTRL-C se aborta el listado.



Entradas
~~~~~~~~
::


 ALL    --  Muestra todos los subdirectorios y sus archivos de manera
            recursiva.
 DIRS   --  Muestra únicamente los directorios.
 FILES  --  Muestra solamente los archivos.
 INTER  --  Ingresa al modo interactivo.

            El modo de listado interactivo se detiene después de cada
            nombre para mostrar un signo de interrogación para que 
            puedas escribir algún comando. Estos comandos son:

            Return      --  Continúa con el siguiente archivo o directorio.
            E/ENTER     --  Ingresa al directorio.
            DEL/DELETE  --  Borra el archivo o el directorio vacío.
            C/COM       --  Hace que el archivo o el directorio sea la
                            entrada de un comando DOS (que es especificado 
                            después de la C o de COM o más tarde por separado).
            Q/QUIT      --  Sale del modo interactivo.
            B/BACK      --  Retrocede un nivel de directorio.


