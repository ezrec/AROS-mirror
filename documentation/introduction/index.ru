==========================
Краткое введение в AROS
==========================

:Authors:   Aaron Digulla, Stefan Rieken, Matt Parsons, Adam Chodorowski 
:Copyright: Copyright Љ 1995-2002, The AROS Development Team
:Version:   $Revision: 24430 $
:Date:      $Date: 2006-05-08 04:01:22 +0500 (а†аН, 08 аМаАаЙ 2006) $
:Status:    Almost finished, I think...


.. Include:: index-abstract.en


Цели проекта
=====

Задачей проекта AROS является создание операционной системы (ОС), которая бы:

1. Имела бы максимально возможную совместимость с AmigaOS 3.1.

2. Могла быть портирована на всевозможнейшие виды платформ и процессоров, такие, как x86, PowerPC, Alpha, Sparc, HPPA and other.

3. Была бы совместима с Amiga на бинарном(двоичном) уровне, и с прочим аппаратным обеспечением на уровне исходников 
Should be binary compatible on Amiga and source compatible on any
   other hardware.
  
4. Способна работать как самостоятельная ОС, загружаемая непосредственно с жёсткого диска, а также в эмуляции, открывая окно на имеющейся ОС-хосте с целью разработки ПО и запуска "родных" и Amiga-приложений в одно и то же время.
Can run as a standalone version which boots directly from hard disk and
   as an emulation which opens a window on an existing OS to develop software
   and run Amiga and native applications at the same time.

5. Была удобнее в использовании, чем AmigaOS.
Improves upon the functionality of AmigaOS.

Для достижения этой цели, мы используем ряд приёмов. Прежде всего, это возможности Интернет. Можно участвовать в проекте, даже если Вы можете 
обеспечить всего одну фунцию ОС. Наиболее полная и свежая копия исходников ОС, программ и документации доступна 24 часа в сутки, и в любое время доступна для изменений. Небольшой перечень доступных задач позволяет не делать лишнюю работу.
To reach this goal, we use a number of techniques. First of all, we make
heavy use of the Internet. You can participate in our project even if you
can write only one single OS function. The most current version of the
source is accessible 24 hours per day and patches can be merged into it at
any time.  A small database with open tasks makes sure work is not duplicated.


История
=======
В далеком 1993 году тучи сгустились над благодатным миром Amiga, и несколько 
поклонников ОС, собравшись, решили обсудить, что нужно сделать для поднятия 
престижа любимой ОС. Немедленно была выявлена главная причина полосы неудач Амиги - возпроизводимость ОС, точнее, её недостаток. ОС нужна более широкий ряд платформ, чтобы она стала действительно привлекательной длшя всех, как для пользователей, так и для разработчиков. Были составлены планы достижения этой
высокой цели, включавшие также устранение недостатков AmigaOS, и создание действительно современной ОС. Так появился на свет проект AOS.
Some time back in the year 1993, the situation for the Amiga looked somewhat 
worse than usual and some Amiga fans got together and discussed what should be 
done to increase the acceptance of our beloved machine. Immediately the main 
reason for the missing success of the Amiga became clear: it was propagation,
or rather the lack thereof. The Amiga should get a more widespread basis to 
make it more attractive for everyone to use and to develop for. So plans were
made to reach this goal. One of the plans was to fix the bugs of the AmigaOS, 
another was to make it an modern operating system. The AOS project was born.

Но что же нужно считать недостатками и "багами" ? И как их следует исправлять ?
Каковы основные возможности упомянутой "современной" ОС , как их внедрить в AmigaOS ?
But exactly what was a bug? And how should the bugs be fixed? What are the
features a so-called *modern* OS must have? And how should they be implemented 
into the AmigaOS?

Спустя два года споры всё продолжались, и из-под "пера" создателей не появилось ни единой строчки кода (по крайней мере, никто его не видел). Все обсуждения начинались строчками вида "нам нужно ...", продолжались ответами "поройся же в почте, наконец..." или "так не пойдет, ибо ..." , затем вслед летело "все ты врешь, говорю же ..." и т.п.
Two years later, people were still arguing about this and not even one line of 
code had been written (or at least no one had ever seen that code). Discussions 
were still of the pattern where someone stated that "we must have ..." and 
someone answered "read the old mails" or "this is impossible to do, because ..."
which was shortly followed by "you're wrong because ..." and so on. 

Понемногу ситация начинала доставать, и зимой 1995 года, Арон Дигулла (Aaron Digulla) опубликовал RFC (request for comments) - опрос мнений, в списке рассылки AOS с целью выделения минимально возможного общего мнения. Были предопределены несколько вариантов, и подведенные итоги показали, что практически все желали бы видеть открытую ОС, совместимую с AmigaOS 3.1 (Kickstart 40.68), на базе которой все дальнейшие прения и должны продолжаться,
с целью определения, что же всё-таки возможо, а что - нет .
In the winter of 1995, Aaron Digulla got fed up with this situation and posted 
an RFC (request for comments) to the AOS mailing list in which he asked what the
minimal common ground might be. Several options were given and the conclusion 
was that almost everyone would like to see an open OS which is compatible with
AmigaOS 3.1 (Kickstart 40.68) on which further discussions could be based, 
to see what is possible and what is not.

Работа закипела, и AROS родилась окончательно.
So the work began and AROS was born.

