===
Set
===

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <search>`_ `Next <setclock>`_ 

---------------

::

 Set 

Synopsis
~~~~~~~~
::


     NAME,STRING/F


Ubicación
~~~~~~~~~
::


     Workbench:c


Función
~~~~~~~
::


     Establece una variable local en el shell actual. Si alguna variable 
     global ya tiene ese nombre se usará la variable local en su lugar.

     La instancia de la variable sólo es accesible desde el shell que la
     definió.

     Si no se especifican parámetros, se muestra la lista actual de 
     variables locales.


Entradas
~~~~~~~~
::


     NAME    - El nombre de la variable local a establecer.

     STRING  - El valor de la variable local NAME.


Resultado
~~~~~~~~~
::


     Los código de error DOS normales.


Ejemplo
~~~~~~~
::


     Set Jump 5

         Establece una variable local llamada "Jump" al valor de "5".


Vea también
~~~~~~~~~~~
::


     Get, Unset


