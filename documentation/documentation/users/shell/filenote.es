========
Filenote
========

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <fault>`_ `Next <get>`_ 

---------------

::

 Filenote 

Synopsis
~~~~~~~~
::


     FILE/A,COMMENT,ALL/S,QUIET/S


Ubicación
~~~~~~~~~
::


     Workbench:c


Función
~~~~~~~
::


     Agrega un comentario a un archivo o directorio.

     Filenote permite una revisión recursiva de todos los directorios 
     agregando comentarios a cada archivo/directorio si encuentra que 
     cumple el patrón de archivo especificado.


Entradas
~~~~~~~~
::


     FILE    - Siempre se debe especificar. Puede ser un nombre de archivo
               con la ruta completa o un patrón de archivo que debe ser 
               satisfecho.

     COMMENT - La cadena ASCII que se agregará como un comentario al 
               archivo(s)/directorio(s) especificado.

               Para proporcionar un comentario que incluye comillas, se
               las debe preceder con un asterisco.

                 i.e. Filenote FILE=RAM:test.txt COMMENT=*"hello*"

     ALL     - Parámetro booleano. Si se especifica, Filenote revisa 
               los directorios que satisfacen el patrón especificado,
               recursivamente.

     QUIET   - Parámetro booleano. Si se especifica, no se enviará ningún
               texto de diagnóstico a stdout.


Resultado
~~~~~~~~~
::


     Los códigos de regreso DOS normales.


Ejemplo
~~~~~~~
::


     Filenote ram: hello all

         Recurses through each directory in RAM: adding "hello" as a
         filenote to each file/directory.
         Pasa recursivamente por cada directorio en RAM: agregando
         "hello" como filenote a cada archivo/directorio.


Notas
~~~~~
::


     Output from AROS' Filenote is more neat and structured than the
     standard Filenote command.
     La salida del Filenote de AROS es más ###neat y estructurada que
     la del comando Filenote normal.

     Does not yet support multi-assigns.
     Todavía no soporta multiasignaciones.


También vea
~~~~~~~~~~~
::


     dos.library/SetComment()


