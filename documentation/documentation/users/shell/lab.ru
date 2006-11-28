===
Lab
===

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <join>`_ `Next <list>`_ 

---------------

::

 Lab 

Расположение
~~~~~~~~~~~~
::


     Workbench:C


Функции
~~~~~~~
::

     Обозначает метку в скрипте. На эту ссылку можно ссылаться при использовании
     команды Skip.
     

Примеры
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


См. также
~~~~~~~~~
::


    Skip


