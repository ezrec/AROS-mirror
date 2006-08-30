=========================
Работа с Subversion (SVN)
=========================

:Authors:   Aaron Digulla, Adam Chodorowski, Sergey Mineychev 
:Copyright: Copyright щ 1995-2002, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Done.
:Abstract: 
    Субверсия (Subversion или, кратко, SVN) - это инструмент контроля версий 
    файлов, входящих в проект. SVN позволяет отслеживать и управлять 
    изменениями каждого отдельного файла: какие и когда были сделаны изменения, 
    кто их внес, каковы были цели (благодаря наличию лога), отменять ошибочные 
    изменения, объединять код нескольких разных разработчиков и многое 
    другое.
    
    В сущности, всё это позволяет *существенно* упростить работу большой группы
    разработчиков в общих рамках проекта, позволяя каждому быть в курсе изменний
    файлов проекта, исключая возможность ошибочной порчи работы одних людей 
    другими и предоставляя доступ к этим возможностям по сетям Интернет. 
    Разумеется, мы используем SVN в работе над AROS.
    
.. Contents::


Введение
========

На сервере хранится главный "репозиторий", представлющий собой основную базу 
публикуемых данных проекта. Каждый из разработчиков имеет свою "рабочую копию",
то есть, локальную копию базы с изменниями с некоторого времени и изменениями, внесенными самим разработчиком, но ещё не переданными в базу на сервере. Если
разработчик решает внести результат своей работы в "общий котёл", он  вносит 
(commit) свои изменения на сервер с помощью клиентской программы SVN, которая берет на себя заботы по загрузке данных на сервер и объединению их с изменениями от других разработчиков.
в разработке...

Программное обеспечение
=======================

UNIX
----

Если вы работаете в Linux, FreeBSD или другой современной UNIX-системе, тогда 
достаточно просто установить официальное ПО для SVN, версии 1.0 или выше, для вашей ОС. Большинство дистрибутивов Linux уже содержат его.

.. Note:: На сервере работает Subversion 1.1, могут быть использованы
          клиенты версий 1.0, 1.1 или 1.2.

SVN не поддерживает UTF-8 локализацию. Нужно переключить локаль на ISO8859 перед каждым действием с SVN.

AmigaOS
-------

Работая в AmigaOS, вам понадобится установить TCP/IP стэк и любой порт SVN.
Как вариант, можно использовать порт для Amiga от Olaf Barthel, который
можно найти на AmiNET__ (надо искать "subversion").

__ http://main.aminet.net/
                         
Windows
-------

При работе в Microsoft Windows (TM) можно рекомендовать SVN-клиент TortoiseSVN__, который особенно понравится тем, кто привык работать с Проводником. Программа переведена на русский и содержит довольно подробную документацию (пока не русскую). Отправляя ваши файлы, проверьте, установлено ли свойство eol-style: native, если нет - все ваши файлы должны иметь концы строк в стандарте UNIX (только LF), если да - это не имеет значения. Концы строк UNIX также должны быть во всех ваших новых файлах. Чтобы их получить, можно рекомендовать следующие бесплатные программы: редактор TigerPAD__ (сохранять как текст UNIX), DOS Navigator (в свойствах редактора проставить LF), утилиту dos2unix (конвертор).

__ http://tortoisesvn.sourceforge.net
__ http://tigerpad.narod.ru

Доступ к серверу
================

В отличие от CVS, не требуется логиниться на сервер. Вместо этого, SVN запросит ваш логин и пароль по мере надобности.

.. Note:: 

    Репозиторий AROS находится на защищенном паролем сервере SVN, что означает,     что необходима `регистрация для доступа к серверу`__ для возможности участия
    в разработке. По требованию Amiga Inc., анонимный доступ только-на-чтение   
    был отключен.
          
__ contribute#joining-the-team


Получение исходников AROS 
=========================

Чтобы получить рабочую копию кода и документации AROS необходимо использовать команду "checkout" (сверка), например::

    > svn checkout https://svn.aros.org:8080/svn/aros/trunk/AROS

Эта команда создаст директорию с именем AROS и наполнит её всеми исходниками
системы, что может занять значительное время, если ваша сеть медленная. Раздел "contrib" содержит сторонние программы, портированные на AROS. Его нужно сверять, чтобы собирать все виды AROS::

    > cd AROS
    > svn checkout https://svn.aros.org:8080/svn/aros/trunk/contrib

.. Tip:: 

    После сверки (checkout), SVN запомнит источник кода.
    After the checkout, SVN will remember where the source came from.


Получение дополнительного исходного кода
========================================

Отдельно от основных исходников AROS, которые мы сверяле в предыдущем абзаце,
на SVN сервере есть другое содержимое, не связанное прямо с кодом ОС. Например, 
раздел "binaries", содержащий изображения, например, скриншоты, бэкдропы и
и т.п., и раздел "documentation", содержащий исходники для построения содержимого сайта.

Список имеющихся разделов можно получить с помощью следующей команды::

    > svn ls https://svn.aros.org:8080/svn/aros/trunk/

Обновление исходников
======================

После сверки исходников, естественным желанием будет периодическое их обновление для получения последних внесенных изменений. Для этого используется команда "update"::

    > cd AROS
    > svn update
    
This will merge any changes that other developers have made into your sources 
and also check out new directories and files that have been added. If someone
committed changes to a file that you also have changed locally, SVN will try 
to merge the changes automatically. If both of you changed the same lines SVN
might fail in merging the sources. When this happens, SVN will complain and put
**both** versions in the file separated by ``<<<<`` You need to edit the file
and resolve the conflict manually.

.. Warning:: 

    Just because SVN successfully merged the other developers changes with your
    doesn't mean everything is fine. SVN only cares about the *textual* content;
    there could still be *logical* conflicts after the merge (eg. the other
    developer might have changed the semantics of some function that you use in
    your changes). You should always inspect files that were merged and see if
    it still makes sense.


Внесение изменений
==================

If you have made some changes and feel that you want to share your work with 
the other developers, you should use the "commit" command::

    > svn commit

You can specify a list of files to commit; otherwise SVN will recurse down from 
the current directory and find all files you have changed and commit them. 
Before sending your changes to the server for incorporation, SVN will ask you 
to input a log message. This log message should contain a brief description of
what you have changed and in certain cases a rationale for them. Well written 
log messages are very important, since they make it much easier for the other 
developers quickly can see what you have done and perhaps why. The log messages
are collected and then sent in a daily mail to the development mailing list so
everyone can keep up with developments to the code base.

Before committing your changes in a directory, you should first do an update
there to see if anyone else has changed the files in the meantime you've been
working on them. In case that happens, you need to resolve any problems before
committing. Also please make sure you have tested your changes before committing
them; at least so that they do not break the build.

Добавление новых файлов и директорий
====================================

To add new files and directories to the repository, use the "add" command::

    > svn add file.c
    > svn add dir

SVN will not automatically recurse into newly added directories and add the 
contents; you have to do that yourself. After having added the file, you need
to use the "commit" command to actually add them to the repository. 


Импорт
======

When you want to add a larger collection of files, eg. the source code of some
existing software, "svn add" quickly becomes tiresome. For this you should use 
the "svn import" command. Unfortunately, the section about the import command 
in the SVN manual is quite poorly written, so an example is in order:

1. Put the files and directories you want to import wherever you like, as long
   as it is **not** inside your working copy. Running the "import" command on
   a directory situated inside an existing SVN working copy can lead to very
   strange results, so it's best to avoid that.

2. Change to the directory containing the files you wish to import::

       > cd name-1.2.3

3. Import the files with the "svn import" command::

       > svn import -m <logMessage> <destinationPath>

   This will recursively import all files from the current directory and below
   into the repository, into the destination path and with the log message 
   you've specified. Actually, not *all* files will be added: SVN will ignore
   filenames that are common for backup and hidden files, like ``#?.bak``,
   ``.#?`` and ``#?~``.

   Nontheless, you should remove all files which you don't want to end up
   in the repository before you start the import. Don't try to interrupt
   SVN during the import when you see a file being added that you don't want,
   though. Just make a note and then delete the file afterwards.
   
   For example, say that you wanted to import the SVN 1.1.3 sources into 
   the "contrib/development/versioning/svn" directory::

      > cd subversion-1.1.3
      > svn import -m "Initial import of SVN 1.11.12" 
      \ https://svn.aros.org:8080/svn/aros/trunk/contrib/development/versioning/svn

Дополнительная документация
===========================

More detailed information about SVN can of course be found in the manual pages
and info files distributed with SVN itself, and there are also numerous sites
containing useful tutorials and guides which might be more easy to read.
The following pages are highly recommended:

+ `Version Control with Subversion`_
+ `Subversion Home`_

.. _`Version Control with Subversion`: http://svnbook.red-bean.com/
.. _`Subversion Home`:               http://subversion.tigris.org/

