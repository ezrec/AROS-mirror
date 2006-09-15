=====
Fault
=====

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <failat>`_ `Next <filenote>`_ 

---------------

::

 Fault 

Формат
~~~~~~
::

     Fault <error number>


Шаблон
~~~~~~
::

     NUMBERS/N/M


Расположение
~~~~~~~~~~~~
::

     INTERNAL


Функции
~~~~~~~~
::

     Fault печатает сообщение, соответствующее заданному номеру ошибки.
     Номеров может быть несколько сразу, но они должны быть разделены пробелами.
     

Примеры
~~~~~~~
::


     1.SYS:> Fault 205
     Fault 205: object not found

         Команда сообщит, что код ошибки 205 означает, что объект на диске не 
         был найден.
