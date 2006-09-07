======
Delete
======

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <date>`_ `Next <dir>`_ 

---------------

::

 Delete 

Шаблон
~~~~~~
::

 
 Delete { (name | pattern) } [ALL] [QUIET] [FORCE]


Расположение
~~~~~~~~~~~~
::


     Workbench/c


Функции
~~~~~~~
::


 Deletes files and directories. You may delete several files and directories
 by listing them separately or by using wildcards. To abort a multiple
 delete, press CTRL-C. Delete will notify the user of which files it
 weren't able to delete.
     Delete cannot delete directories which are not empty unless the
 ALL option is used. To suppresss file and directory names from being
 printed while deleted use the QUIET option. If the 'd' protection bit
 is cleared for a file or directory, it may not be deleted unless the
 FORCE option is used.


Ввод
~~~~
::


 FILE/M/A  --  files or directories to delete (may contain patterns)
 ALL/S     --  recursively delete dirctories
 QUIET/S   --  don't print which files/directories were deleted
 FORCE/S   --  delete files/directories even if they are protected from
               deletion


Примеры
~~~~~~~
::


 Delete RAM:T/#? ALL FORCE

 Deletes all directories and files recursively in the directory RAM:T
 even if they are protected from deletion.



