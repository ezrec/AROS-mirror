======
Prompt
======

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <path>`_ `Next <protect>`_ 

---------------

::

 Prompt 

Synopsis
~~~~~~~~
::


     PROMPT


Ubicación
~~~~~~~~~
::


     Workbench:c


Función
~~~~~~~
::


     Specify the prompt for the current shell.
     Especifica el ###prompt del shell actual.


Entradas
~~~~~~~~
::


     PROMPT  --  The prompt como una cadena. Se pueden usar los siguientes
                 comandos en un tipo de estilo printf.

                 N  --  el número de CLI
                 S  --  el nombre del directorio actual
                 R  --  el código devuelto por la última operación

                 Si no se especifica ningún prompt "%N.%S> " se usa.


Resultado
~~~~~~~~~
::


     Los códigos devueltos DOS normales.


Ejemplo
~~~~~~~
::


     Prompt "Oepir Risti.%N> " da:

     Oepir Risti.10>      (si el número de CLI era 10)


También vea
~~~~~~~~~~~
::


     SetPrompt()


