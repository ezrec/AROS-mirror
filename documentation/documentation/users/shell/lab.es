===
Lab
===

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <join>`_ `Next <list>`_ 

---------------

::

 Lab 

Ubicación
~~~~~~~~~
::


     Workbench:C


Función
~~~~~~~
::


     En un archivo de guión declara una etiqueta. Esta etiqueta puede estar
     referida a un comando Skip.


Ejemplo
~~~~~~~
::


    If NOT EXISTS S:User-Startup
        Skip NoUserSeq
    EndIf

    FailAt 20
    Execute S:User-Startup
    Quit

    Lab NoUserSeq
    Echo "No User-Startup found"


También vea
~~~~~~~~~~~
::


    Skip


