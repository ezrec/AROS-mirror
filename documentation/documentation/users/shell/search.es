======
Search
======

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <run>`_ `Next <set>`_ 

---------------

::

 Search 

Synopsis
~~~~~~~~
::


 Search [FROM] {(name | pattern} [SEARCH] (string | pattern) [ALL] 
        [NONUM] [QUIET] [QUICK] [FILE] [PATTERN] [LINES=Number]


Ubicación
~~~~~~~~~
::


 Workbench:C


Función
~~~~~~~
::


 Search busca en los archivos contenidos en el directorio FROM una
 cadena especificada (SEARCH); si el switch ALL está especificado,
 también se busca en los subdirectorios del directorio FROM. Se presentan
 los nombres de todos los archivos que contienen la cadena SEARCH 
 junto con el número de renglón donde se encuentra.
      Si se aprieta CTRL-C, se deja de buscar. CTRL-D abandonará la
      búsqueda en el archivo actual.


Entradas
~~~~~~~~
::


 NONUM    --  no se imprimen los números de renglón
 QUIET    --  no se muestra el nombre del archivo donde se busca
 QUICK    --  la salida es más compacta
 FILE     --  busca un nombre de archivo en vez de una cadena
 PATTERN  --  cuando busca usa el ###matching de patrón
 CASE     --  cuando busca usa un ###matching de patrón sensitivo a las mayúsculas
 LINES    --  se mostrarán los renglones posteriores al renglón que
              cumpla con el patrón.


Resultado
~~~~~~~~~
::


 Si se encuentra el objeto, la bandera de condición es puesta a 0. De
 otro modo es puesta WARN.


