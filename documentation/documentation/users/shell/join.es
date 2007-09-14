====
Join
====

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <info>`_ `Next <lab>`_ 

---------------

::

 Join [FILE] {(file | pattern)} AS|TO (filename)

Synopsis
~~~~~~~~
::


 FILE/M/A,AS=TO/K/A


Ubicación
~~~~~~~~~
::


 Workbench:C


Función
~~~~~~~
::


 Join crea un único gran archivo con todos los archivos listados
 poniéndolos juntos en el orden en que fueron dados. El archivo de destino
 no puede tener el mismo nombre que alguno de los archivos de entrada.
 Tú debes proporcionar un nombre de archivo de destino. Los archivos
 originales permanecen sin cambio. Cualquier cantidad de archivos 
 pueden ser unidos en una operación.


Entradas
~~~~~~~~
::


 FILE   --  los archivos a juntar
 TO=AS  --  el nombre del archivo combinado


