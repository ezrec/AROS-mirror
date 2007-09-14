====
Info
====

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <if>`_ `Next <join>`_ 

---------------

::

 Info 

Synopsis
~~~~~~~~
::


     DISKS/S, VOLS=VOLUMES/S, GOODONLY/S, BLOCKS/S, DEVICES/M


Ubicación
~~~~~~~~~
::


     Workbench:C


Función
~~~~~~~
::


 Muestra información sobre los volúmenes y los dispositivos del sistema
 de archivos. Cuando no tiene argumentos, se presenta la información
 sobre todos los dispositivos y volúmenes hallados en el sistema. Si se
 quiere solamente la información para algunos dispositivos específicos,
 sus nombres deben darse como argumentos.


Entradas
~~~~~~~~
::


 DISKS     --  muestra la información sobre los dispositivos del sistema
               de archivos.
 VOLS      --  muestra la información sobre los volúmenes.
 GOODONLY  --  no muestra información sobre los malo dispositivos o
               volúmenes.
 BLOCKS    --  muestra información adicional sobre el tamaño del bloque
               y la cantidad de bloques usados.
 DEVICES   --  los nombres de los dispositivos sobre los que se informará.


Ejemplo
~~~~~~~
::


 Info

 Unit                 Size    Used    Free Full Errs   State    Type Name
 Foreign harddisk:  964.1M  776.7M  187.4M  81%    0 read/write  OFS Workbench
 RAM:                 8.0M    7.1M    7.1M  12%    0 read/write  OFS Ram Disk


