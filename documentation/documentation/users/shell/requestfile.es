===========
RequestFile
===========

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <requestchoice>`_ `Next <run>`_ 

---------------

::

 RequestFile 

Synopsis
~~~~~~~~
::


     DRAWER,FILE/K,PATTERN/K,TITLE/K,POSITIVE/K,NEGATIVE/K,
     ACCEPTPATTERN/K,REJECTPATTERN/K,SAVEMODE/S,MULTISELECT/S,
     DRAWERSONLY/S,NOICONS/S,PUBSCREEN/K,INITIALVOLUMES/S


Ubicación
~~~~~~~~~
::


     Workbench:c


Función
~~~~~~~
::

 
     Crea un requester de archivo. Los archivos seleccionados se mostrarán
     separados por espacios. Si no se selecciona ningún archivo el 
     código devuelto es 5 (advertencia).
 

Entradas
~~~~~~~~
::

     DRAWER          -- el contenido inicial del campo cajón
     FILE            -- el contenido inicial del campo archivo
     PATTERN         -- el contenido del campo patrón (p.e. #?.c)
     TITLE           -- el título del cuadro de diálogo
     POSITIVE        -- la cadena para el botón izquierdo
     NEGATIVE        -- la cadena para el botón derecho
     ACCEPTPATTERN   -- solamente se muestran los archivos con igual patrón
     REJECTPATTERN   -- no se muestran los archivos que satisfacen el patrón
     SAVEMODE        -- el requester se presenta como un requester guardar
     MULTISELECT     -- se puede seleccionar más de un archivo
     DRAWERSONLY     -- solamente se presentan los cajones
     NOICONS         -- no se muestran los archivos de ícono (#?.info)
     PUBSCREEN       -- el requester se abre en la pantalla pública dada
     INITIALVOLUMES  -- muestra los volúmenes
     

Resultado
~~~~~~~~~
::


     Los códigos de error DOS normales.


