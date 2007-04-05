===========
Обновление!
===========

:Автор:   Paolo Besser
:Дата:     2007-04-05

"Нарядный" Wanderer
-------------------

Был сделан важный шаг к более тонкой настройке интерфейса AROS. 
Совместный труд Darius Brewka и Georg Steger - новая система декорации -
позволит пользователю редактировать и изменять стили оформления. 
Для примера использован стиль ICE от Damir Sijakovic, использующийся теперь
в качестве стиля офрмления по умолчанию в ночных сборках. Внешний
вид показан на скриншоте(`а здесь полностью`__):

.. image:: /images/20070405.jpeg
   :alt: AROS decorated desktop
   :align: center

Красиво? Между тем, мы продолжаем работу над внешним видом AROS,
стараясь сделать её более "стильной" и приятной на вид. Так, любые виды
оформления, созданные Вами (такие, как обои рабочего стола, наборы иконок,
элементы управления и прочее) будут приняты с благодарностью. Более подробную 
информацию вы найдёте на `AROS-Exec`__.


Невероятные Новости
-------------------

Michal Schulz работает над драйвером клавиатур для своего USB-стэка.
Он уже опубликовал начальную версию, которая должна работать вполне хорошо
(верно отображаются символы, однако индикаторы пока не работают). Пожалуйста,
учтите, что USB-стэк AROS пока совместим только с UHCI USB-контроллерами.

Файловая система SFS теперь автоматически встраивается в ночные сборки. 
Это позволит пользователям намного более просто монтировать и использовать
разделы с SFS. Павел Федин доработал команду форматирования, добавив поддержку
различных файловых систем. Для того, чтобы отформатировать раздел с SFS, 
необходимо сперва создать его при помощи HDToolBox, сменить его тип на SFS,
и выполнить в оболочке следующую команду:

  FORMAT DRIVE=DH1: NAME=MyVolume

Учтите, пожалуйста, что текущая версия GRUB не сможет загрузить AROS с
раздела SFS. Поэтому придётся создать небольшой раздел FFS (EXT3/FAT) для
файлов ядра и загрузчика, чтобы GRUB смог работать с ними.

Pavel is also working on a new CD filesystem called CDVDFS, which 
currently supports CD and DVD with joliet extensions. It will be 
put into nightlies as soon as some bugs are fixed.

Matthias Rustler has updated AROS LUA to version 5.1.2. `here`__ 
is a list of fixed bugs. Matthias has also created a new 
DepthMenu commodity. When the user right-clicks on the depth 
gadget, it opens a popup menu with all open windows, allowing 
a faster selection.

Staf Verhaegen has improved support for ARexx/Regina scripts. 
There is a new rx command in AROS, waiting to execute all your 
ARexx scripts.

Thanks to Bernd Roesch, ZUNE can now use MUI4 images (.mim files). 
Pavel Fedin has fixed crash in pendisplay class when there's no 
free pens on the screen. Some new Gorilla Icons had been added 
by Paolo Besser.


__ http://aros-exec.org/modules/xcgal/displayimage.php?pid=239
__ http://aros-exec.org/modules/news/article.php?storyid=227
__ http://www.lua.org/bugs.html#5.1.1
