======
FailAt
======

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <execute>`_ `Next <fault>`_ 

---------------

::

 FailAt 

Formato
~~~~~~~
::

     FailAt <limit>


Synopsis
~~~~~~~~
::

     RCLIM/N


Ubicación
~~~~~~~~~
::

     C:


Función
~~~~~~~
::

     FailAt establece el límite para el código devuelto del actual guión
     del shell. Si algún comando del guión devuelve un código de falla
     igual o mayor que éste el guión abortará.

     Los códigos de falla comunes son:
         0   - No hay error
         5   - Advertencia
         10  - Error
         20  - Falla

     El valor normal para el límite del código devuelto es 10.


Ejemplo
~~~~~~~
::

     Si tenemos un guión con los comandos

         Copy RAM:SomeFile DF0:
         Echo "Done!"

     y el archivo RAM:SomeFile no existe, el comando Copy devolverá:

         Copy: object not found
         Copy: returned with error code 20

     y el guión abortará. Sin embargo, si incluyes el comando

         FailAt 21

     entonces el guión se completará porque el código devuelto de Copy
     es menor que el límite para el código devuelto.


