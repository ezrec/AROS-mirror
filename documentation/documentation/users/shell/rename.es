======
Rename
======

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <relabel>`_ `Next <requestchoice>`_ 

---------------

::

 Rename 

Synopsis
~~~~~~~~
::


     Rename [{FROM}] <name> [TO|AS] <name> [QUIET]

     FROM/A/M,TO=AS/A,QUIET/S


Ubicación
~~~~~~~~~
::


     Workbench/c


Función
~~~~~~~
::


     Renames a directory or file. Rename can also act like the UNIX mv
     command, which moves a file or files to another location on disk.
     Renombra un directorio o un archivo. Rename puede actuar también
     como el comando mv de UNIX, que mueve un archivo(s) a otra ubicación
     en el disco.


Entradas
~~~~~~~~
::


     FROM   --  The name(s) of the file(s) to rename or move. There may
                be many files specified, this is used when moving files
                into a new directory.

     TO|AS  --  The name which we wish to call the file.

     QUIET  --  Suppress any output from the command.
     FROM   --  El(los) nombre(s) del archivo(s) a renombrar o mover.
                Pueden haber especificados muchos archivos; se usa cuando
                se mueven archivos a un nuevo directorio.

     TO|AS  --  El nombre con que queremos llamar al archivo.

     QUIET  --  Suprime cualquier salida del comando.


Resultado
~~~~~~~~~
::


     Standard DOS error codes.
     Los códigos devueltos DOS normales.


Ejemplo
~~~~~~~
::


     Rename letter1.doc letter2.doc letters
     Rename carta1.doc carta2.doc cartas

         Moves letter1.doc and letter2.doc to the directory letters.
         Mueve carta1.doc y carta2.doc al directorio cartas.

     Rename ram:a ram:b quiet
     Rename from ram:a to ram:b quiet
     Rename from=ram:a to=ram:b quiet

         All versions, renames file "a" to "b" and does not output any
         diagnostic information.
         Todas las variantes, renombra el archivo "a" a "b" y no muestra
         ninguna información de diagnóstico.


