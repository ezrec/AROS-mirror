=====
Alias
=====

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <addbuffers>`_ `Next <ask>`_ 

---------------

::

 Alias 

Synopsis
~~~~~~~~
::


     NAME,STRING/F
     NOMBRE, CADENA/F


Ubicación
~~~~~~~~~
::


     Workbench:c


Función
~~~~~~~
::


     Alias allows you to create an alternate name for other DOS commands.
     If Alias is used with no parameters, it will display the current
     list of Aliases defined within the current shell.
     Alias te premite crear un nombre alternativo a los comandos DOS.
     Si se usa Alias sin parámetros, mostrará la lista actual de alias
     definidos en el shell actual.

     Usar los corchetes dentro del alias te permite proporcionar el
     'nuevo' comando dos con parámetros.

     Si no se especifican parámetros, se muestra la lista actual de alias.


Entradas
~~~~~~~~
::


     STRING  - The value of the alias NAME.
     CADENA  - El valor del alias NOMBRE.


Resultado
~~~~~~~~~

::


     Los código de error DOS normales.


Ejemplo
~~~~~~~
::


     Alias DF "Type [] number"

         Al tipear "DF S:Shell-Startup" en el shell, estás ejecutando en
         realidad el comando "Type S:Shell-Startup number". Esto mostrará
         los contenidos del archivo S:Shell-Startup en el shell con el
         número de renglón en el margen izquierdo.


También mira
~~~~~~~~~~~~

::


     Unalias


