==========================
Short introduction to AROS
==========================

:Authors:   Aaron Digulla, Stefan Rieken, Matt Parsons, Adam Chodorowski 
:Copyright: Copyright © 1995-2002, The AROS Development Team
:Version:   $Revision: 26120 $
:Date:      $Date: 2007-05-16 03:40:10 +0200 (st, 16 V 2007) $
:Status:    Almost finished, I think...


.. raw:: html

   <h1>Úvod<br><img style="width: 238px; height: 2px;" alt="spacer" src="/images/sidespacer.png"></h1>

AROS Research Operating System je jednoduchý, výkonný a pøizpùsobitelný 
desktopový operaèní systém navr¾ený k tomu, aby vám pomohl vytì¾it maximum
z va¹eho poèítaèe. 
Je to nezávislý, pøenosný a svobodný projekt, sna¾ící se být kompatibilní
s AmigaOS 3.1 na API levelu (jako Wine, ne jako UAE), pøi souèasném zlep¹ování
v mnoha oblastech. Zdrojový kód je dostupný pod open source licencí, 
která ho dovoluje komukoli zdokonalovat.


Cíle
====

Cílem AROS projektu je vytvoøit OS, který:

1. Je kompatibilní, jak je to jen mo¾né, s AmigaOS 3.1.

2. Mù¾e být portován na rùzné druhy hardwarových architektur a
   procesorù, jako je x86, PowerPC, Alpha, Sparc, HPPA a dal¹í.

3. By mìl být binárnì kompatibilní na Amize a zdrojovì kompatibilní na ostatním
   hardwaru.
  
4. Mù¾e bì¾et jako samostatná verze, která bootuje pøímo z pevného disku, a
   jako emulace, která otevøe okno na stávajícím OS k vývoji software
   a bìhu Amigy a nativních aplikací zároveò.

5. Zlep¹í pùvodní funkce systému AmigaOS.

K dosa¾ení tìchto cílù pou¾íváme mno¾ství technik. V první øadì, we make
heavy use of the Internet. Mù¾ete se podílet na na¹em projektu i kdy¾
umíte napsat pouze jednu jedinou funkci OS. Nejaktuálnìj¹í verze
zdroje je dostupná 24 hodin dennì a patche se mohou objevovat kdykoli.
Malá databáze s veøejnými úkoly (open tasks) zaji¹»uje, ¾e práce není duplikovaná.


Historie
========

Nìjaký èas zpátky v roce 1993 vypadala situace pro Amigu ponìkud 
hùøe ne¾ obvykle a nìkteøí fanou¹ci Amigy se radili a diskutovali, co by se mìlo
udìlat, aby vzrostla akceptace na¹ich milovaných strojù. Immediately the main 
reason for the missing success of the Amiga became clear: it was propagation,
or rather the lack thereof. The Amiga should get a more widespread basis to 
make it more attractive for everyone to use and to develop for. So plans were
made to reach this goal. One of the plans was to fix the bugs of the AmigaOS, 
another was to make it an modern operating system. The AOS project was born.

But exactly what was a bug? And how should the bugs be fixed? What are the
features a so-called *modern* OS must have? And how should they be implemented 
into the AmigaOS?

O dva roky pozdìji, people were still arguing about this and not even one line of 
code had been written (or at least no one had ever seen that code). Discussions 
were still of the pattern where someone stated that "we must have ..." and 
someone answered "read the old mails" or "není to mo¾né udìlat, proto¾e..."
which was shortly followed by "plete¹ se, proto¾e..." a tak dále. 

V zimì roku 1995, Aaron Digulla got fed up with this situation and posted 
an RFC (request for comments) to the AOS mailing list in which he asked what the
minimal common ground might be. Several options were given and the conclusion 
was that almost everyone would like to see an open OS which is compatible with
AmigaOS 3.1 (Kickstart 40.68) on which further discussions could be based, 
to see what is possible and what is not.

Tak zaèala práce a AROS se narodil.

