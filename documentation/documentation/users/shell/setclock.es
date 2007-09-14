========
SetClock
========

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <set>`_ `Next <setdefaultfont>`_ 

---------------

::

 SetClock 

Formato
~~~~~~~
::

     SetClock {LOAD|SAVE|RESET}


Synopsis
~~~~~~~~
::

     LOAD/S,SAVE/S,RESET/S


Ubicación
~~~~~~~~~
::

     C:SetClock


Función
~~~~~~~
::

     SetClock se puede usar para:
         o Cargar la hora desde el reloj respaldado por la batería,
         o Guardar la hora al reloj respaldado por la batería,
         o Restablecer el reloj respaldado por la batería.


Ejemplo
~~~~~~~
::


     SetClock LOAD

         pondrá la hora del sistema desde el reloj respaldado por la
         batería. En la mayoría de los sistemas esto se hará 
         automáticamente durante el inicio del sistema.

     SetClock SAVE

         pondrá la hora del reloj respaldado por la batería de la
         hora actual del reloj del sistema.

     SetClock RESET

         restablecerá el reloj respaldado por la batería al valor
         del 1 de enero de 1978 00:00:00. Esto hace si tuvo un
         error y no responde a los comandos normales load y save.


Mire también
~~~~~~~~~~~~
::

     C:Date, Prefs/Time


