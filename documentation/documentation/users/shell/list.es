###
matching:

====
List
====

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <lab>`_ `Next <lock>`_ 

---------------

::

 List 

Formato
~~~~~~~
::


     List [(dir | pattern | filename)] [ PAT (pattern)] [KEYS] [DATES]
          [NODATES] [TO (name)] [SUB (string)] [SINCE (date)] [UPTO (date)]
          [QUICK] [BLOCK] [NOHEAD] [FILES] [DIRS] [LFORMAT (string)] [ALL]
             

Plantilla
~~~~~~~~~
::


     DIR/M,P=PAT/K,DATES/S,NODATES/S,TO/K,SUB/K,SINCE/K,UPTO/K,QUICK/S,BLOCK/S,NOHEAD/S,FILES/S,DIRS/S,LFORMAT/K,ALL/S


Ubicación
~~~~~~~~~
::


     Workbench:C/
        

Función
~~~~~~~
::


     Da una lista con información detallada sobre los archivos y los 
     directorios en el directorio actual o en el directorio especificado
     por DIR.

     La información para cada archivo o directorio es presentada en un
     renglón separado, conteniendo la siguiente información:
      
     nombre
     tamaño (en bytes)
     bits de protección
     hora y fecha

     

Entradas
~~~~~~~~
::


     DIR           --  El directorio a listar. Si no está, se listará
                       el directorio actual.
     PAT           -- Muestra solamente los archivos que ###matching 'string'
     KEYS          --  Muestra el número de bloque de cada archivo o 
                       directorio.
     DATES         -- Muestra la fecha de creación de los archivos y
                       directorios.
     NODATES       --  No muestra las fechas.
     TO (nombre)   --  Escribe el listado en un archivo en vez de en stdout.
     SUB (cadena)  --  Muestra solamente los archivos que incluyen la
                       subcadena 'cadena'.
     SINCE (fecha) --  Muestra los archivos posteriores a 'fecha'.
     UPTO (fecha)  --  Muestra los archivos anteriores a 'fecha'.
     QUICK         --  Muestra solamente los nombres de los archivos.
     BLOCK         --  Los tamaños de archivo están en bloques de 512 bytes.
     NOHEAD        --  No imprime ninguna cabecera de información.
     FILES         --  Muestra solamente los archivos.
     DIRS          --  Muestra solamente los directorios.
     LFORMAT       --  Especifica la salida de la lista en estilo printf.
     ALL           --  Lista los contenidos de los directorios recursivamente.

     Están disponibles los siguientes atributos de las cadenas de LFORMAT

     %A  --  los atributos del archivo
     %B  --  el tamaño del archivo en bloques en vez de bytes
     %C  --  el comentario del archivo
     %D  --  la fecha de creación
     %E  --  la extensión del archivo
     %F  --  el nombre del volumen
     %K  --  el número de bloques clave del archivo
     %L  --  el tamaño del archivo en bytes
     %M  --  el nombre del archivo sin la extensión
     %N  --  el nombre del archivo
     %P  --  la ruta del archivo
     %S  --  sustituido por %N y %P; ahora obsoleto
     %T  --  la hora de creación

Resultado
~~~~~~~~~
::


     Los códigos devueltos DOS normales.


Ejemplo
~~~~~~~
::


     1> List C:
     Directory "C:" on Wednesday 12-Dec-99
     AddBuffers                  444 --p-rwed 02-Sep-99 11:51:31
     Assign                     3220 --p-rwed 02-Sep-99 11:51:31
     Avail                       728 --p-rwed 02-Sep-99 11:51:31
     Copy                       3652 --p-rwed 02-Sep-99 11:51:31
     Delete                     1972 --p-rwed 02-Sep-99 11:51:31
     Execute                    4432 --p-rwed 02-Sep-99 11:51:31
     List                       5108 --p-rwed 02-Sep-99 11:51:31
     Installer                109956 ----rwed 02-Sep-99 11:51:31
     Which                      1068 --p-rwed 02-Sep-99 11:51:31
     9 files - 274 blocks used        
     

Vea también
~~~~~~~~~~~
::


     Dir


