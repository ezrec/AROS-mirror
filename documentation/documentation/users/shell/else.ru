====
Else
====

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <echo>`_ `Next <endcli>`_ 

---------------

::

 Else 

Расположение
~~~~~~~~~~~~
::


     Workbench:C


Функции
~~~~~~~
::


     Разделяет блоки 'true' ('правда') и 'false' ('ложь') в условии If.
     Блок, следующий за командой Else выполняется, если состояние 
     предшествовавшего условия If было 'ложь'.
     

Примеры
~~~~~~~
::


     If EXISTS Sys:Devs
         Copy random.device Sys:Devs/
     Else
         Echo "Не могу найти Sys:Devs"
     EndIf


См. также
~~~~~~~~~
::


     If, EndIf


