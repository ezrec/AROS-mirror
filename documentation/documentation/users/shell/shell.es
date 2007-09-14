=====
Shell
=====

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <setkeyboard>`_ `Next <skip>`_ 

---------------

::

 Shell 

Synopsis
~~~~~~~~
::


     COMMAND/K/F,FROM


Ubicación
~~~~~~~~~
::


     Workbench:C


Función
~~~~~~~
::


     Inicia un shell (interactivo o en segundo plano).


Entradas
~~~~~~~~
::


     COMMAND  --  la línea de comandos a ejecutar

     FROM     --  guión a llamar antes de la interacción del usuario.



Ejemplo
~~~~~~~
::


     shell FROM S:Startup-Sequence

     Inicia un shell y ejecuta el guión de inicio.


Notas
~~~~~
::


 El archivo de guión no es un guión en sentido estricto (ya que no podrías
 usar ningún .key, .bra o .ket ni cosas similares).


Mira también
~~~~~~~~~~~~
::


 Execute, NewShell


