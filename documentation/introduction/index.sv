===========================
Kort introduktion till AROS
===========================

:Authors:   Aaron Digulla, Stefan Rieken, Matt Parsons, Adam Chodorowski 
:Copyright: Copyright © 1995-2002, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Almost finished, I think...


.. Include:: index-abstract.sv


Mål
===

AROS-projektets mål är att skapa ett operativsystem som:

1. Är så kompatibelt med AmigaOS 3.1 som möjligt.

2. Kan portas till olika former av hårdvaruarkitekturer och processorer,
   såsom x86, PowerPC, Alpha, Sparc, HPPA och andra.

3. Bör vara kompatibel på binär nivå när man kör den på Amiga-hårdvara
   och källkodskompatibel på annan hårdvara.
  
4. Kan köras som en fristående version som startar direkt från hårddisk eller som
   en emulerad version som öppnar ett fönster vilket tillåter att man kan köra Amiga
   och övriga applikationer samtidigt.

5. Förbättrar funktionaliteten hos AmigaOS

För att nå dessa mål använder vi ett antal olika tekniker. First och främst
använder vi Internet. Du kan deltaga i vårat projekt även om bara kan skriva
en enda funktion i operativsystemet. Den mest uppdaterade versionen av källkoden
finns tillgänglig dygnet runt och patchar kan integreras när som helst. En liten
databas med öppna jobb säkerställer att samma jobb inte utförs av flera personer
samtidigt.


Historia
========

Some time back in the year 1993, the situation for the Amiga looked somewhat 
worse than usual and some Amiga fans got together and discussed what should be 
done to increase the acceptance of our beloved machine. Immediately the main 
reason for the missing success of the Amiga became clear: it was propagation,
or rather the lack thereof. The Amiga should get a more widespread basis to 
make it more attractive for everyone to use and to develop for. So plans were
made to reach this goal. One of the plans was to fix the bugs of the AmigaOS, 
another was to make it an modern operating system. The AOS project was born.

But exactly what was a bug? And how should the bugs be fixed? What are the
features a so-called *modern* OS must have? And how should they be implemented 
into the AmigaOS?

Two years later, people were still arguing about this and not even one line of 
code had been written (or at least no one had ever seen that code). Discussions 
were still of the pattern where someone stated that "we must have ..." and 
someone answered "read the old mails" or "this is impossible to do, because ..."
which was shortly followed by "you're wrong because ..." and so on. 

In the winter of 1995, Aaron Digulla got fed up with this situation and posted 
an RFC (request for comments) to the AOS mailing list in which he asked what the
minimal common ground might be. Several options were given and the conclusion 
was that almost everyone would like to see an open OS which is compatible with
AmigaOS 3.1 (Kickstart 40.68) on which further discussions could be based, 
to see what is possible and what is not.

So the work began and AROS was born.

